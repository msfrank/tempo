
#include <tempo_config/internal/piece_printer.h>

tempo_config::internal::PrinterState::PrinterState()
    : options(nullptr),
      line(0),
      positionInLine(0),
      totalChars(0),
      indentLevel(0)
{
}

tempo_config::internal::PrinterState::PrinterState(const PrinterOptions *options)
    : PrinterState()
{
      this->options = options;
}

tempo_config::internal::PrinterState::PrinterState(const PrinterState &other)
    : options(other.options),
      line(other.line),
      positionInLine(other.positionInLine),
      totalChars(other.totalChars),
      indentLevel(other.indentLevel),
      leftMargin(other.leftMargin)
{
}

void
tempo_config::internal::PrinterState::append(char ch, std::string &output)
{
    output.push_back(ch);
    totalChars++;
    if (ch == '\n') {
        line++;
        positionInLine = 0;
    } else {
        positionInLine++;
    }
}

void
tempo_config::internal::PrinterState::append(std::string_view s, std::string &output)
{
    if (s.empty())
        return;

    size_t curr = 0;
    size_t next = 0;
    while (next != std::string_view::npos) {
        next = s.find('\n', curr);
        auto chunk = s.substr(curr, next);
        output.append(chunk.begin(), chunk.end());
        totalChars += chunk.size();

        if (next != std::string_view::npos) {
            output.push_back('\n');
            totalChars++;
            line++;
            positionInLine = 0;
        } else {
            positionInLine += chunk.size();
        }

        curr = next;
    }
}

std::unique_ptr<tempo_config::internal::AbstractPiecePrinter>
make_printer(tempo_config::internal::JsonPiece *piece, const tempo_config::internal::PrinterState &initial)
{
    TU_ASSERT (piece != nullptr);

    switch (piece->type) {
        case tempo_config::internal::PieceType::Root:
            {
                auto *rootPiece = static_cast<tempo_config::internal::RootPiece *>(piece);
                return std::make_unique<tempo_config::internal::RootPiecePrinter>(rootPiece, initial);
            }
        case tempo_config::internal::PieceType::Keyword:
            {
                auto *keywordPiece = static_cast<tempo_config::internal::KeywordPiece *>(piece);
                return std::make_unique<tempo_config::internal::KeywordPiecePrinter>(keywordPiece, initial);
            }
        case tempo_config::internal::PieceType::Number:
            {
                auto *numberPiece = static_cast<tempo_config::internal::NumberPiece *>(piece);
                return std::make_unique<tempo_config::internal::NumberPiecePrinter>(numberPiece, initial);
            }
        case tempo_config::internal::PieceType::String:
            {
                auto *stringPiece = static_cast<tempo_config::internal::StringPiece *>(piece);
                return std::make_unique<tempo_config::internal::StringPiecePrinter>(stringPiece, initial);
            }
        case tempo_config::internal::PieceType::Array:
            {
                auto *arrayPiece = static_cast<tempo_config::internal::ArrayPiece *>(piece);
                return std::make_unique<tempo_config::internal::ArrayPiecePrinter>(arrayPiece, initial);
            }
        case tempo_config::internal::PieceType::Object:
            {
                auto *objectPiece = static_cast<tempo_config::internal::ObjectPiece *>(piece);
                return std::make_unique<tempo_config::internal::ObjectPiecePrinter>(objectPiece, initial);
            }
        case tempo_config::internal::PieceType::Comment:
            {
                auto *commentPiece = static_cast<tempo_config::internal::CommentPiece *>(piece);
                return std::make_unique<tempo_config::internal::CommentPiecePrinter>(commentPiece, initial);
            }
        case tempo_config::internal::PieceType::Whitespace:
            {
                auto *whitespacePiece = static_cast<tempo_config::internal::WhitespacePiece *>(piece);
                return std::make_unique<tempo_config::internal::WhitespacePiecePrinter>(whitespacePiece, initial);
            }

        default:
            TU_UNREACHABLE();
    }
}

