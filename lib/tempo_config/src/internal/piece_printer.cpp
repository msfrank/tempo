
#include <tempo_config/internal/piece_printer.h>

tempo_config::internal::PrinterState::PrinterState(const PrinterOptions *options)
    : options(options),
      line(0),
      positionInLine(0),
      totalChars(0),
      indentLevel(0)
{
}

std::unique_ptr<tempo_config::internal::AbstractPiecePrinter>
make_printer(tempo_config::internal::JsonPiece *piece)
{
    TU_ASSERT (piece != nullptr);

    switch (piece->type) {
        case tempo_config::internal::PieceType::Root:
            {
                auto *rootPiece = static_cast<tempo_config::internal::RootPiece *>(piece);
                return std::make_unique<tempo_config::internal::RootPiecePrinter>(rootPiece);
            }
        case tempo_config::internal::PieceType::Keyword:
            {
                auto *keywordPiece = static_cast<tempo_config::internal::KeywordPiece *>(piece);
                return std::make_unique<tempo_config::internal::KeywordPiecePrinter>(keywordPiece);
            }
        case tempo_config::internal::PieceType::Number:
            {
                auto *numberPiece = static_cast<tempo_config::internal::NumberPiece *>(piece);
                return std::make_unique<tempo_config::internal::NumberPiecePrinter>(numberPiece);
            }
        case tempo_config::internal::PieceType::String:
            {
                auto *stringPiece = static_cast<tempo_config::internal::StringPiece *>(piece);
                return std::make_unique<tempo_config::internal::StringPiecePrinter>(stringPiece);
            }
        case tempo_config::internal::PieceType::Element:
            {
                auto *elementPiece = static_cast<tempo_config::internal::ElementPiece *>(piece);
                return std::make_unique<tempo_config::internal::ElementPiecePrinter>(elementPiece);
            }
        case tempo_config::internal::PieceType::Member:
            {
                auto *memberPiece = static_cast<tempo_config::internal::MemberPiece *>(piece);
                return std::make_unique<tempo_config::internal::MemberPiecePrinter>(memberPiece);
            }
        case tempo_config::internal::PieceType::Array:
            {
                auto *arrayPiece = static_cast<tempo_config::internal::ArrayPiece *>(piece);
                return std::make_unique<tempo_config::internal::ArrayPiecePrinter>(arrayPiece);
            }
        case tempo_config::internal::PieceType::Object:
            {
                auto *objectPiece = static_cast<tempo_config::internal::ObjectPiece *>(piece);
                return std::make_unique<tempo_config::internal::ObjectPiecePrinter>(objectPiece);
            }
        case tempo_config::internal::PieceType::Comment:
            {
                auto *commentPiece = static_cast<tempo_config::internal::CommentPiece *>(piece);
                return std::make_unique<tempo_config::internal::CommentPiecePrinter>(commentPiece);
            }
        case tempo_config::internal::PieceType::Whitespace:
            {
                auto *whitespacePiece = static_cast<tempo_config::internal::WhitespacePiece *>(piece);
                return std::make_unique<tempo_config::internal::WhitespacePiecePrinter>(whitespacePiece);
            }

        // unhandled
        case tempo_config::internal::PieceType::Comma:
        case tempo_config::internal::PieceType::Colon:
        case tempo_config::internal::PieceType::BracketClose:
        case tempo_config::internal::PieceType::CurlyClose:
            TU_UNREACHABLE();
    }
}

tempo_config::internal::RootPiecePrinter::RootPiecePrinter(const RootPiece *piece)
    : m_piece(piece)
{
    TU_ASSERT (m_piece != nullptr);
}

tempo_utils::Status
tempo_config::internal::RootPiecePrinter::print(PrinterState &state)
{
    for (auto *piece : m_piece->before) {
        auto printer = make_printer(piece);
        TU_RETURN_IF_NOT_OK (printer->print(state));
    }
    if (m_piece->root != nullptr) {
        auto printer = make_printer(m_piece->root);
        TU_RETURN_IF_NOT_OK (printer->print(state));
    }
    for (auto *piece : m_piece->after) {
        auto printer = make_printer(piece);
        TU_RETURN_IF_NOT_OK (printer->print(state));
    }
    return {};
}

