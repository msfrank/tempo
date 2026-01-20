

#include <ANTLRInputStream.h>

#include <tempo_config/config_result.h>
#include <tempo_config/internal/json_piece.h>
#include <tempo_config/internal/piece_store.h>
#include <tempo_utils/log_console.h>

#include "ConfigLexer.h"

tempo_config::internal::PieceStore::PieceStore()
{
}

tempo_config::internal::PieceStore::~PieceStore()
{
    reset();
}

tempo_config::internal::RootPiece *
tempo_config::internal::PieceStore::root() const
{
    return m_root.get();
}

tempo_utils::Status
tempo_config::internal::PieceStore::parse(
    antlr4::BufferedTokenStream &tokens,
    std::stack<JsonPiece *> &stack)
{
    for (int i = 0; i < tokens.size(); i++) {
        auto *t = tokens.get(i);
        TU_ASSERT (t != nullptr);

        Token token;
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

        auto curr = stack.top();

        switch (token.type) {

            case tcf1::ConfigLexer::TrueKeyword:
            case tcf1::ConfigLexer::FalseKeyword:
            case tcf1::ConfigLexer::NullKeyword:
                {
                    auto piece = std::make_unique<KeywordPiece>(token, std::move(text));
                    TU_RETURN_IF_NOT_OK (curr->append(std::move(piece), stack));
                    break;
                }

            case tcf1::ConfigLexer::HexInteger:
            case tcf1::ConfigLexer::DecimalInteger:
            case tcf1::ConfigLexer::OctalInteger:
            case tcf1::ConfigLexer::HexFloat:
            case tcf1::ConfigLexer::DecimalScientificFloat:
            case tcf1::ConfigLexer::DecimalFixedFloat:
                {
                    auto piece = std::make_unique<NumberPiece>(token, std::move(text));
                    TU_RETURN_IF_NOT_OK (curr->append(std::move(piece), stack));
                    break;
                }

            case tcf1::ConfigLexer::CharLiteral:
            case tcf1::ConfigLexer::StringLiteral:
            case tcf1::ConfigLexer::UrlLiteral:
                {
                    auto piece = std::make_unique<StringPiece>(token, std::move(text));
                    TU_RETURN_IF_NOT_OK (curr->append(std::move(piece), stack));
                    break;
                }

            case tcf1::ConfigLexer::BracketOpen:
                {
                    auto piece = std::make_unique<ArrayPiece>(token);
                    TU_RETURN_IF_NOT_OK (curr->append(std::move(piece), stack));
                    break;
                }

            case tcf1::ConfigLexer::BracketClose:
                {
                    auto piece = std::make_unique<BracketClosePiece>(token);
                    if (curr->type != PieceType::Array)
                        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                            "unexpected JSON");
                    auto *arrayPtr = static_cast<ArrayPiece *>(curr);
                    TU_RETURN_IF_NOT_OK (arrayPtr->finish(std::move(piece), stack));
                    break;
                }

            case tcf1::ConfigLexer::CurlyOpen:
                {
                    auto piece = std::make_unique<ObjectPiece>(token);
                    TU_RETURN_IF_NOT_OK (curr->append(std::move(piece), stack));
                    break;
                }

            case tcf1::ConfigLexer::CurlyClose:
                {
                    auto piece = std::make_unique<CurlyClosePiece>(token);
                    if (curr->type != PieceType::Object)
                        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                            "unexpected JSON");
                    auto *objectPtr = static_cast<ObjectPiece *>(curr);
                    TU_RETURN_IF_NOT_OK (objectPtr->finish(std::move(piece), stack));
                    break;
                }

            case tcf1::ConfigLexer::ColonOperator:
                {
                    auto piece = std::make_unique<ColonPiece>(token);
                    TU_RETURN_IF_NOT_OK (curr->append(std::move(piece), stack));
                    break;
                }

            case tcf1::ConfigLexer::CommaOperator:
                {
                    auto piece = std::make_unique<CommaPiece>(token);
                    TU_RETURN_IF_NOT_OK (curr->append(std::move(piece), stack));
                    break;
                }

            case tcf1::ConfigLexer::SingleLineComment:
            case tcf1::ConfigLexer::MultiLineComment:
                {
                    auto piece = std::make_unique<CommentPiece>(token, std::move(text));
                    TU_RETURN_IF_NOT_OK (curr->append(std::move(piece), stack));
                    break;
                }

            case tcf1::ConfigLexer::EXPRWS:
                {
                    auto piece = std::make_unique<WhitespacePiece>(token, std::move(text));
                    TU_RETURN_IF_NOT_OK (curr->append(std::move(piece), stack));
                    break;
                }

            default:
                return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                    "invalid lexer token");
        }
    }

    return {};
}

tempo_utils::Status
tempo_config::internal::PieceStore::parse(std::string_view utf8)
{
    reset();
    m_root = std::make_unique<RootPiece>();

    antlr4::ANTLRInputStream input(utf8.data(), utf8.size());
    tcf1::ConfigLexer lexer(&input);
    antlr4::BufferedTokenStream tokens(&lexer);
    tokens.fill();

    std::stack<JsonPiece *> stack;
    stack.push(m_root.get());

    TU_RETURN_IF_NOT_OK (parse(tokens, stack));
    if (stack.empty())
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "unexpected parser stack state after parsing");
    auto top = stack.top();
    stack.pop();
    if (top != m_root.get() || !stack.empty())
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "unexpected parser stack state after parsing");

    return {};
}

tempo_utils::Status
tempo_config::internal::PieceStore::insert(const ConfigNode &node, JsonPiece *root)
{
    auto jsonString = node.toString();

    antlr4::ANTLRInputStream input(jsonString.data(), jsonString.size());
    tcf1::ConfigLexer lexer(&input);
    antlr4::BufferedTokenStream tokens(&lexer);
    tokens.fill();

    std::stack<JsonPiece *> stack;
    stack.push(root);

    TU_RETURN_IF_NOT_OK (parse(tokens, stack));
    if (stack.empty())
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "unexpected parser stack state after parsing");
    auto top = stack.top();
    stack.pop();
    if (top != root || !stack.empty())
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "unexpected parser stack state after parsing");
    return {};
}

void
tempo_config::internal::PieceStore::reset()
{
    m_root.reset();
}

tempo_utils::Status
tempo_config::internal::PieceStore::writeJson(std::string &out) const
{
    if (m_root == nullptr)
        return {};
    return m_root->print(out);
}