tempo_config::internal::RootPiecePrinter::RootPiecePrinter(
    const RootPiece *piece,
    const PrinterState &initial)
    : m_piece(piece),
      m_initial(initial)
{
    TU_ASSERT (m_piece != nullptr);
}

tempo_utils::Status
tempo_config::internal::RootPiecePrinter::print(std::string &output, PrinterState &state)
{
    for (auto *piece : m_piece->before) {
        auto printer = make_printer(piece, m_initial);
        TU_RETURN_IF_NOT_OK (printer->print(output, state));
    }
    if (m_piece->root != nullptr) {
        auto printer = make_printer(m_piece->root, m_initial);
        TU_RETURN_IF_NOT_OK (printer->print(output, state));
    }
    for (auto *piece : m_piece->after) {
        auto printer = make_printer(piece, m_initial);
        TU_RETURN_IF_NOT_OK (printer->print(output, state));
    }
    return {};
}

tempo_config::internal::KeywordPiecePrinter::KeywordPiecePrinter(
    const KeywordPiece *piece,
    const PrinterState &initial)
    : m_piece(piece),
      m_initial(initial)
{
    TU_ASSERT (m_piece != nullptr);
}

tempo_utils::Status
tempo_config::internal::KeywordPiecePrinter::print(std::string &output, PrinterState &state)
{
    absl::StrAppend(&output, m_piece->value);
    auto size = m_piece->value.size();
    state.positionInLine += size;
    state.totalChars += size;
    return {};
}

tempo_config::internal::NumberPiecePrinter::NumberPiecePrinter(
    const NumberPiece *piece,
    const PrinterState &initial)
    : m_piece(piece),
      m_initial(initial)
{
    TU_ASSERT (m_piece != nullptr);
}

tempo_utils::Status
tempo_config::internal::NumberPiecePrinter::print(std::string &output, PrinterState &state)
{
    absl::StrAppend(&output, m_piece->value);
    auto size = m_piece->value.size();
    state.positionInLine += size;
    state.totalChars += size;
    return {};
}

tempo_config::internal::StringPiecePrinter::StringPiecePrinter(
    const StringPiece *piece,
    const PrinterState &initial)
    : m_piece(piece),
      m_initial(initial)
{
    TU_ASSERT (m_piece != nullptr);
}

tempo_utils::Status
tempo_config::internal::StringPiecePrinter::print(std::string &output, PrinterState &state)
{
    absl::StrAppend(&output, m_piece->value);
    auto size = m_piece->value.size();
    state.positionInLine += size;
    state.totalChars += size;
    return {};
}

tempo_config::internal::ElementPiecePrinter::ElementPiecePrinter(
    const ElementPiece *piece,
    const PrinterState &parent,
    bool only)
    : m_piece(piece),
      m_pos(only? Only : First),
      m_parent(parent)
{
    TU_ASSERT (m_piece != nullptr);
}

tempo_config::internal::ElementPiecePrinter::ElementPiecePrinter(
    const ElementPiece *piece,
    const PrinterState &parent,
    const PrinterState &prev,
    bool last)
    : m_piece(piece),
      m_pos(last? Last : Next),
      m_parent(parent),
      m_prev(prev)
{
    TU_ASSERT (m_piece != nullptr);
}

