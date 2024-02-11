#ifndef TEMPO_UTILS_FAULT_HANDLER_H
#define TEMPO_UTILS_FAULT_HANDLER_H

namespace tempo_utils {

    /**
     * If configured then invoke the fault handler, which may capture a stack trace
     * or other runtime information. This function may or may not abort execution.
     *
     * @param signum If this function is called from a signal handler on a POSIX system,
     *   then the signal handler can pass the signal number through this argument.
     */
    void invoke_fault_handler(int signum, const char *message);

};

#endif // TEMPO_UTILS_FAULT_HANDLER_H
