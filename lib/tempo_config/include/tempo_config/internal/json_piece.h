#ifndef TEMPO_CONFIG_INTERNAL_JSON_PIECE_H
#define TEMPO_CONFIG_INTERNAL_JSON_PIECE_H

#include <tempo_config/config_result.h>

namespace tempo_config::internal {

    enum class PieceType {
        Literal,
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
        int type;
        int channel;
        int offset;
        int span;
    };

    struct JsonPiece {
        PieceType type;
        Token token;
        std::string value;
        JsonPiece *prev;
        JsonPiece *next;
        JsonPiece *last;
        JsonPiece *parent;
        JsonPiece(PieceType type, Token token, std::string_view value);
        virtual ~JsonPiece() = default;
        virtual tempo_utils::Status append(std::unique_ptr<JsonPiece> &&piece, JsonPiece **head, JsonPiece **tail) = 0;
    };

    JsonPiece *
    append_piece(
        std::unique_ptr<JsonPiece> &&piece,
        JsonPiece **head,
        JsonPiece **tail);

    struct OperatorPiece : JsonPiece {
        OperatorPiece(PieceType type, Token token, std::string_view value);
    };

    struct CommaPiece : OperatorPiece {
        CommaPiece(Token token);
        tempo_utils::Status append(std::unique_ptr<JsonPiece> &&piece, JsonPiece **head, JsonPiece **tail) override;
    };

    struct ColonPiece : OperatorPiece {
        ColonPiece(Token token);
        tempo_utils::Status append(std::unique_ptr<JsonPiece> &&piece, JsonPiece **head, JsonPiece **tail) override;
    };

    struct BracketClosePiece : OperatorPiece {
        BracketClosePiece(Token token);
        tempo_utils::Status append(std::unique_ptr<JsonPiece> &&piece, JsonPiece **head, JsonPiece **tail) override;
    };

    struct CurlyClosePiece : OperatorPiece {
        CurlyClosePiece(Token token);
        tempo_utils::Status append(std::unique_ptr<JsonPiece> &&piece, JsonPiece **head, JsonPiece **tail) override;
    };

    struct LiteralPiece : JsonPiece {
        LiteralPiece(Token token, std::string value);
        tempo_utils::Status append(std::unique_ptr<JsonPiece> &&piece, JsonPiece **head, JsonPiece **tail) override;
    };

    struct ElementPiece : JsonPiece {
        ElementPiece(Token token);
        JsonPiece *element;
        int index;
        enum class Expect { Value, Comma, Done } expect;
        tempo_utils::Status append(std::unique_ptr<JsonPiece> &&piece, JsonPiece **head, JsonPiece **tail) override;
        tempo_utils::Status finish(const std::unique_ptr<JsonPiece> &piece);
    };

    struct ArrayPiece : JsonPiece {
        ArrayPiece(Token token);
        std::vector<ElementPiece *> elements;
        std::vector<CommaPiece *> separators;
        ElementPiece *pending;
        tempo_utils::Status append(std::unique_ptr<JsonPiece> &&piece, JsonPiece **head, JsonPiece **tail) override;
    };

    struct MemberPiece : JsonPiece {
        MemberPiece(Token token);
        LiteralPiece *key;
        JsonPiece *value;
        int index;
        enum class Expect { Key, Colon, Value, Comma, Done } expect;
        tempo_utils::Status append(std::unique_ptr<JsonPiece> &&piece, JsonPiece **head, JsonPiece **tail) override;
        tempo_utils::Status finish(const std::unique_ptr<JsonPiece> &piece);
    };

    struct ObjectPiece : JsonPiece {
        ObjectPiece(Token token);
        absl::flat_hash_map<std::string, MemberPiece *> members;
        std::vector<CommaPiece *> separators;
        MemberPiece *pending;
        tempo_utils::Status append(std::unique_ptr<JsonPiece> &&piece, JsonPiece **head, JsonPiece **tail) override;
    };

    struct CommentPiece : JsonPiece {
        CommentPiece(Token token, std::string value);
        tempo_utils::Status append(std::unique_ptr<JsonPiece> &&piece, JsonPiece **head, JsonPiece **tail) override;
    };

    struct WhitespacePiece : JsonPiece {
        WhitespacePiece(Token token, std::string value);
        tempo_utils::Status append(std::unique_ptr<JsonPiece> &&piece, JsonPiece **head, JsonPiece **tail) override;
    };
}

#endif // TEMPO_CONFIG_INTERNAL_JSON_PIECE_H