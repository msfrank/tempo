
#include <tempo_config/internal/json_piece.h>

#include "ConfigLexer.h"

tempo_config::internal::JsonPiece::JsonPiece(PieceType type, Token token, std::string_view value)
    : type(type),
      token(token),
      value(value),
      prev(nullptr),
      next(nullptr),
      last(nullptr),
      parent(nullptr)
{
}

tempo_config::internal::JsonPiece *
tempo_config::internal::append_piece(
    std::unique_ptr<JsonPiece> &&piece,
    JsonPiece **head,
    JsonPiece **tail)
{
    TU_ASSERT (piece != nullptr);
    JsonPiece *piecePtr = nullptr;

    if (*head == nullptr) {
        TU_ASSERT (*tail == nullptr);
        piecePtr = piece.release();
        *head = piecePtr;
        *tail = piecePtr;
    } else {
        TU_ASSERT (*tail != nullptr);
        piecePtr = piece.release();
        (*tail)->next = piecePtr;
        piecePtr->prev = *tail;
        *tail = piecePtr;
    }
    piece.reset();
    return piecePtr;
}

tempo_config::internal::LiteralPiece::LiteralPiece(Token token, std::string value)
    : JsonPiece(PieceType::Literal, token, value)
{
}

tempo_utils::Status
tempo_config::internal::LiteralPiece::append(std::unique_ptr<JsonPiece> &&piece, JsonPiece **head, JsonPiece **tail)
{
    return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
        "invalid lexer token");
}

tempo_config::internal::ElementPiece::ElementPiece(Token token)
    : JsonPiece(PieceType::Element, token, "")
{
}

tempo_utils::Status
tempo_config::internal::ElementPiece::append(std::unique_ptr<JsonPiece> &&piece, JsonPiece **head, JsonPiece **tail)
{
    if (expect == Expect::Done)
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "invalid lexer token");

    if (piece->type == PieceType::Comment || piece->type == PieceType::Whitespace) {
        last = append_piece(std::move(piece), head, tail);
        return {};
    }

    switch (piece->type) {
        case PieceType::Literal:
        case PieceType::Array:
        case PieceType::Object:
            {
                if (expect != Expect::Value)
                    return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                        "invalid lexer token");
                piece->parent = this;
                expect = Expect::Comma;
                element = append_piece(std::move(piece), head, tail);
                last = element;
                return {};
            }
        // case PieceType::Comma:
        //     {
        //         if (expect != Expect::Comma)
        //             return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
        //                 "invalid lexer token");
        //         piece->parent = this;
        //         expect = Expect::Done;
        //         last = append_piece(std::move(piece), head, tail);
        //         return {};
        //     }
        default:
            return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                "invalid lexer token");
    }
}

tempo_utils::Status
tempo_config::internal::ElementPiece::finish(const std::unique_ptr<JsonPiece> &piece)
{
    if (expect != Expect::Comma || piece->type != PieceType::Comma)
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "invalid lexer token");
    expect = Expect::Done;
    return {};
}

tempo_config::internal::ArrayPiece::ArrayPiece(Token token)
    : JsonPiece(PieceType::Array, token, "["),
      pending(nullptr)
{
}

tempo_utils::Status
tempo_config::internal::ArrayPiece::append(std::unique_ptr<JsonPiece> &&piece, JsonPiece **head, JsonPiece **tail)
{
    if (pending == nullptr) {
        auto element = std::make_unique<ElementPiece>(piece->token);
        element->parent = this;
        element->index = elements.size();
        pending = (ElementPiece *) append_piece(std::move(element), head, tail);
    }

    switch (piece->type) {
        case PieceType::Literal:
        case PieceType::Array:
        case PieceType::Object:
            {
                if (pending->expect != ElementPiece::Expect::Value)
                    return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                        "invalid lexer token");
                elements.push_back(pending);
                TU_RETURN_IF_NOT_OK (pending->append(std::move(piece), head, tail));
                return {};
            }
        case PieceType::Comment:
        case PieceType::Whitespace:
            {
                TU_RETURN_IF_NOT_OK (pending->append(std::move(piece), head, tail));
                return {};
            }
        case PieceType::Comma:
            {
                TU_RETURN_IF_NOT_OK (pending->finish(piece));
                TU_ASSERT (pending->expect == ElementPiece::Expect::Done);
                pending = nullptr;
                auto *comma = (CommaPiece *) append_piece(std::move(piece), head, tail);
                comma->parent = this;
                separators.push_back(comma);
                return {};
            }
        default:
            return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                "invalid lexer token");
    }
}

