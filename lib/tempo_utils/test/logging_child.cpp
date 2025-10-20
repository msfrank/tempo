
#include <tempo_utils/log_stream.h>
#include <tempo_utils/logging.h>

int
main(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++) {
        std::string line(argv[i]);
        if (line == "--start") {
            tempo_utils::LoggingConfiguration config;
            tempo_utils::init_logging(config, true, true);
        } else if (line == "--stop") {
            tempo_utils::cleanup_logging(false);
        } else if (line == "--finish") {
            tempo_utils::cleanup_logging(true);
        } else {
            TU_LOG_INFO << "" << line;
        }
    }
    return 0;
}