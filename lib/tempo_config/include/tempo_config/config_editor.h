#ifndef TEMPO_CONFIG_CONFIG_EDITOR_H
#define TEMPO_CONFIG_CONFIG_EDITOR_H

#include <string>
#include <stack>

#include <tempo_utils/result.h>
#include <tempo_utils/status.h>

#include "config_path.h"
#include "config_types.h"

namespace tempo_config {

    namespace internal {
        struct JsonPiece;
        struct LiteralPiece;
        struct ArrayPiece;
        struct ObjectPiece;
        struct OperatorPiece;
        struct CommentPiece;
        struct WhitespacePiece;
    }

    class ConfigEditor {
    public:
        ConfigEditor();
        ~ConfigEditor();

        tempo_utils::Status parse(std::string_view utf8);

        tempo_utils::Status replaceNode(const ConfigPath &path, const ConfigNode &node);
        tempo_utils::Status removeNode(const ConfigPath &path);

        void reset();

        std::string toString() const;

    private:
        std::string m_utf8;

        internal::JsonPiece *m_root;
        internal::JsonPiece *m_head;
        internal::JsonPiece *m_tail;
        std::stack<internal::JsonPiece *> m_stack;

        tempo_utils::Status append(std::unique_ptr<internal::OperatorPiece> &&piece);
        tempo_utils::Status append(std::unique_ptr<internal::CommentPiece> &&piece);
        tempo_utils::Status append(std::unique_ptr<internal::WhitespacePiece> &&piece);
        tempo_utils::Status push(std::unique_ptr<internal::LiteralPiece> &&piece);
        tempo_utils::Status push(std::unique_ptr<internal::ArrayPiece> &&piece);
        tempo_utils::Status push(std::unique_ptr<internal::ObjectPiece> &&piece);
        tempo_utils::Status pop(std::unique_ptr<internal::OperatorPiece> &&piece);
    };
}

#endif // TEMPO_CONFIG_CONFIG_EDITOR_H