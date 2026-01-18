
#include <ANTLRInputStream.h>

#include <tempo_config/config_editor.h>
#include <tempo_config/config_result.h>
#include <tempo_config/internal/json_piece.h>
#include <tempo_utils/log_console.h>

#include "ConfigLexer.h"

tempo_config::ConfigEditor::ConfigEditor()
    : m_root(nullptr),
      m_head(nullptr),
      m_tail(nullptr)
{
}

tempo_config::ConfigEditor::~ConfigEditor()
{
    reset();
}

tempo_utils::Status
tempo_config::ConfigEditor::parse(std::string_view utf8)
{
    m_utf8 = utf8;

    antlr4::ANTLRInputStream input(m_utf8.data(), m_utf8.size());
    tcf1::ConfigLexer lexer(&input);
    antlr4::BufferedTokenStream tokens(&lexer);
    tokens.fill();

    for (int i = 0; i < tokens.size(); i++) {
        auto *t = tokens.get(i);
        TU_ASSERT (t != nullptr);

        internal::Token token;
        token.type = t->getType();
        if (token.type == -1)
            break;
        token.channel = t->getChannel();
        token.offset = t->getStartIndex();
        token.span = (t->getStopIndex() + 1) - token.offset;

        auto text = t->getText();

        TU_CONSOLE_ERR << "token channel=" << token.channel
            << " type=" << token.type
            << " offset=" << token.offset
            << " span=" << token.span
            << " text='" << text << "'";

        switch (token.type) {

            //
            case tcf1::ConfigLexer::TrueKeyword:
            case tcf1::ConfigLexer::FalseKeyword:
            case tcf1::ConfigLexer::NullKeyword:
            case tcf1::ConfigLexer::HexInteger:
            case tcf1::ConfigLexer::DecimalInteger:
            case tcf1::ConfigLexer::OctalInteger:
            case tcf1::ConfigLexer::HexFloat:
            case tcf1::ConfigLexer::DecimalScientificFloat:
            case tcf1::ConfigLexer::DecimalFixedFloat:
            case tcf1::ConfigLexer::CharLiteral:
            case tcf1::ConfigLexer::StringLiteral:
            case tcf1::ConfigLexer::UrlLiteral:
                {
                    auto piece = std::make_unique<internal::LiteralPiece>(token, std::move(text));
                    TU_RETURN_IF_NOT_OK (push(std::move(piece)));
                    break;
                }

            //
            case tcf1::ConfigLexer::BracketOpen:
                {
                    auto piece = std::make_unique<internal::ArrayPiece>(token);
                    TU_RETURN_IF_NOT_OK (push(std::move(piece)));
                    break;
                }

            //
            case tcf1::ConfigLexer::BracketClose:
                {
                    auto piece = std::make_unique<internal::OperatorPiece>(token);
                    TU_RETURN_IF_NOT_OK (pop(std::move(piece)));
                    break;
                }

            //
            case tcf1::ConfigLexer::CurlyOpen:
                {
                    auto piece = std::make_unique<internal::ObjectPiece>(token);
                    TU_RETURN_IF_NOT_OK (push(std::move(piece)));
                    break;
                }

            //
            case tcf1::ConfigLexer::CurlyClose:
                {
                    auto piece = std::make_unique<internal::OperatorPiece>(token);
                    TU_RETURN_IF_NOT_OK (pop(std::move(piece)));
                    break;
                }

            // operator pieces
            case tcf1::ConfigLexer::ColonOperator:
            case tcf1::ConfigLexer::CommaOperator:
                {
                    auto piece = std::make_unique<internal::OperatorPiece>(token);
                    TU_RETURN_IF_NOT_OK (append(std::move(piece)));
                    break;
                }

            // comment pieces
            case tcf1::ConfigLexer::SingleLineComment:
            case tcf1::ConfigLexer::MultiLineComment:
                {
                    auto piece = std::make_unique<internal::CommentPiece>(token, std::move(text));
                    TU_RETURN_IF_NOT_OK (append(std::move(piece)));
                    break;
                }

            // whitespace piece
            case tcf1::ConfigLexer::EXPRWS:
                {
                    auto piece = std::make_unique<internal::WhitespacePiece>(token, std::move(text));
                    TU_RETURN_IF_NOT_OK (append(std::move(piece)));
                    break;
                }

            default:
                return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                    "invalid lexer token");
        }
    }

    return {};
}