tempo_utils::Status
tempo_config::internal::ElementPiecePrinter::print(std::string &output, PrinterState &state)
{
    auto *options = state.options;

    bool reformat;
    switch (options->reformat)
    {
        case Reformat::Full:
            reformat = true;
            break;
        case Reformat::OnlyChanged:
            reformat = m_piece->token.synthesized;
            break;
        case Reformat::None:
            reformat = false;
            break;
    }

    if (reformat) {
        state.append('\n', output);

        std::string leftMargin;
        if (m_pos == First || m_pos == Only) {
            leftMargin = absl::StrCat(m_parent.leftMargin, std::string(options->indent, ' '));
        } else {
            leftMargin = m_prev.leftMargin;
        }

        PrinterState reformatState = state;
        reformatState.leftMargin = leftMargin;

        for (auto *piece : m_piece->before) {
            if (piece->type == PieceType::Whitespace)
                continue;
            if (piece->type == PieceType::Comment) {
                state.append(leftMargin, output);
                CommentPiecePrinter printer(static_cast<CommentPiece *>(piece), reformatState);
                TU_RETURN_IF_NOT_OK (printer.print(output, state));
                state.append('\n', output);
                reformatState = state;
            } else {
                return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                    "unexpected token");
            }
        }
        if (m_piece->element != nullptr) {
            state.append(leftMargin, output);
            auto printer = make_printer(m_piece->element, reformatState);
            TU_RETURN_IF_NOT_OK (printer->print(output, state));
            if (m_pos == First || m_pos == Next) {
                state.append('\n', output);
            }
            reformatState = state;
        }
        for (auto *piece : m_piece->after) {
            if (piece->type == PieceType::Whitespace)
                continue;
            if (piece->type == PieceType::Comment) {
                state.append(leftMargin, output);
                CommentPiecePrinter printer(static_cast<CommentPiece *>(piece), reformatState);
                TU_RETURN_IF_NOT_OK (printer.print(output, state));
                state.append('\n', output);
                reformatState = state;
            } else {
                return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                    "unexpected token");
            }
        }
        return {};
    }

    for (auto *piece : m_piece->before) {
        auto printer = make_printer(piece, m_parent);
        TU_RETURN_IF_NOT_OK (printer->print(output, state));
    }
    if (m_piece->element != nullptr) {
        auto printer = make_printer(m_piece->element, m_parent);
        TU_RETURN_IF_NOT_OK (printer->print(output, state));
    }
    for (auto *piece : m_piece->after) {
        auto printer = make_printer(piece, m_parent);
        TU_RETURN_IF_NOT_OK (printer->print(output, state));
    }

    return {};
}

tempo_config::internal::MemberPiecePrinter::MemberPiecePrinter(
    const MemberPiece *piece,
    const PrinterState &parent,
    bool only)
    : m_piece(piece),
      m_pos(only? Only : First),
      m_parent(parent)
{
    TU_ASSERT (m_piece != nullptr);
}

tempo_config::internal::MemberPiecePrinter::MemberPiecePrinter(
    const MemberPiece *piece,
    const PrinterState &parent,
    const PrinterState &prev,
    bool last)
    : m_piece(piece),
      m_pos(last? Last : Next),
      m_parent(parent),
      m_prev(prev)
{
    TU_ASSERT (m_piece != nullptr);
}