tempo_config::internal::MemberPiece::MemberPiece(Token token)
    : JsonPiece(PieceType::Member, token, "")
{
}

tempo_utils::Status
tempo_config::internal::MemberPiece::append(std::unique_ptr<JsonPiece> &&piece, JsonPiece **head, JsonPiece **tail)
{
    if (expect == Expect::Done)
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "invalid lexer token");

    if (piece->type == PieceType::Comment || piece->type == PieceType::Whitespace) {
        last = append_piece(std::move(piece), head, tail);
        return {};
    }

    switch (expect) {
        case Expect::Key:
            {
                if (piece->type != PieceType::Literal || piece->token.type != tcf1::ConfigLexer::StringLiteral)
                    return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                        "invalid lexer token");
                piece->parent = this;
                expect = Expect::Colon;
                key = (LiteralPiece *) append_piece(std::move(piece), head, tail);
                last = key;
                return {};
            }
        case Expect::Colon:
            {
                if (piece->type != PieceType::Colon)
                    return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                        "invalid lexer token");
                piece->parent = this;
                expect = Expect::Value;
                last = append_piece(std::move(piece), head, tail);
                return {};
            }
        case Expect::Value:
            {
                if (piece->type != PieceType::Literal && piece->type != PieceType::Array && piece->type != PieceType::Object)
                    return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                        "invalid lexer token");
                piece->parent = this;
                expect = Expect::Comma;
                value = append_piece(std::move(piece), head, tail);
                last = value;
                return {};
            }
        // case Expect::Comma:
        //     {
        //         if (piece->type != PieceType::Comma)
        //             return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
        //                 "invalid lexer token");
        //         piece->parent = this;
        //         expect = Expect::Done;
        //         last = append_piece(std::move(piece), head, tail);
        //         return {};
        //     }
        default:
            return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                "invalid lexer token");
    }
}

tempo_utils::Status
tempo_config::internal::MemberPiece::finish(const std::unique_ptr<JsonPiece> &piece)
{
    if (expect != Expect::Comma || piece->type != PieceType::Comma)
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "invalid lexer token");
    expect = Expect::Done;
    return {};
}

tempo_config::internal::ObjectPiece::ObjectPiece(Token token)
    : JsonPiece(PieceType::Object, token, "{"),
      pending(nullptr)
{
}

inline std::string strip_quotes(std::string_view s)
{
    TU_ASSERT (s.size() >= 2);
    TU_ASSERT (s[0] == '"' && s[s.size() - 1] == '"');
    return std::string(s.data() + 1, s.size() - 2);
}

