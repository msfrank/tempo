#ifndef TEMPO_CONFIG_INTERNAL_PIECE_STORE_H
#define TEMPO_CONFIG_INTERNAL_PIECE_STORE_H

#include <string>
#include <stack>

#include <ANTLRInputStream.h>

#include <tempo_utils/result.h>
#include <tempo_utils/status.h>

#include "../config_types.h"

namespace tempo_config::internal {

    class PieceStore {
    public:
        PieceStore();
        ~PieceStore();

        RootPiece *root() const;

        tempo_utils::Status parse(std::string_view utf8);
        tempo_utils::Status insert(const ConfigNode &node, JsonPiece *root);

        void reset();

        tempo_utils::Status writeJson(std::string &out) const;

    private:
        std::unique_ptr<RootPiece> m_root;

        tempo_utils::Status parse(
            antlr4::BufferedTokenStream &tokens,
            std::stack<JsonPiece *> &stack);

    };

    // tempo_utils::Status append_piece(
    //     std::unique_ptr<OperatorPiece> &&piece,
    //     JsonPiece **pred,
    //     JsonPiece **succ,
    //     std::stack<JsonPiece *> &stack);
    // tempo_utils::Status append_piece(
    //     std::unique_ptr<CommentPiece> &&piece,
    //     JsonPiece **pred,
    //     JsonPiece **succ,
    //     std::stack<JsonPiece *> &stack);
    // tempo_utils::Status append_piece(
    //     std::unique_ptr<WhitespacePiece> &&piece,
    //     JsonPiece **pred,
    //     JsonPiece **succ,
    //     std::stack<JsonPiece *> &stack);
    // tempo_utils::Status push_piece(
    //     std::unique_ptr<LiteralPiece> &&piece,
    //     JsonPiece **pred,
    //     JsonPiece **succ,
    //     std::stack<JsonPiece *> &stack);
    // tempo_utils::Status push_piece(
    //     std::unique_ptr<ArrayPiece> &&piece,
    //     JsonPiece **pred,
    //     JsonPiece **succ,
    //     std::stack<JsonPiece *> &stack);
    // tempo_utils::Status push_piece(
    //     std::unique_ptr<ObjectPiece> &&piece,
    //     JsonPiece **pred,
    //     JsonPiece **succ,
    //     std::stack<JsonPiece *> &stack);
    // tempo_utils::Status pop_piece(
    //     std::unique_ptr<OperatorPiece> &&piece,
    //     JsonPiece **pred,
    //     JsonPiece **succ,
    //     std::stack<JsonPiece *> &stack);
}

#endif // TEMPO_CONFIG_INTERNAL_PIECE_STORE_H