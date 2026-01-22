#ifndef TEMPO_CONFIG_INTERNAL_PIECE_STORE_H
#define TEMPO_CONFIG_INTERNAL_PIECE_STORE_H

#include <string>
#include <stack>

#include <ANTLRInputStream.h>

#include <tempo_utils/result.h>
#include <tempo_utils/status.h>

#include "../config_path.h"
#include "../config_types.h"
#include "json_piece.h"

namespace tempo_config::internal {

    class PieceStore {
    public:
        PieceStore();
        ~PieceStore();

        RootPiece *root() const;

        tempo_utils::Status parse(std::string_view utf8);
        tempo_utils::Status insert(const ConfigNode &node, JsonPiece *root);

        JsonPiece *findParent(const ConfigPath &path, ConfigPathPart &last);
        JsonPiece *find(const ConfigPath &path);

        void reset();

        tempo_utils::Status writeJson(std::string &out, const PrinterOptions &options = {}) const;

    private:
        std::unique_ptr<RootPiece> m_root;

        tempo_utils::Status parse(
            antlr4::BufferedTokenStream &tokens,
            std::stack<JsonPiece *> &stack);

    };
}

#endif // TEMPO_CONFIG_INTERNAL_PIECE_STORE_H