
#include <unistd.h>

#include <tempo_utils/program_location.h>

int
main(int argc, char *argv[])
{
    bool printPath = false;
    bool printDirectory = false;

    int ch;
    while ((ch = getopt(argc, argv, "pd")) != -1) {
        switch (ch) {
        case 'p':
            printPath = true;
            break;
        case 'd':
            printDirectory = true;
            break;
        default:
            return -1;
        }
    }
    argc -= optind;
    argv += optind;

    if (printPath) {
        auto result = tempo_utils::get_program_path();
        if (result.isStatus()) {
            auto status = result.getStatus();
            fprintf(stdout, "%s", status.toString().c_str());
            return 1;
        }
        auto path = result.getResult();
        fprintf(stdout, "%s", path.c_str());
    }

    if (printDirectory) {
        auto result = tempo_utils::get_program_directory();
        if (result.isStatus()) {
            auto status = result.getStatus();
            fprintf(stdout, "%s", status.toString().c_str());
            return 1;
        }
        auto path = result.getResult();
        fprintf(stdout, "%s", path.c_str());
    }

    return 0;
}
