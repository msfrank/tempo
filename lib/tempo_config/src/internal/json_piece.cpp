#include <tempo_config/internal/json_piece.h>

#include "ConfigLexer.h"

tempo_config::internal::Token::Token()
    : type(0), channel(0), offset(0), span(0), synthesized(true)
{
}

tempo_config::internal::Token::Token(size_t type, size_t channel, size_t offset, size_t span)
    : type(type), channel(channel), offset(offset), span(span), synthesized(false)
{
}

tempo_config::internal::Token::Token(const Token &other)
    : type(other.type),
      channel(other.channel),
      offset(other.offset),
      span(other.span),
      synthesized(other.synthesized)
{
}

tempo_config::internal::JsonPiece::JsonPiece(PieceType type, Token token, std::string_view value)
    : type(type),
      token(token),
      value(value)
{
}

tempo_utils::Status
tempo_config::internal::JsonPiece::append(std::unique_ptr<JsonPiece>&& piece, std::stack<JsonPiece*>& stack)
{
    return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
        "append not implemented for JSON piece");
}

bool
tempo_config::internal::JsonPiece::isMultiLine() const
{
    return false;
}

tempo_config::internal::RootPiece::RootPiece()
    : JsonPiece(PieceType::Root, {}, ""),
      root(nullptr),
      expect(Expect::Root)
{
}

tempo_config::internal::RootPiece::~RootPiece()
{
    delete root;
    for (auto& piece : before) {
        delete piece;
    }
    for (auto& piece : after) {
        delete piece;
    }
}

tempo_utils::Status
tempo_config::internal::RootPiece::append(std::unique_ptr<JsonPiece>&& piece, std::stack<JsonPiece*>& stack)
{
    bool push = false;
    switch (piece->type) {
        case PieceType::Whitespace:
        case PieceType::Comment:
            {
                if (expect == Expect::Root) {
                    before.push_back(piece.release());
                } else {
                    after.push_back(piece.release());
                }
                return {};
            }
        case PieceType::Array:
        case PieceType::Object:
            push = true;
            [[fallthrough]];
        case PieceType::Keyword:
        case PieceType::Number:
        case PieceType::String:
            {
                if (expect != Expect::Root)
                    return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                        "unexpected JSON; root is already set");
                expect = Expect::Done;
                root = piece.release();
                if (push) {
                    stack.push(root);
                }
                return {};
            }
        default:
            return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                "unexpected JSON; invalid content");
    }
}

bool
tempo_config::internal::RootPiece::isMultiLine() const
{
    for (auto *piece : before) {
        if (piece->isMultiLine())
            return true;
    }
    if (root != nullptr && root->isMultiLine())
        return true;
    for (auto *piece : after) {
        if (piece->isMultiLine())
            return true;
    }
    return false;
}

tempo_config::internal::CommaPiece::CommaPiece(Token token)
    : JsonPiece(PieceType::Comma, token, ",")
{
}

tempo_config::internal::ColonPiece::ColonPiece(Token token)
    : JsonPiece(PieceType::Colon, token, ":")
{
}

tempo_config::internal::BracketClosePiece::BracketClosePiece(Token token)
    : JsonPiece(PieceType::BracketClose, token, "]")
{
}

tempo_config::internal::CurlyClosePiece::CurlyClosePiece(Token token)
    : JsonPiece(PieceType::CurlyClose, token, "}")
{
}

tempo_config::internal::KeywordPiece::KeywordPiece(Token token, std::string value)
    : JsonPiece(PieceType::Keyword, token, value)
{
}

tempo_config::internal::NumberPiece::NumberPiece(Token token, std::string value)
    : JsonPiece(PieceType::Number, token, value)
{
}

tempo_config::internal::StringPiece::StringPiece(Token token, std::string value)
    : JsonPiece(PieceType::String, token, value)
{
}

