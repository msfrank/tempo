#ifndef TEMPO_UTILS_USER_HOME_H
#define TEMPO_UTILS_USER_HOME_H

#include <filesystem>

namespace tempo_utils {

    std::filesystem::path get_user_home_directory();
}

#endif // TEMPO_UTILS_USER_HOME_H
