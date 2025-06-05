
#include <absl/strings/str_cat.h>

#include "bytes_to_code_serializer.h"

inline const char *endline()
{
    return "\n";
}

static tempo_utils::Status serialize(
    const tempo_utils::FileReader &inputDataReader,
    const std::string &includeGuard,
    const std::string &cppNamespace,
    std::string &code)
{
    auto bytes = inputDataReader.getBytes();
    auto *inputData = (const char *) bytes->getData();
    auto inputSize = bytes->getSize();

    // add timestamp comment
    auto timestamp = absl::FormatTime(absl::Now(), absl::UTCTimeZone());
    absl::StrAppend(&code, "// Generated at ", timestamp, endline());
    absl::StrAppend(&code, endline());

    // add opening conditional compilation directives if includeGuard is specified
    if (!includeGuard.empty()) {
        absl::StrAppend(&code, "#ifndef ", includeGuard, endline());
        absl::StrAppend(&code, "#define ", includeGuard, endline());
        absl::StrAppend(&code, endline());
    }

    // include cstdint for uint32_t and uint8_t
    absl::StrAppend(&code, "#include <cstdint>", endline());
    absl::StrAppend(&code, endline());

    // open the namespace declaration if cppNamespace is specified
    if (!cppNamespace.empty()) {
        absl::StrAppend(&code, "namespace ", cppNamespace, " {", endline());
    }

    // write the const uint32_t array size
    absl::StrAppend(&code, "    constexpr uint32_t size = ", inputSize, ";", endline());

    // write the const uint8_t array of bytes
    absl::StrAppend(&code, "    constexpr uint8_t data[", inputSize + 1, "] = {" , endline());
    uint32_t i = 0;
    while (i < inputSize) {
        absl::StrAppend(&code, "        ");
        for (int j = 0; i < inputSize && j < 16; i++, j++) {
            absl::StrAppend(&code, "0x", absl::Hex(inputData[i], absl::kZeroPad2), ",");
        }
        absl::StrAppend(&code, endline());
    }

    // add a trailing null so that the data array can be used as a \0-terminated string
    absl::StrAppend(&code, "        0", endline());

    // close the data array definition
    absl::StrAppend(&code, "    };", endline());

    // close the namespace declaration if cppNamespace is specified
    if (!cppNamespace.empty()) {
        absl::StrAppend(&code, "}", endline());
        absl::StrAppend(&code, endline());
    }

    // add closing conditional compilation directives if includeGuard is specified
    if (!includeGuard.empty()) {
        absl::StrAppend(&code, "#endif // ", includeGuard, endline());
    }

    return {};
}

BytesToCodeSerializer::BytesToCodeSerializer(
    const std::filesystem::path &inputDataFile,
    const std::string &includeGuard,
    const std::string &cppNamespace)
{
    tempo_utils::FileReader inputDataReader(inputDataFile);
    if (inputDataReader.isValid()) {
        m_status = serialize(inputDataReader, includeGuard, cppNamespace, m_code);
    } else {
        m_status = inputDataReader.getStatus();
    }
}

bool
BytesToCodeSerializer::isValid() const
{
    return m_status.isOk();
}

tempo_utils::Status
BytesToCodeSerializer::getStatus() const
{
    return m_status;
}

std::string_view
BytesToCodeSerializer::getCode() const
{
    return m_code;
}
