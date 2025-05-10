#ifndef TEMPO_UTILS_PROCESS_RUNNER_H
#define TEMPO_UTILS_PROCESS_RUNNER_H

#include <filesystem>
#include <memory>

#include "process_builder.h"
#include "status.h"

namespace tempo_utils {

    class ProcessRunner {
    public:
        explicit ProcessRunner(const ProcessInvoker &invoker);
        ProcessRunner(
            const ProcessInvoker &invoker,
            const std::filesystem::path &runDirectory);
        ProcessRunner(const ProcessRunner &other) = delete;
        ProcessRunner(ProcessRunner &&other) = delete;

        bool isValid() const;
        Status getStatus() const;
        ProcessInvoker getInvoker() const;
        std::filesystem::path getRunDirectory() const;
        std::string getChildOutput() const;
        std::string getChildError() const;
        int getExitStatus() const;

    private:
        ProcessInvoker m_invoker;
        std::filesystem::path m_runDirectory;
        std::string m_childOutput;
        std::string m_childError;
        int m_exitStatus;
        Status m_status;
    };
}

#endif // TEMPO_UTILS_PROCESS_RUNNER_H