tempo_utils::Status
tempo_config::internal::MemberPiecePrinter::print(std::string &output, PrinterState &state)
{
    auto *options = state.options;

    bool reformat;
    switch (options->reformat) {
        case Reformat::Full:
            reformat = true;
            break;
        case Reformat::OnlyChanged:
            reformat = m_piece->token.synthesized;
            break;
        case Reformat::None:
            reformat = false;
            break;
    }

    // reformatting mode

    if (reformat) {
        state.append('\n', output);

        std::string leftMargin;
        if (m_pos == First || m_pos == Only) {
            leftMargin = absl::StrCat(m_parent.leftMargin, std::string(options->indent, ' '));
        } else {
            leftMargin = m_prev.leftMargin;
        }

        PrinterState reformatState = state;
        reformatState.leftMargin = leftMargin;

        for (auto *piece : m_piece->before) {
            if (piece->type == PieceType::Whitespace)
                continue;
            if (piece->type == PieceType::Comment) {
                state.append(leftMargin, output);
                CommentPiecePrinter printer(static_cast<CommentPiece *>(piece), reformatState);
                TU_RETURN_IF_NOT_OK (printer.print(output, state));
                state.append('\n', output);
                reformatState = state;
            } else {
                return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                    "unexpected token");
            }
        }
        if (m_piece->key != nullptr) {
            state.append(leftMargin, output);
            auto printer = make_printer(m_piece->key, reformatState);
            TU_RETURN_IF_NOT_OK (printer->print(output, state));
            state.append(": ", output);
            reformatState = state;

            if (m_piece->value == nullptr)
                return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                    "unexpected token");

            for (auto *piece : m_piece->beforeValue) {
                printer = make_printer(piece, m_parent);
                TU_RETURN_IF_NOT_OK (printer->print(output, reformatState));
                reformatState = state;
            }

            printer = make_printer(m_piece->value, m_parent);
            TU_RETURN_IF_NOT_OK (printer->print(output, state));
            if (m_pos == First || m_pos == Next) {
                state.append('\n', output);
            }
            reformatState = state;
        }
        for (auto *piece : m_piece->after) {
            if (piece->type == PieceType::Whitespace)
                continue;
            if (piece->type == PieceType::Comment) {
                state.append(leftMargin, output);
                CommentPiecePrinter printer(static_cast<CommentPiece *>(piece), reformatState);
                TU_RETURN_IF_NOT_OK (printer.print(output, state));
                state.append('\n', output);
                reformatState = state;
            } else {
                return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                    "unexpected token");
            }
        }
        return {};
    }

    // preserving mode

    for (auto *piece : m_piece->before) {
        auto printer = make_printer(piece, m_parent);
        TU_RETURN_IF_NOT_OK (printer->print(output, state));
    }

    if (m_piece->key != nullptr) {
        auto printer = make_printer(m_piece->key, m_parent);
        TU_RETURN_IF_NOT_OK (printer->print(output, state));
        for (auto *piece : m_piece->afterKey) {
            printer = make_printer(piece, m_parent);
            TU_RETURN_IF_NOT_OK (printer->print(output, state));
        }

        state.append(':', output);

        if (m_piece->value == nullptr)
            return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant,
                "unexpected token");

        for (auto *piece : m_piece->beforeValue) {
            printer = make_printer(piece, m_parent);
            TU_RETURN_IF_NOT_OK (printer->print(output, state));
        }
        printer = make_printer(m_piece->value, m_parent);
        TU_RETURN_IF_NOT_OK (printer->print(output, state));
    }

    for (auto *piece : m_piece->after) {
        auto printer = make_printer(piece, m_parent);
        TU_RETURN_IF_NOT_OK (printer->print(output, state));
    }

    return {};
}

tempo_config::internal::ArrayPiecePrinter::ArrayPiecePrinter(
    const ArrayPiece *piece,
    const PrinterState &initial)
    : m_piece(piece),
      m_initial(initial)
{
    TU_ASSERT (m_piece != nullptr);
}

tempo_utils::Status
tempo_config::internal::ArrayPiecePrinter::print(std::string &output, PrinterState &state)
{
    output.push_back('[');
    state.positionInLine++;
    state.totalChars++;
    state.indentLevel++;

    auto numElements = m_piece->elements.size();
    if (numElements == 1) {
        ElementPiecePrinter printer(m_piece->elements.front(), m_initial, /* only= */ true);
        TU_RETURN_IF_NOT_OK (printer.print(output, state));
    } else if (numElements > 1) {
        PrinterState prev;
        int nleft = numElements - 1;

        auto it = m_piece->elements.cbegin();
        if (it != m_piece->elements.cend()) {
            ElementPiecePrinter printer(*it++, m_initial, /* only= */ false);
            TU_RETURN_IF_NOT_OK (printer.print(output, state));
            prev = state;
            nleft--;
        }
        for (; it != m_piece->elements.cend(); ++it) {
            output.push_back(',');
            state.positionInLine++;
            state.totalChars++;
            ElementPiecePrinter printer(*it, m_initial, prev, /* last= */ nleft == 0);
            TU_RETURN_IF_NOT_OK (printer.print(output, state));
            prev = state;
            nleft--;
        }
    }
    for (auto *piece : m_piece->afterElements) {
        auto printer = make_printer(piece, state);
        TU_RETURN_IF_NOT_OK (printer->print(output, state));
    }

    output.push_back(']');
    state.positionInLine++;
    state.totalChars++;
    state.indentLevel--;

    return {};
}

