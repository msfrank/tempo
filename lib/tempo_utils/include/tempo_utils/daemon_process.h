#ifndef TEMPO_UTILS_DAEMON_PROCESS_H
#define TEMPO_UTILS_DAEMON_PROCESS_H

#include <filesystem>
#include <memory>

#include "process_utils.h"
#include "status.h"

namespace tempo_utils {

    class DaemonProcess : public std::enable_shared_from_this<DaemonProcess> {
    public:
        DaemonProcess(
            const ProcessInvoker &invoker,
            const std::filesystem::path &runDirectory,
            std::string_view childOutput,
            int exitStatus);

        ProcessInvoker getInvoker() const;
        std::filesystem::path getRunDirectory() const;
        std::string getChildOutput() const;
        int getExitStatus() const;

        static std::shared_ptr<DaemonProcess> spawn(
            const ProcessInvoker &invoker,
            const std::filesystem::path &runDirectory);

    private:
        ProcessInvoker m_invoker;
        std::filesystem::path m_runDirectory;
        std::string m_childOutput;
        int m_exitStatus;
    };
}

#endif // TEMPO_UTILS_DAEMON_PROCESS_H