tempo_config::internal::KeywordPiecePrinter::KeywordPiecePrinter(const KeywordPiece *piece)
    : m_piece(piece)
{
    TU_ASSERT (m_piece != nullptr);
}

tempo_utils::Status
tempo_config::internal::KeywordPiecePrinter::print(PrinterState &state)
{
    absl::StrAppend(&state.output, m_piece->value);
    auto size = m_piece->value.size();
    state.positionInLine += size;
    state.totalChars += size;
    return {};
}

tempo_config::internal::NumberPiecePrinter::NumberPiecePrinter(const NumberPiece *piece)
    : m_piece(piece)
{
    TU_ASSERT (m_piece != nullptr);
}

tempo_utils::Status
tempo_config::internal::NumberPiecePrinter::print(PrinterState &state)
{
    absl::StrAppend(&state.output, m_piece->value);
    auto size = m_piece->value.size();
    state.positionInLine += size;
    state.totalChars += size;
    return {};
}

tempo_config::internal::StringPiecePrinter::StringPiecePrinter(const StringPiece *piece)
    : m_piece(piece)
{
    TU_ASSERT (m_piece != nullptr);
}

tempo_utils::Status
tempo_config::internal::StringPiecePrinter::print(PrinterState &state)
{
    absl::StrAppend(&state.output, m_piece->value);
    auto size = m_piece->value.size();
    state.positionInLine += size;
    state.totalChars += size;
    return {};
}

tempo_config::internal::ElementPiecePrinter::ElementPiecePrinter(const ElementPiece *piece)
    : m_piece(piece)
{
    TU_ASSERT (m_piece != nullptr);
}

tempo_utils::Status
tempo_config::internal::ElementPiecePrinter::print(PrinterState &state)
{
    for (auto *piece : m_piece->before) {
        auto printer = make_printer(piece);
        TU_RETURN_IF_NOT_OK (printer->print(state));
    }
    if (m_piece->element != nullptr) {
        auto printer = make_printer(m_piece->element);
        TU_RETURN_IF_NOT_OK (printer->print(state));
    }
    for (auto *piece : m_piece->after) {
        auto printer = make_printer(piece);
        TU_RETURN_IF_NOT_OK (printer->print(state));
    }
    return {};
}

tempo_config::internal::MemberPiecePrinter::MemberPiecePrinter(const MemberPiece *piece)
    : m_piece(piece)
{
    TU_ASSERT (m_piece != nullptr);
}

tempo_utils::Status
tempo_config::internal::MemberPiecePrinter::print(PrinterState &state)
{
    for (auto *piece : m_piece->before) {
        auto printer = make_printer(piece);
        TU_RETURN_IF_NOT_OK (printer->print(state));
    }

    if (m_piece->key != nullptr) {
        auto printer = make_printer(m_piece->key);
        TU_RETURN_IF_NOT_OK (printer->print(state));
        for (auto *piece : m_piece->afterKey) {
            printer = make_printer(piece);
            TU_RETURN_IF_NOT_OK (printer->print(state));
        }

        state.output.push_back(':');
        state.positionInLine++;
        state.totalChars++;

        if (m_piece->value != nullptr) {
            for (auto *piece : m_piece->beforeValue) {
                printer = make_printer(piece);
                TU_RETURN_IF_NOT_OK (printer->print(state));
            }
            printer = make_printer(m_piece->value);
            TU_RETURN_IF_NOT_OK (printer->print(state));
        }
    }

    for (auto *piece : m_piece->after) {
        auto printer = make_printer(piece);
        TU_RETURN_IF_NOT_OK (printer->print(state));
    }
    return {};
}

