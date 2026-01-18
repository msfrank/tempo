#ifndef TEMPO_CONFIG_INTERNAL_JSON_PIECE_H
#define TEMPO_CONFIG_INTERNAL_JSON_PIECE_H

#include <tempo_config/config_result.h>

namespace tempo_config::internal {

    enum class PieceType {
        Literal,
        Array,
        Object,
        Operator,
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
        JsonPiece(PieceType type, Token token, std::string_view value);
        virtual ~JsonPiece() = default;
        virtual tempo_utils::Status append(JsonPiece *piece) = 0;
    };

    struct LiteralPiece : JsonPiece {
        LiteralPiece(Token token, std::string value);
        tempo_utils::Status append(JsonPiece *piece) override;
    };

    struct ArrayPiece : JsonPiece {
        ArrayPiece(Token token);
        std::vector<JsonPiece *> elements;

        enum class Expect
        {
            Element,
            Comma,
        } expect;

        tempo_utils::Status append(JsonPiece *piece) override;
    };

    struct ObjectPiece : JsonPiece {
        ObjectPiece(Token token);
        absl::flat_hash_map<std::string, JsonPiece *> members;
        std::string key;

        enum class Expect
        {
            Key,
            Colon,
            Value,
            Comma,
        } expect;

        tempo_utils::Status append(JsonPiece *piece) override;
    };

    struct OperatorPiece : JsonPiece {
        OperatorPiece(Token token);
        tempo_utils::Status append(JsonPiece *piece) override;
    };

    struct CommentPiece : JsonPiece {
        CommentPiece(Token token, std::string value);
        tempo_utils::Status append(JsonPiece *piece) override;
    };

    struct WhitespacePiece : JsonPiece {
        WhitespacePiece(Token token, std::string value);
        tempo_utils::Status append(JsonPiece *piece) override;
    };
}

#endif // TEMPO_CONFIG_INTERNAL_JSON_PIECE_H