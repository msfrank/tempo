#ifndef BYTES_TO_CODE_WRITER_H
#define BYTES_TO_CODE_WRITER_H

#include <filesystem>

#include <tempo_utils/tempo_utils.h>

class BytesToCodeSerializer {

public:
    BytesToCodeSerializer(
        const std::filesystem::path &inputDataFile,
        const std::string &includeGuard,
        const std::string &cppNamespace);

    bool isValid() const;
    tempo_utils::Status getStatus() const;
    std::string_view getCode() const;

private:
    tempo_utils::Status m_status;
    std::string m_code;
};

#endif //BYTES_TO_CODE_WRITER_H
