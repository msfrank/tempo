
#include <tempo_utils/platform.h>

const char *
tempo_utils::systemName()
{
    return PLATFORM_SYSTEM_NAME;
}

const char *
tempo_utils::systemVersion()
{
    return PLATFORM_SYSTEM_VERSION;
}

const char *
tempo_utils::architecture()
{
    return PLATFORM_ARCHITECTURE;
}

const char *
tempo_utils::sharedLibraryFileSuffix()
{
    return PLATFORM_SHARED_LIBRARY_SUFFIX;
}

const char *
tempo_utils::sharedLibraryFileDotSuffix()
{
    return "." PLATFORM_SHARED_LIBRARY_SUFFIX;
}

const char *
tempo_utils::sharedLibraryPlatformId()
{
    return PLATFORM_SYSTEM_NAME "-" PLATFORM_ARCHITECTURE;
}
