
#include <boost/predef.h>

#include <tempo_utils/user_home.h>

static std::filesystem::path get_user_homedir();

std::filesystem::path
tempo_utils::get_user_home_directory()
{
    return get_user_homedir();
}

#ifdef BOOST_OS_MACOS

#include <pwd.h>
#include <stdlib.h>
#include <unistd.h>

static std::filesystem::path get_user_homedir()
{
    auto *envHome = getenv("HOME");
    if (envHome) {
        std::filesystem::path homeDirectory(envHome);
        if (exists(homeDirectory) && is_directory(homeDirectory))
            return homeDirectory;
    }

    auto *pw = getpwuid(getuid());
    if (pw == nullptr)
        return {};

    if (pw->pw_dir == nullptr)
        return {};

    return std::filesystem::path(pw->pw_dir);
}

#elif BOOST_OS_LINUX

static std::filesystem::path get_user_homedir()
{
    TU_UNREACHABLE();
}

#elif BOOST_OS_WINDOWS

static std::filesystem::path get_user_homedir()
{
    TU_UNREACHABLE();
}

#endif
