#ifndef TEMPO_UTILS_PLATFORM_H
#define TEMPO_UTILS_PLATFORM_H

namespace tempo_utils {

    const char *systemName();

    const char *systemVersion();

    const char *architecture();

    const char *sharedLibraryFileSuffix();

    const char *sharedLibraryFileDotSuffix();

    const char *sharedLibraryPlatformId();
}

#endif // TEMPO_UTILS_PLATFORM_H
