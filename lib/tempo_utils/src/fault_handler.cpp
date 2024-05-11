#include <iostream>

#include <tempo_utils/fault_handler.h>

#define BOOST_STACKTRACE_GNU_SOURCE_NOT_REQUIRED 1

#include <boost/stacktrace.hpp>

void tempo_utils::invoke_fault_handler(int signum, const char *message)
{
    std::cerr << "INVOKED FAULT HANDLER";
    if (signum < 0) {
        std::cerr << std::endl;
    } else {
        std::cerr << " (caught signal " << signum << ")" << std::endl;
    }
    if (message != nullptr) {
        std::cerr << "Message was: " << message << std::endl;
    }
    // TODO: capture stacktrace in Status instead of printing it
    //std::cerr << boost::stacktrace::stacktrace();
}