std::string
tempo_config::internal::StringPiece::unquote() const
{
    TU_ASSERT(value.size() >= 2);
    TU_ASSERT(value[0] == '"' && value[value.size() - 1] == '"');
    return std::string(value.data() + 1, value.size() - 2);
}

tempo_config::internal::ElementPiece::ElementPiece(Token token)
    : JsonPiece(PieceType::Element, token, ""),
      element(nullptr),
      expect(Expect::Value)
{
}

tempo_config::internal::ElementPiece::~ElementPiece()
{
    for (auto *piece : before) {
        delete piece;
    }
    for (auto *piece : after) {
        delete piece;
    }
    delete element;
}

tempo_utils::Status
tempo_config::internal::ElementPiece::append(std::unique_ptr<JsonPiece>&& piece, std::stack<JsonPiece*>& stack)
{
    bool push = false;
    switch (piece->type) {
        case PieceType::Whitespace:
        case PieceType::Comment:
            {
                if (expect == Expect::Value) {
                    before.push_back(piece.release());
                } else {
                    after.push_back(piece.release());
                }
                return {};
            }
        case PieceType::Array:
        case PieceType::Object:
            push = true;
            [[fallthrough]];
        case PieceType::Keyword:
        case PieceType::Number:
        case PieceType::String:
            {
                if (expect != Expect::Value)
                    return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                        "invalid lexer token");
                expect = Expect::Done;
                element = piece.release();
                if (push) {
                    stack.push(element);
                }
                return {};
            }
        default:
            return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                "invalid lexer token");
    }
}

bool
tempo_config::internal::ElementPiece::isMultiLine() const
{
    for (auto *piece : before) {
        if (piece->isMultiLine())
            return true;
    }
    if (element != nullptr && element->isMultiLine())
        return true;
    for (auto *piece : after) {
        if (piece->isMultiLine())
            return true;
    }
    return false;
}

tempo_config::internal::ArrayPiece::ArrayPiece(Token token)
    : JsonPiece(PieceType::Array, token, "["),
      pending(nullptr)
{
}

tempo_config::internal::ArrayPiece::~ArrayPiece()
{
    for (auto *piece : elements) {
        delete piece;
    }
    delete pending;
}

tempo_utils::Status
tempo_config::internal::ArrayPiece::append(std::unique_ptr<JsonPiece>&& piece, std::stack<JsonPiece*>& stack)
{
    if (pending == nullptr) {
        auto element = std::make_unique<ElementPiece>(piece->token);
        pending = element.release();
    }

    switch (piece->type) {
        case PieceType::Comment:
        case PieceType::Whitespace:
        case PieceType::Keyword:
        case PieceType::Number:
        case PieceType::String:
        case PieceType::Array:
        case PieceType::Object:
            {
                TU_RETURN_IF_NOT_OK(pending->append(std::move(piece), stack));
                return {};
            }
        case PieceType::Comma:
            {
                if (pending->expect != ElementPiece::Expect::Done)
                    return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                        "invalid lexer token");
                elements.push_back(pending);
                pending = nullptr;
                return {};
            }
        default:
            return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                "invalid lexer token");
    }
}

tempo_utils::Result<tempo_config::internal::ElementPiece*>
tempo_config::internal::ArrayPiece::insert(int index, bool after)
{
    if (index < 0 || elements.size() <= index)
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "array insert index {} is out of bounds", index);

    // allocate the element piece
    Token token;
    auto element = std::make_unique<ElementPiece>(token);

    // initialize iterator to the insert point
    index = index + (after ? 1 : 0);
    auto it = std::next(elements.begin(), index);

    // insert element and release ownership but keep pointer to element
    elements.insert(it, element.get());
    auto* elementPtr = element.release();
    return elementPtr;
}

tempo_utils::Status
tempo_config::internal::ArrayPiece::remove(int index)
{
    if (index < 0 || elements.size() <= index)
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "no array element at index {}", index);
    auto it = std::next(elements.cbegin(), index);
    auto *removedPtr = *it;
    delete removedPtr;
    elements.erase(it);
    return {};
}

