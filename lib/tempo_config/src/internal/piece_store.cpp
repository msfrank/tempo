

#include <ANTLRInputStream.h>

#include <tempo_config/config_result.h>
#include <tempo_config/internal/json_piece.h>
#include <tempo_config/internal/piece_store.h>
#include <tempo_utils/log_console.h>

#include "ConfigLexer.h"
#include "tempo_config/internal/piece_printer.h"

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

        auto type = t->getType();
        if (type == -1)
            break;
        auto channel = t->getChannel();
        auto offset = t->getStartIndex();
        auto span = (t->getStopIndex() + 1) - offset;
        Token token(type, channel, offset, span);

        auto text = t->getText();

        TU_LOG_V << "token channel=" << (int) token.channel
            << " type=" << (int) token.type
            << " offset=" << (int) token.offset
            << " span=" << (int) token.span
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

tempo_config::internal::JsonPiece *
tempo_config::internal::PieceStore::findParent(const ConfigPath &path, ConfigPathPart &last)
{
    std::vector parts(path.partsBegin(), path.partsEnd());
    last = parts.back();
    parts.pop_back();

    JsonPiece *currPtr = m_root->root;

    for (const auto &part : parts) {
        if (currPtr == nullptr)
            return nullptr;
        switch (part.getType()) {
            case ConfigPathPartType::Index:
                {
                    if (currPtr->type != PieceType::Array)
                        return nullptr;
                    auto *arrayPtr = static_cast<ArrayPiece *>(currPtr);
                    auto index = part.getIndex();
                    if (index < 0 || arrayPtr->elements.size() <= index)
                        return nullptr;
                    auto *elementPtr = arrayPtr->elements.at(index);
                    currPtr = elementPtr->element;
                    break;
                }
            case ConfigPathPartType::Key:
                {
                    if (currPtr->type != PieceType::Object)
                        return nullptr;
                    auto *objectPtr = static_cast<ObjectPiece *>(currPtr);
                    auto key = part.getKey();
                    auto entry = objectPtr->keyIndex.find(key);
                    if (entry == objectPtr->keyIndex.cend())
                        return nullptr;
                    auto index = entry->second;
                    if (index < 0 || objectPtr->members.size() <= index)
                        return nullptr;
                    auto *memberPtr = objectPtr->members.at(index);
                    currPtr = memberPtr->value;
                    break;
                }
            default:
                return nullptr;
        }
    }

    return currPtr;
}

tempo_config::internal::JsonPiece *
tempo_config::internal::PieceStore::find(const ConfigPath &path)
{
    ConfigPathPart lastPart;
    auto *parentPtr = findParent(path, lastPart);
    if (parentPtr == nullptr)
        return nullptr;

    switch (lastPart.getType()) {
        case ConfigPathPartType::Index:
            {
                if (parentPtr->type != PieceType::Array)
                    return nullptr;
                auto *arrayPtr = static_cast<ArrayPiece *>(parentPtr);
                auto index = lastPart.getIndex();
                if (index < 0 || arrayPtr->elements.size() <= index)
                    return nullptr;
                auto *elementPtr = arrayPtr->elements.at(index);
                return elementPtr->element;
            }
        case ConfigPathPartType::Key:
            {
                if (parentPtr->type != PieceType::Object)
                    return nullptr;
                auto *objectPtr = static_cast<ObjectPiece *>(parentPtr);
                auto key = lastPart.getKey();
                auto entry = objectPtr->keyIndex.find(key);
                if (entry == objectPtr->keyIndex.cend())
                    return nullptr;
                auto index = entry->second;
                if (index < 0 || objectPtr->members.size() <= index)
                    return nullptr;
                auto *memberPtr = objectPtr->members.at(index);
                return memberPtr->value;
            }
        default:
            return nullptr;
    }
}

void
tempo_config::internal::PieceStore::reset()
{
    m_root.reset();
}

tempo_utils::Status
tempo_config::internal::PieceStore::writeJson(std::string &out, const PrinterOptions &options) const
{
    if (m_root == nullptr)
        return {};
    return print_root(options, m_root.get(), out);
}