tempo_config::internal::ArrayPiecePrinter::ArrayPiecePrinter(const ArrayPiece *piece)
    : m_piece(piece)
{
    TU_ASSERT (m_piece != nullptr);
}

tempo_utils::Status
tempo_config::internal::ArrayPiecePrinter::print(PrinterState &state)
{
    state.output.push_back('[');
    state.positionInLine++;
    state.totalChars++;
    state.indentLevel++;

    auto it = m_piece->elements.cbegin();
    if (it != m_piece->elements.cend()) {
        auto printer = make_printer(*it++);
        TU_RETURN_IF_NOT_OK (printer->print(state));
    }
    for (; it != m_piece->elements.cend(); ++it) {
        state.output.push_back(',');
        state.positionInLine++;
        state.totalChars++;
        auto printer = make_printer(*it++);
        TU_RETURN_IF_NOT_OK (printer->print(state));
    }

    state.output.push_back(']');
    state.positionInLine++;
    state.totalChars++;
    state.indentLevel--;

    return {};
}

tempo_config::internal::ObjectPiecePrinter::ObjectPiecePrinter(const ObjectPiece *piece)
    : m_piece(piece)
{
    TU_ASSERT (m_piece != nullptr);
}

tempo_utils::Status
tempo_config::internal::ObjectPiecePrinter::print(PrinterState &state)
{
    state.output.push_back('{');
    state.positionInLine++;
    state.totalChars++;
    state.indentLevel++;

    auto it = m_piece->members.cbegin();
    if (it != m_piece->members.cend()) {
        auto printer = make_printer(*it++);
        TU_RETURN_IF_NOT_OK (printer->print(state));
    }
    for (; it != m_piece->members.cend(); ++it) {
        state.output.push_back(',');
        state.positionInLine++;
        state.totalChars++;
        auto printer = make_printer(*it++);
        TU_RETURN_IF_NOT_OK (printer->print(state));
    }

    state.output.push_back('}');
    state.positionInLine++;
    state.totalChars++;
    state.indentLevel--;

    return {};
}

tempo_config::internal::CommentPiecePrinter::CommentPiecePrinter(const CommentPiece *piece)
    : m_piece(piece)
{
    TU_ASSERT (m_piece != nullptr);
}

tempo_utils::Status
tempo_config::internal::CommentPiecePrinter::print(PrinterState &state)
{
    auto size = m_piece->value.size();
    for (const auto ch : m_piece->value) {
        state.output.push_back(ch);
        if (ch == '\n') {
            state.line++;
            state.positionInLine = 0;
        } else {
            state.positionInLine++;
        }
    }
    state.totalChars += size;
    return {};
}

tempo_config::internal::WhitespacePiecePrinter::WhitespacePiecePrinter(const WhitespacePiece *piece)
    : m_piece(piece)
{
    TU_ASSERT (m_piece != nullptr);
}

tempo_utils::Status
tempo_config::internal::WhitespacePiecePrinter::print(PrinterState &state)
{
    auto size = m_piece->value.size();
    int startOfLine = -1;

    if (state.totalChars == 0) {
        startOfLine = 0;
    }

    for (const auto ch : m_piece->value) {
        state.output.push_back(ch);

        if (ch == '\n') {
            if (startOfLine > 0) {
                state.leftMargin = state.output.substr(startOfLine, state.positionInLine);
            }
            state.line++;
            state.positionInLine = 0;
            startOfLine = state.output.size();
        } else {
            state.positionInLine++;
        }
    }

    if (startOfLine >= 0) {
        state.leftMargin = state.output.substr(startOfLine, state.positionInLine);
    }

    state.totalChars += size;

    return {};
}

tempo_utils::Status
tempo_config::internal::print_root(const PrinterOptions &options, RootPiece *root, std::string &output)
{
    PrinterState state(&options);
    RootPiecePrinter rootPrinter(root);
    TU_RETURN_IF_NOT_OK (rootPrinter.print(state));
    output = std::move(state.output);
    return {};
}