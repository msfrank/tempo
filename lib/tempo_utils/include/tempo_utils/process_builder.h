#ifndef TEMPO_UTILS_PROCESS_BUILDER_H
#define TEMPO_UTILS_PROCESS_BUILDER_H

#include <filesystem>
#include <string>
#include <vector>

#include "log_message.h"

namespace tempo_utils {

    /**
     * ProcessInvoker encapsulates a path to an executable and its arguments.
     */
    class ProcessInvoker {
    public:
        ProcessInvoker();
        ProcessInvoker(const std::filesystem::path &exe, const std::vector<std::string> &args);
        ProcessInvoker(const ProcessInvoker &other);
        ~ProcessInvoker();

        bool isValid() const;
        const char *getExecutable() const;
        std::filesystem::path getExecutablePath() const;
        const char *getArg(int index) const;
        int getArgc() const;
        char **getArgv() const;

        std::string toString() const;

    private:
        std::filesystem::path m_exe;
        int m_argc;
        char **m_argv;
    };

    tempo_utils::LogMessage&& operator<<(tempo_utils::LogMessage &&message, const ProcessInvoker &invoker);

    /**
     * ProcessBuilder helps build a ProcessInvoker by adding arguments incrementally.
     */
    class ProcessBuilder {
    public:
        ProcessBuilder(const std::filesystem::path &executable);
        ProcessBuilder(const std::filesystem::path &executable, std::string_view arg0);

        void appendArg(std::string_view arg);
        void appendArg(std::string_view option, std::string_view value);
        ProcessInvoker toInvoker() const;

        std::string toString() const;

    private:
        std::filesystem::path m_exe;
        std::vector<std::string> m_args;
    };
}

#endif // TEMPO_UTILS_PROCESS_BUILDER_H