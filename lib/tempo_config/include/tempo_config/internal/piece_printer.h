#ifndef TEMPO_CONFIG_INTERNAL_PIECE_PRINTER_H
#define TEMPO_CONFIG_INTERNAL_PIECE_PRINTER_H

#include "../config_types.h"

#include "json_piece.h"

namespace tempo_config::internal {

    struct PrinterState {
        PrinterState();
        explicit PrinterState(const PrinterOptions *options);
        PrinterState(const PrinterState &other);

        void append(char ch, std::string &output);
        void append(std::string_view s, std::string &output);

        const PrinterOptions *options;
        int line;
        int positionInLine;
        int totalChars;
        int indentLevel;
        std::string leftMargin;
    };

    class AbstractPiecePrinter {
    public:
        virtual ~AbstractPiecePrinter() = default;
        virtual tempo_utils::Status print(std::string &output, PrinterState &state) = 0;
    };

    class RootPiecePrinter : public AbstractPiecePrinter {
    public:
        explicit RootPiecePrinter(const RootPiece *piece, const PrinterState &initial);
        tempo_utils::Status print(std::string &output, PrinterState &state) override;
    private:
        const RootPiece *m_piece;
        PrinterState m_initial;
    };

    class KeywordPiecePrinter : public AbstractPiecePrinter {
    public:
        explicit KeywordPiecePrinter(const KeywordPiece *piece, const PrinterState &initial);
        tempo_utils::Status print(std::string &output, PrinterState &state) override;
    private:
        const KeywordPiece *m_piece;
        PrinterState m_initial;
    };

    class NumberPiecePrinter : public AbstractPiecePrinter {
    public:
        explicit NumberPiecePrinter(const NumberPiece *piece, const PrinterState &initial);
        tempo_utils::Status print(std::string &output, PrinterState &state) override;
    private:
        const NumberPiece *m_piece;
        PrinterState m_initial;
    };

    class StringPiecePrinter : public AbstractPiecePrinter {
    public:
        explicit StringPiecePrinter(const StringPiece *piece, const PrinterState &initial);
        tempo_utils::Status print(std::string &output, PrinterState &state) override;
    private:
        const StringPiece *m_piece;
        PrinterState m_initial;
    };

    class ElementPiecePrinter : public AbstractPiecePrinter {
    public:
        ElementPiecePrinter(
            const ElementPiece *piece,
            const PrinterState &parent,
            bool only);
        ElementPiecePrinter(
            const ElementPiece *piece,
            const PrinterState &parent,
            const PrinterState &prev,
            bool last);
        tempo_utils::Status print(std::string &output, PrinterState &state) override;
    private:
        const ElementPiece *m_piece;
        enum { Only, First, Next, Last } m_pos;
        PrinterState m_parent;
        PrinterState m_prev;
    };

    class MemberPiecePrinter : public AbstractPiecePrinter {
    public:
        MemberPiecePrinter(
            const MemberPiece *piece,
            const PrinterState &parent,
            bool only);
        MemberPiecePrinter(
            const MemberPiece *piece,
            const PrinterState &parent,
            const PrinterState &prev,
            bool last);
        tempo_utils::Status print(std::string &output, PrinterState &state) override;
    private:
        const MemberPiece *m_piece;
        enum { Only, First, Next, Last } m_pos;
        PrinterState m_parent;
        PrinterState m_prev;
    };

    class ArrayPiecePrinter : public AbstractPiecePrinter {
    public:
        explicit ArrayPiecePrinter(const ArrayPiece *piece, const PrinterState &initial);
        tempo_utils::Status print(std::string &output, PrinterState &state) override;
    private:
        const ArrayPiece *m_piece;
        PrinterState m_initial;
    };

    class ObjectPiecePrinter : public AbstractPiecePrinter {
    public:
        explicit ObjectPiecePrinter(const ObjectPiece *piece, const PrinterState &initial);
        tempo_utils::Status print(std::string &output, PrinterState &state) override;
    private:
        const ObjectPiece *m_piece;
        PrinterState m_initial;
    };

    class CommentPiecePrinter : public AbstractPiecePrinter {
    public:
        explicit CommentPiecePrinter(const CommentPiece *piece, const PrinterState &initial);
        tempo_utils::Status print(std::string &output, PrinterState &state) override;
    private:
        const CommentPiece *m_piece;
        PrinterState m_initial;
    };

    class WhitespacePiecePrinter : public AbstractPiecePrinter {
    public:
        explicit WhitespacePiecePrinter(const WhitespacePiece *piece, const PrinterState &initial);
        tempo_utils::Status print(std::string &output, PrinterState &state) override;
    private:
        const WhitespacePiece *m_piece;
        PrinterState m_initial;
    };

    tempo_utils::Status print_root(const PrinterOptions &options, RootPiece *root, std::string &output);

}

#endif // TEMPO_CONFIG_INTERNAL_PIECE_PRINTER_H