void
tempo_config::ConfigEditor::reset()
{
    m_utf8.clear();
    while (!m_stack.empty()) {
        m_stack.pop();
    }

    m_root = nullptr;
    m_tail = nullptr;

    while (m_head != nullptr) {
        auto *nextPtr = m_head->next;
        delete m_head;
        m_head = nextPtr;
    }
}

std::string
tempo_config::ConfigEditor::toString() const
{
    std::string out;

    //tu_uint32 line = 0;
    //tu_uint32 positionInLine = 0;

    for (auto *currPtr = m_head; currPtr != nullptr; currPtr = currPtr->next) {
        absl::StrAppend(&out, currPtr->value);
    }

    return out;
}

static tempo_config::internal::JsonPiece *append_piece(
    std::unique_ptr<tempo_config::internal::JsonPiece> &&piece,
    tempo_config::internal::JsonPiece **head,
    tempo_config::internal::JsonPiece **tail)
{
    TU_ASSERT (piece != nullptr);
    tempo_config::internal::JsonPiece *piecePtr = nullptr;

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

tempo_utils::Status
tempo_config::ConfigEditor::append(std::unique_ptr<internal::OperatorPiece> &&piece)
{
    if (m_stack.empty())
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "invalid lexer token");
    auto curr = m_stack.top();

    auto *piecePtr = append_piece(std::move(piece), &m_head, &m_tail);
    return curr->append(piecePtr);
}

tempo_utils::Status
tempo_config::ConfigEditor::append(std::unique_ptr<internal::CommentPiece> &&piece)
{
    append_piece(std::move(piece), &m_head, &m_tail);
    return {};
}

tempo_utils::Status
tempo_config::ConfigEditor::append(std::unique_ptr<internal::WhitespacePiece> &&piece)
{
    append_piece(std::move(piece), &m_head, &m_tail);
    return {};
}

tempo_utils::Status
tempo_config::ConfigEditor::push(std::unique_ptr<internal::LiteralPiece> &&piece)
{
    if (m_root == nullptr) {
        TU_ASSERT (m_stack.empty());
        auto *piecePtr = append_piece(std::move(piece), &m_head, &m_tail);
        m_stack.push(piecePtr);
        m_root = piecePtr;
        return {};
    }
    auto *piecePtr = append_piece(std::move(piece), &m_head, &m_tail);
    auto currPtr = m_stack.top();
    return currPtr->append(piecePtr);
}

tempo_utils::Status
tempo_config::ConfigEditor::push(std::unique_ptr<internal::ArrayPiece> &&piece)
{
    if (m_root == nullptr) {
        TU_ASSERT (m_stack.empty());
        auto *piecePtr = append_piece(std::move(piece), &m_head, &m_tail);
        m_stack.push(piecePtr);
        m_root = piecePtr;
        return {};
    }
    auto *piecePtr = append_piece(std::move(piece), &m_head, &m_tail);
    auto currPtr = m_stack.top();
    return currPtr->append(piecePtr);
}

tempo_utils::Status
tempo_config::ConfigEditor::push(std::unique_ptr<internal::ObjectPiece> &&piece)
{
    if (m_root == nullptr) {
        TU_ASSERT (m_stack.empty());
        auto *piecePtr = append_piece(std::move(piece), &m_head, &m_tail);
        m_stack.push(piecePtr);
        m_root = piecePtr;
        return {};
    }
    auto *piecePtr = append_piece(std::move(piece), &m_head, &m_tail);
    auto currPtr = m_stack.top();
    return currPtr->append(piecePtr);
}

tempo_utils::Status
tempo_config::ConfigEditor::pop(std::unique_ptr<internal::OperatorPiece> &&piece)
{
    if (m_stack.empty())
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "stack is empty");
    auto currPtr = m_stack.top();

    switch (piece->token.type) {
        case tcf1::ConfigLexer::BracketClose:
            {
                if (currPtr->type != internal::PieceType::Array)
                    return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                        "invalid lexer token");
                break;
            }
        case tcf1::ConfigLexer::CurlyClose:
            {
                if (currPtr->type != internal::PieceType::Object)
                    return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                        "invalid lexer token");
                break;
            }
        default:
            return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                "invalid lexer token");
    }

    m_stack.pop();
    append_piece(std::move(piece), &m_head, &m_tail);
    return {};
}