
#include <boost/predef.h>

#include <tempo_utils/file_result.h>
#include <tempo_utils/library_loader.h>
#include <tempo_utils/library_result.h>
#include <tempo_utils/log_stream.h>

// forward declaration for our platform specific loader

static tempo_utils::Status
load_library(const std::filesystem::path &path, const char *symbol, void **handle);

tempo_utils::LibraryLoader::LibraryLoader(
    const std::filesystem::path &path,
    const char *symbolName)
{
    m_absolutePath = path;
    m_status = load_library(path, symbolName, &m_symbol);
}

bool
tempo_utils::LibraryLoader::isValid() const
{
    return m_status.isOk();
}

tempo_utils::Status
tempo_utils::LibraryLoader::getStatus() const
{
    return m_status;
}

void *
tempo_utils::LibraryLoader::symbolPointer() const
{
    return m_symbol;
}

/*
 * Code below is platform specific.
 */

#ifdef BOOST_OS_MACOS

#include <dlfcn.h>

static tempo_utils::Status
load_library(const std::filesystem::path &path, const char *symbol, void **ptr)
{
    TU_ASSERT (ptr != nullptr);

    if (!std::filesystem::exists(path))
        return tempo_utils::FileStatus::forCondition(
            tempo_utils::FileCondition::kFileNotFound, "missing shared library {}", path.string());

    auto *handle = dlopen(path.c_str(), RTLD_LAZY);
    if (handle == nullptr)
        return tempo_utils::LibraryStatus::forCondition(
            tempo_utils::LibraryCondition::kLibraryInvariant, "failed to open shared object {}: {}",
            path.string(), dlerror());

    *ptr = dlsym(handle, symbol);
    if (*ptr == nullptr)
        return tempo_utils::LibraryStatus::forCondition(
            tempo_utils::LibraryCondition::kLibraryInvariant,
            "failed to resolve symbol '{}' in shared object {}: {}", symbol, path.string(), dlerror());

    return tempo_utils::LibraryStatus::ok();
}

#elif BOOST_OS_LINUX

static tempo_utils::Status
load_library(const std::filesystem::path &path, const char *symbol, void **ptr)
{
    TU_UNREACHABLE();
}

#elif BOOST_OS_WINDOWS

static tempo_utils::Status
load_library(const std::filesystem::path &path, const char *symbol, void **ptr)
{
    TU_UNREACHABLE();
}

#endif
