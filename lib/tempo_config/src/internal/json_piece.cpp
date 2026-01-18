
#include <tempo_config/internal/json_piece.h>

#include "ConfigLexer.h"

tempo_config::internal::JsonPiece::JsonPiece(PieceType type, Token token, std::string_view value)
    : type(type),
      token(token),
      value(value),
      prev(nullptr),
      next(nullptr)
{
}

tempo_config::internal::LiteralPiece::LiteralPiece(Token token, std::string value)
    : JsonPiece(PieceType::Literal, token, value)
{
}

tempo_utils::Status
tempo_config::internal::LiteralPiece::append(JsonPiece *piece)
{
    return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
        "invalid lexer token");
}

tempo_config::internal::ArrayPiece::ArrayPiece(Token token)
    : JsonPiece(PieceType::Array, token, "["),
      expect(Expect::Element)
{
}

tempo_utils::Status
tempo_config::internal::ArrayPiece::append(JsonPiece *piece)
{
    switch (piece->type) {
        case PieceType::Literal:
        case PieceType::Array:
        case PieceType::Object:
            {
                if (expect != Expect::Element)
                    return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                        "invalid lexer token");
                elements.push_back(piece);
                expect = Expect::Comma;
                return {};
            }
        case PieceType::Operator:
            {
                if (expect != Expect::Comma || piece->token.type != tcf1::ConfigLexer::CommaOperator)
                    return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                        "invalid lexer token");
                expect = Expect::Element;
                return {};
            }
        default:
            return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                "invalid lexer token");
    }
}

tempo_config::internal::ObjectPiece::ObjectPiece(Token token)
    : JsonPiece(PieceType::Object, token, "{"),
      expect(Expect::Key)
{
}

tempo_utils::Status
tempo_config::internal::ObjectPiece::append(JsonPiece *piece)
{
    switch (expect) {
        case Expect::Key:
            {
                if (piece->type != PieceType::Literal || piece->token.type != tcf1::ConfigLexer::StringLiteral)
                    return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                        "invalid lexer token");
                key = piece->value;
                expect = Expect::Colon;
                return {};
            }
        case Expect::Colon:
            {
                if (piece->type != PieceType::Operator || piece->token.type != tcf1::ConfigLexer::ColonOperator)
                    return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                        "invalid lexer token");
                expect = Expect::Value;
                return {};
            }
        case Expect::Value:
            {
                if (piece->type != PieceType::Literal && piece->type != PieceType::Array && piece->type != PieceType::Object)
                    return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                        "invalid lexer token");
                expect = Expect::Comma;
                members[key] = piece;
                key = {};
                return {};
            }
        case Expect::Comma:
            {
                if (piece->type != PieceType::Operator || piece->token.type != tcf1::ConfigLexer::CommaOperator)
                    return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                        "invalid lexer token");
                expect = Expect::Key;
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

tempo_config::internal::OperatorPiece::OperatorPiece(Token token)
    : JsonPiece(PieceType::Operator, token, token_type_to_value(token.type))
{
}

tempo_utils::Status
tempo_config::internal::OperatorPiece::append(JsonPiece *piece)
{
    return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
        "invalid lexer token");
}

tempo_config::internal::CommentPiece::CommentPiece(Token token, std::string value)
    : JsonPiece(PieceType::Comment, token, value)
{
}

tempo_utils::Status
tempo_config::internal::CommentPiece::append(JsonPiece *piece)
{
    return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
        "invalid lexer token");
}

tempo_config::internal::WhitespacePiece::WhitespacePiece(Token token, std::string value)
    : JsonPiece(PieceType::Whitespace, token, value)
{
}

tempo_utils::Status
tempo_config::internal::WhitespacePiece::append(JsonPiece *piece)
{
    return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
        "invalid lexer token");
}
