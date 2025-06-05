
#include "bytes_to_code_serializer.h"

int main(int argc, char *argv[])
{
    std::filesystem::path inputDataFile(INPUT_DATA_FILE);
    std::string includeGuard(INCLUDE_GUARD);
    std::string cppNamespace(CPP_NAMESPACE);
    std::filesystem::path outputHeaderFile(OUTPUT_HEADER_FILE);

    BytesToCodeSerializer serializer(inputDataFile, includeGuard, cppNamespace);
    TU_RAISE_IF_NOT_OK (serializer.getStatus());

    tempo_utils::FileWriter writer(outputHeaderFile, serializer.getCode(),
        tempo_utils::FileWriterMode::CREATE_OR_OVERWRITE);
    TU_RAISE_IF_NOT_OK (writer.getStatus());

    return 0;
}
