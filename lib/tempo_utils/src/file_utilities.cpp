#include <random>

#include <absl/strings/match.h>
#include <absl/strings/str_replace.h>

#include <tempo_utils/file_utilities.h>

#define TEMPLATE_REPLACEMENT    "XXXXXXXX"

static inline
char base62_to_ascii(unsigned char b62)
{
    if (b62 < 10)
        return '0' + b62;
    if (b62 < 36)
        return 'A' + (b62 - 10);
    return 'a' + (b62 - 36);
}

bool
tempo_utils::is_valid_tempname(std::string_view tempname)
{
    return absl::StrContains(tempname, TEMPLATE_REPLACEMENT)
           && !absl::StrContains(tempname, "/");
}

std::string
tempo_utils::generate_name(std::string_view tempname)
{
    std::mt19937 randengine{std::random_device()()};
    std::uniform_int_distribution<unsigned char> chargen(0, 61);

    std::string filename;
    for (int i = 0; i < 8; i++) {
        filename.push_back(base62_to_ascii(chargen(randengine)));
    }

    return absl::StrReplaceAll(tempname, {{TEMPLATE_REPLACEMENT, filename}});
}