tempo_utils::Status
tempo_config::internal::ObjectPiece::append(std::unique_ptr<JsonPiece> &&piece, JsonPiece **head, JsonPiece **tail)
{
    if (pending == nullptr) {
        auto member = std::make_unique<MemberPiece>(piece->token);
        member->parent = this;
        member->index = members.size();
        pending = (MemberPiece *) append_piece(std::move(member), head, tail);
    }

    switch (piece->type) {
        case PieceType::Literal:
        case PieceType::Array:
        case PieceType::Object:
            {
                auto expect = pending->expect;
                TU_RETURN_IF_NOT_OK (pending->append(std::move(piece), head, tail));
                if (expect == MemberPiece::Expect::Key) {
                    auto key = strip_quotes(pending->key->value);
                    members[key] = pending;
                }
                return {};
            }
        case PieceType::Comment:
        case PieceType::Whitespace:
            {
                TU_RETURN_IF_NOT_OK (pending->append(std::move(piece), head, tail));
                return {};
            }
        case PieceType::Colon:
            {
                TU_RETURN_IF_NOT_OK (pending->append(std::move(piece), head, tail));
                if (pending->expect == MemberPiece::Expect::Done) {
                    pending = nullptr;
                }
                return {};
            }
        case PieceType::Comma:
            {
                TU_RETURN_IF_NOT_OK (pending->finish(piece));
                TU_ASSERT (pending->expect == MemberPiece::Expect::Done);
                pending = nullptr;
                auto *comma = (CommaPiece *) append_piece(std::move(piece), head, tail);
                comma->parent = this;
                separators.push_back(comma);
                return {};
            }
        default:
            return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                "invalid lexer token");
    }
}

std::string token_type_to_value(int type)
{
    switch (type)
    {
    case tcf1::ConfigLexer::ColonOperator:
        return ":";
    case tcf1::ConfigLexer::CommaOperator:
        return ",";
    case tcf1::ConfigLexer::BracketClose:
        return "]";
    case tcf1::ConfigLexer::CurlyClose:
        return "}";
    default:
        return "";
    }
}

tempo_config::internal::OperatorPiece::OperatorPiece(PieceType type, Token token, std::string_view value)
    : JsonPiece(type, token, value)
{
}

tempo_config::internal::CommaPiece::CommaPiece(Token token)
    : OperatorPiece(PieceType::Comma, token, ",")
{
}

tempo_utils::Status
tempo_config::internal::CommaPiece::append(std::unique_ptr<JsonPiece> &&piece, JsonPiece **head, JsonPiece **tail)
{
    return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
        "invalid lexer token");
}

tempo_config::internal::ColonPiece::ColonPiece(Token token)
    : OperatorPiece(PieceType::Colon, token, ":")
{
}

tempo_utils::Status
tempo_config::internal::ColonPiece::append(std::unique_ptr<JsonPiece> &&piece, JsonPiece **head, JsonPiece **tail)
{
    return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
        "invalid lexer token");
}

tempo_config::internal::BracketClosePiece::BracketClosePiece(Token token)
    : OperatorPiece(PieceType::BracketClose, token, "]")
{
}

tempo_utils::Status
tempo_config::internal::BracketClosePiece::append(std::unique_ptr<JsonPiece> &&piece, JsonPiece **head, JsonPiece **tail)
{
    return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
        "invalid lexer token");
}

tempo_config::internal::CurlyClosePiece::CurlyClosePiece(Token token)
    : OperatorPiece(PieceType::CurlyClose, token, "}")
{
}

tempo_utils::Status
tempo_config::internal::CurlyClosePiece::append(std::unique_ptr<JsonPiece> &&piece, JsonPiece **head, JsonPiece **tail)
{
    return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
        "invalid lexer token");
}

tempo_config::internal::CommentPiece::CommentPiece(Token token, std::string value)
    : JsonPiece(PieceType::Comment, token, value)
{
}

tempo_utils::Status
tempo_config::internal::CommentPiece::append(std::unique_ptr<JsonPiece> &&piece, JsonPiece **head, JsonPiece **tail)
{
    return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
        "invalid lexer token");
}

tempo_config::internal::WhitespacePiece::WhitespacePiece(Token token, std::string value)
    : JsonPiece(PieceType::Whitespace, token, value)
{
}

tempo_utils::Status
tempo_config::internal::WhitespacePiece::append(std::unique_ptr<JsonPiece> &&piece, JsonPiece **head, JsonPiece **tail)
{
    return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
        "invalid lexer token");
}