tempo_utils::Status
tempo_config::internal::ArrayPiece::finish(const std::unique_ptr<JsonPiece>& piece, std::stack<JsonPiece*>& stack)
{
    if (pending != nullptr) {
        if (pending->expect == ElementPiece::Expect::Value) {
            afterElements = std::move(pending->before);
        } else {
            afterElements = std::move(pending->after);
        }
        elements.push_back(pending);
        pending = nullptr;
    }
    if (stack.empty())
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "stack is empty");
    stack.pop();
    return {};
}

bool
tempo_config::internal::ArrayPiece::isMultiLine() const
{
    for (const auto *element : elements) {
        if (element->isMultiLine())
            return true;
    }
    return false;
}

tempo_config::internal::MemberPiece::MemberPiece(Token token)
    : JsonPiece(PieceType::Member, token, ""),
      key(nullptr),
      value(nullptr),
      expect(Expect::Key)
{
}

tempo_config::internal::MemberPiece::MemberPiece(Token token, std::string_view key)
    : JsonPiece(PieceType::Member, token, ""),
      value(nullptr)
{
    this->key = new StringPiece(token, absl::StrCat("\"", key, "\""));
    expect = Expect::Value;
}

tempo_config::internal::MemberPiece::~MemberPiece()
{
    for (auto *piece : before) {
        delete piece;
    }
    for (auto *piece : afterKey) {
        delete piece;
    }
    for (auto *piece : beforeValue) {
        delete piece;
    }
    for (auto *piece : after) {
        delete piece;
    }
    delete key;
    delete value;
}

tempo_utils::Status
tempo_config::internal::MemberPiece::append(std::unique_ptr<JsonPiece>&& piece, std::stack<JsonPiece*>& stack)
{
    bool push = false;
    switch (piece->type) {
        case PieceType::Whitespace:
        case PieceType::Comment:
            {
                switch (expect) {
                case Expect::Key:
                    before.push_back(piece.release());
                    break;
                case Expect::Colon:
                    afterKey.push_back(piece.release());
                    break;
                case Expect::Value:
                    beforeValue.push_back(piece.release());
                    break;
                default:
                    after.push_back(piece.release());
                    break;
                }
                return {};
            }
        case PieceType::String:
            {
                switch (expect) {
                case Expect::Key:
                    {
                        key = (StringPiece*)piece.release();
                        expect = Expect::Colon;
                        break;
                    }
                case Expect::Value:
                    {
                        value = piece.release();
                        expect = Expect::Done;
                        break;
                    }
                default:
                    return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                        "invalid lexer token");
                }
                return {};
            }
        case PieceType::Colon:
            {
                if (expect != Expect::Colon)
                    return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                        "invalid lexer token");
                expect = Expect::Value;
                return {};
            }
        case PieceType::Array:
        case PieceType::Object:
            push = true;
            [[fallthrough]];
        case PieceType::Keyword:
        case PieceType::Number:
            {
                if (expect != Expect::Value)
                    return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                        "invalid lexer token");
                expect = Expect::Done;
                value = piece.release();
                if (push) {
                    stack.push(value);
                }
                return {};
            }
        default:
            return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                "invalid lexer token");
    }
}

bool
tempo_config::internal::MemberPiece::isMultiLine() const
{
    for (auto *piece : before) {
        if (piece->isMultiLine())
            return true;
    }
    if (key != nullptr && key->isMultiLine())
        return true;
    for (auto *piece : afterKey) {
        if (piece->isMultiLine())
            return true;
    }
    for (auto *piece : beforeValue) {
        if (piece->isMultiLine())
            return true;
    }
    if (value != nullptr && value->isMultiLine())
        return true;
    for (auto *piece : after) {
        if (piece->isMultiLine())
            return true;
    }
    return false;
}

tempo_config::internal::ObjectPiece::ObjectPiece(Token token)
    : JsonPiece(PieceType::Object, token, "{"),
      pending(nullptr)
{
}

