#ifndef TEMPO_CONFIG_INTERNAL_PIECE_PRINTER_H
#define TEMPO_CONFIG_INTERNAL_PIECE_PRINTER_H

#include "json_piece.h"

namespace tempo_config::internal {

    enum class Reformat {
        None,
        OnlyChanged,
        Full,
    };

    struct PrinterOptions {
        int indent = 2;
        Reformat reformat = Reformat::OnlyChanged;
    };

    struct PrinterState {
        explicit PrinterState(const PrinterOptions *options);
        const PrinterOptions *options;
        int line;
        int positionInLine;
        int totalChars;
        int indentLevel;
        std::string leftMargin;
        std::string output;
    };

    class AbstractPiecePrinter {
    public:
        virtual ~AbstractPiecePrinter() = default;
        virtual tempo_utils::Status print(PrinterState &state) = 0;
    };

    class RootPiecePrinter : public AbstractPiecePrinter {
    public:
        explicit RootPiecePrinter(const RootPiece *piece);
        tempo_utils::Status print(PrinterState &state) override;
    private:
        const RootPiece *m_piece;
    };

    class KeywordPiecePrinter : public AbstractPiecePrinter {
    public:
        explicit KeywordPiecePrinter(const KeywordPiece *piece);
        tempo_utils::Status print(PrinterState &state) override;
    private:
        const KeywordPiece *m_piece;
    };

    class NumberPiecePrinter : public AbstractPiecePrinter {
    public:
        explicit NumberPiecePrinter(const NumberPiece *piece);
        tempo_utils::Status print(PrinterState &state) override;
    private:
        const NumberPiece *m_piece;
    };

    class StringPiecePrinter : public AbstractPiecePrinter {
    public:
        explicit StringPiecePrinter(const StringPiece *piece);
        tempo_utils::Status print(PrinterState &state) override;
    private:
        const StringPiece *m_piece;
    };

    class ElementPiecePrinter : public AbstractPiecePrinter {
    public:
        explicit ElementPiecePrinter(const ElementPiece *piece);
        tempo_utils::Status print(PrinterState &state) override;
    private:
        const ElementPiece *m_piece;
    };

    class MemberPiecePrinter : public AbstractPiecePrinter {
    public:
        explicit MemberPiecePrinter(const MemberPiece *piece);
        tempo_utils::Status print(PrinterState &state) override;
    private:
        const MemberPiece *m_piece;
    };

    class ArrayPiecePrinter : public AbstractPiecePrinter {
    public:
        explicit ArrayPiecePrinter(const ArrayPiece *piece);
        tempo_utils::Status print(PrinterState &state) override;
    private:
        const ArrayPiece *m_piece;
    };

    class ObjectPiecePrinter : public AbstractPiecePrinter {
    public:
        explicit ObjectPiecePrinter(const ObjectPiece *piece);
        tempo_utils::Status print(PrinterState &state) override;
    private:
        const ObjectPiece *m_piece;
    };

    class CommentPiecePrinter : public AbstractPiecePrinter {
    public:
        explicit CommentPiecePrinter(const CommentPiece *piece);
        tempo_utils::Status print(PrinterState &state) override;
    private:
        const CommentPiece *m_piece;
    };

    class WhitespacePiecePrinter : public AbstractPiecePrinter {
    public:
        explicit WhitespacePiecePrinter(const WhitespacePiece *piece);
        tempo_utils::Status print(PrinterState &state) override;
    private:
        const WhitespacePiece *m_piece;
    };

    tempo_utils::Status print_root(const PrinterOptions &options, RootPiece *root, std::string &output);

}

#endif // TEMPO_CONFIG_INTERNAL_PIECE_PRINTER_H