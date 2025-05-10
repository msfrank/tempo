
#include <stdio.h>
#include <unistd.h>

#include <tempo_utils/log_message.h>

int
main(int argc, char *argv[])
{
    std::string outputString;
    std::string errorString;
    int exitStatus = 0;

    int ch;
    while ((ch = getopt(argc, argv, "o:e:x:")) != -1) {
        switch (ch) {
            case 'o':
                outputString = optarg;
                break;
            case 'e':
                errorString = optarg;
                break;
            case 'x':
                exitStatus = atoi(optarg);
                break;
            default:
                return -1;
        }
    }
    argc -= optind;
    argv += optind;

    if (!outputString.empty()) {
        fprintf(stdout, "%s", outputString.c_str());
    }
    if (!errorString.empty()) {
        fprintf(stderr, "%s", errorString.c_str());
    }

    return exitStatus;
}