tempo_config::internal::ObjectPiecePrinter::ObjectPiecePrinter(
    const ObjectPiece *piece,
    const PrinterState &initial)
    : m_piece(piece),
      m_initial(initial)
{
    TU_ASSERT (m_piece != nullptr);
}

tempo_utils::Status
tempo_config::internal::ObjectPiecePrinter::print(std::string &output, PrinterState &state)
{
    output.push_back('{');
    state.positionInLine++;
    state.totalChars++;
    state.indentLevel++;

    auto numMembers = m_piece->members.size();
    if (numMembers == 1) {
        MemberPiecePrinter printer(m_piece->members.front(), m_initial, /* only= */ true);
        TU_RETURN_IF_NOT_OK (printer.print(output, state));
    } else if (numMembers > 1) {
        PrinterState prev;
        int nleft = numMembers - 1;

        auto it = m_piece->members.cbegin();
        if (it != m_piece->members.cend()) {
            MemberPiecePrinter printer(*it++, m_initial, /* only= */ false);
            TU_RETURN_IF_NOT_OK (printer.print(output, state));
            prev = state;
            nleft--;
        }
        for (; it != m_piece->members.cend(); ++it) {
            output.push_back(',');
            state.positionInLine++;
            state.totalChars++;
            MemberPiecePrinter printer(*it, m_initial, prev, /* last= */ nleft == 0);
            TU_RETURN_IF_NOT_OK (printer.print(output, state));
            prev = state;
            nleft--;
        }
    }
    for (auto *piece : m_piece->afterMembers) {
        auto printer = make_printer(piece, state);
        TU_RETURN_IF_NOT_OK (printer->print(output, state));
    }

    output.push_back('}');
    state.positionInLine++;
    state.totalChars++;
    state.indentLevel--;

    return {};
}

tempo_config::internal::CommentPiecePrinter::CommentPiecePrinter(
    const CommentPiece *piece,
    const PrinterState &initial)
    : m_piece(piece),
      m_initial(initial)
{
    TU_ASSERT (m_piece != nullptr);
}

tempo_utils::Status
tempo_config::internal::CommentPiecePrinter::print(std::string &output, PrinterState &state)
{
    auto size = m_piece->value.size();
    for (const auto ch : m_piece->value) {
        output.push_back(ch);
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

tempo_config::internal::WhitespacePiecePrinter::WhitespacePiecePrinter(
    const WhitespacePiece *piece,
    const PrinterState &initial)
    : m_piece(piece),
      m_initial(initial)
{
    TU_ASSERT (m_piece != nullptr);
}

tempo_utils::Status
tempo_config::internal::WhitespacePiecePrinter::print(std::string &output, PrinterState &state)
{
    auto size = m_piece->value.size();
    int startOfLine = -1;

    if (state.totalChars == 0) {
        startOfLine = 0;
    }

    for (const auto ch : m_piece->value) {
        output.push_back(ch);

        if (ch == '\n') {
            if (startOfLine > 0) {
                state.leftMargin = output.substr(startOfLine, state.positionInLine);
            }
            state.line++;
            state.positionInLine = 0;
            startOfLine = output.size();
        } else {
            state.positionInLine++;
        }
    }

    if (startOfLine >= 0) {
        state.leftMargin = output.substr(startOfLine, state.positionInLine);
    }

    state.totalChars += size;

    return {};
}

tempo_utils::Status
tempo_config::internal::print_root(const PrinterOptions &options, RootPiece *root, std::string &output)
{
    PrinterState state(&options);
    RootPiecePrinter rootPrinter(root, state);
    TU_RETURN_IF_NOT_OK (rootPrinter.print(output, state));
    return {};
}