tempo_config::internal::ObjectPiece::~ObjectPiece()
{
    for (auto *piece : members) {
        delete piece;
    }
    delete pending;
}

tempo_utils::Status
tempo_config::internal::ObjectPiece::append(std::unique_ptr<JsonPiece>&& piece, std::stack<JsonPiece*>& stack)
{
    if (pending == nullptr) {
        auto member = std::make_unique<MemberPiece>(piece->token);
        pending = member.release();
    }

    switch (piece->type) {
        case PieceType::Comment:
        case PieceType::Whitespace:
        case PieceType::Keyword:
        case PieceType::Number:
        case PieceType::String:
        case PieceType::Array:
        case PieceType::Object:
        case PieceType::Colon:
            {
                TU_RETURN_IF_NOT_OK(pending->append(std::move(piece), stack));
                return {};
            }
        case PieceType::Comma:
            {
                if (pending->expect != MemberPiece::Expect::Done)
                    return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                        "invalid lexer token");
                auto index = members.size();
                auto key = pending->key->unquote();
                keyIndex[key] = index;
                members.push_back(pending);
                pending = nullptr;
                return {};
            }
        default:
            return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                "invalid lexer token");
    }
}

tempo_utils::Result<tempo_config::internal::MemberPiece *>
tempo_config::internal::ObjectPiece::insert(std::string_view key)
{
    // allocate the element piece
    Token token;
    auto member = std::make_unique<MemberPiece>(token, key);

    if (keyIndex.contains(key))
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "object already contains member with key '{}'", key);

    // insert member and release ownership but keep pointer to member
    auto index = members.size();
    keyIndex[key] = index;
    members.push_back(member.get());
    auto *memberPtr = member.release();
    return memberPtr;
}

tempo_utils::Status
tempo_config::internal::ObjectPiece::remove(std::string_view key)
{
    auto entry = keyIndex.find(key);
    if (entry == keyIndex.cend())
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "no object member at key '{}'", key);
    auto index = entry->second;
    keyIndex.erase(key);
    auto it = std::next(members.cbegin(), index);
    auto *removedPtr = *it;
    delete removedPtr;
    members.erase(it);
    for (auto &p : keyIndex) {
        if (p.second > index) {
            --p.second;
        }
    }
    return {};
}

tempo_utils::Status
tempo_config::internal::ObjectPiece::finish(const std::unique_ptr<JsonPiece>& piece, std::stack<JsonPiece*>& stack)
{
    if (pending != nullptr) {
        auto index = members.size();
        switch (pending->expect)
        {
            case MemberPiece::Expect::Key:
                {
                    afterMembers = std::move(pending->before);
                    members.push_back(pending);
                    break;
                }
            case MemberPiece::Expect::Done:
                {
                    afterMembers = std::move(pending->after);
                    auto key = pending->key->unquote();
                    keyIndex[key] = index;
                    members.push_back(pending);
                    break;
                }
            default:
                return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                    "invalid lexer token");
        }
        pending = nullptr;
    }
    if (stack.empty())
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
            "stack is empty");
    stack.pop();
    return {};
}

bool
tempo_config::internal::ObjectPiece::isMultiLine() const
{
    for (const auto *member : members) {
        if (member->isMultiLine())
            return true;
    }
    return false;
}

tempo_config::internal::CommentPiece::CommentPiece(Token token, std::string value)
    : JsonPiece(PieceType::Comment, token, value)
{
}

bool
tempo_config::internal::CommentPiece::isMultiLine() const
{
    return value.find('\n') != std::string::npos;
}

tempo_config::internal::WhitespacePiece::WhitespacePiece(Token token, std::string value)
    : JsonPiece(PieceType::Whitespace, token, value)
{
}

bool
tempo_config::internal::WhitespacePiece::isMultiLine() const
{
    return value.find('\n') != std::string::npos;
}
