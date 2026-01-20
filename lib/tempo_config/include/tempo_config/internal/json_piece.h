#ifndef TEMPO_CONFIG_INTERNAL_JSON_PIECE_H
#define TEMPO_CONFIG_INTERNAL_JSON_PIECE_H

#include <tempo_config/config_result.h>

namespace tempo_config::internal {

    enum class PieceType {
        Root,
        Keyword,
        Number,
        String,
        Array,
        Object,
        Element,
        Member,
        Comma,
        Colon,
        BracketClose,
        CurlyClose,
        Comment,
        Whitespace,
    };

    struct Token {
        size_t type;
        size_t channel;
        size_t offset;
        size_t span;
        bool synthesized;
        Token();
        Token(size_t type, size_t channel, size_t offset, size_t span);
        Token(const Token &other);
    };

    struct JsonPiece {
        PieceType type;
        Token token;
        std::string value;

        JsonPiece(PieceType type, Token token, std::string_view value);
        virtual ~JsonPiece() = default;
        virtual tempo_utils::Status append(std::unique_ptr<JsonPiece> &&piece, std::stack<JsonPiece *> &stack);
        virtual tempo_utils::Status print(std::string &out);
    };

    struct RootPiece : JsonPiece {
        JsonPiece *root;
        std::vector<JsonPiece *> before;
        std::vector<JsonPiece *> after;
        enum class Expect { Root, Done } expect;
        RootPiece();
        ~RootPiece() override;
        tempo_utils::Status append(std::unique_ptr<JsonPiece> &&piece, std::stack<JsonPiece *> &stack) override;
        tempo_utils::Status print(std::string &out) override;
    };

    struct CommaPiece : JsonPiece {
        explicit CommaPiece(Token token);
    };

    struct ColonPiece : JsonPiece {
        explicit ColonPiece(Token token);
    };

    struct BracketClosePiece : JsonPiece {
        explicit BracketClosePiece(Token token);
    };

    struct CurlyClosePiece : JsonPiece {
        explicit CurlyClosePiece(Token token);
    };

    struct KeywordPiece : JsonPiece {
        KeywordPiece(Token token, std::string value);
        tempo_utils::Status print(std::string &out) override;
    };

    struct NumberPiece : JsonPiece {
        NumberPiece(Token token, std::string value);
        tempo_utils::Status print(std::string &out) override;
    };

    struct StringPiece : JsonPiece {
        StringPiece(Token token, std::string value);
        std::string unquote() const;
        tempo_utils::Status print(std::string &out) override;
    };

    struct ElementPiece : JsonPiece {
        JsonPiece *element;
        std::vector<JsonPiece *> before;
        std::vector<JsonPiece *> after;
        enum class Expect { Value, Done } expect;
        explicit ElementPiece(Token token);
        ~ElementPiece() override;
        tempo_utils::Status append(std::unique_ptr<JsonPiece> &&piece, std::stack<JsonPiece *> &stack) override;
        tempo_utils::Status print(std::string &out) override;
    };

    struct ArrayPiece : JsonPiece {
        std::vector<ElementPiece *> elements;
        ElementPiece *pending;
        explicit ArrayPiece(Token token);
        ~ArrayPiece() override;
        tempo_utils::Status append(std::unique_ptr<JsonPiece> &&piece, std::stack<JsonPiece *> &stack) override;
        tempo_utils::Result<ElementPiece *> insert(int index, bool after);
        tempo_utils::Status remove(int index);
        tempo_utils::Status finish(const std::unique_ptr<JsonPiece> &piece, std::stack<JsonPiece *> &stack);
        tempo_utils::Status print(std::string &out) override;
    };

    struct MemberPiece : JsonPiece {
        StringPiece *key;
        JsonPiece *value;
        std::vector<JsonPiece *> before;
        std::vector<JsonPiece *> afterKey;
        std::vector<JsonPiece *> beforeValue;
        std::vector<JsonPiece *> after;
        enum class Expect { Key, Colon, Value, Done } expect;
        explicit MemberPiece(Token token);
        MemberPiece(Token token, std::string_view key);
        ~MemberPiece() override;
        tempo_utils::Status append(std::unique_ptr<JsonPiece> &&piece, std::stack<JsonPiece *> &stack) override;
        tempo_utils::Status print(std::string &out) override;
    };

    struct ObjectPiece : JsonPiece {
        std::vector<MemberPiece *> members;
        absl::flat_hash_map<std::string, size_t> keyIndex;
        MemberPiece *pending;
        explicit ObjectPiece(Token token);
        ~ObjectPiece() override;
        tempo_utils::Status append(std::unique_ptr<JsonPiece> &&piece, std::stack<JsonPiece *> &stack) override;
        tempo_utils::Result<MemberPiece *> insert(std::string_view key);
        tempo_utils::Status remove(std::string_view key);
        tempo_utils::Status finish(const std::unique_ptr<JsonPiece> &piece, std::stack<JsonPiece *> &stack);
        tempo_utils::Status print(std::string &out) override;
    };

    struct CommentPiece : JsonPiece {
        CommentPiece(Token token, std::string value);
        tempo_utils::Status print(std::string &out) override;
    };

    struct WhitespacePiece : JsonPiece {
        WhitespacePiece(Token token, std::string value);
        tempo_utils::Status print(std::string &out) override;
    };
}

#endif // TEMPO_CONFIG_INTERNAL_JSON_PIECE_H