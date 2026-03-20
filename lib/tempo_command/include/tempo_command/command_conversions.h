#ifndef TEMPO_COMMAND_COMMAND_CONVERSIONS_H
#define TEMPO_COMMAND_COMMAND_CONVERSIONS_H

#include <tempo_config/base_conversions.h>
#include <tempo_config/enum_conversions.h>

namespace tempo_command {

    /**
     *
     */
    class SeverityFilterParser : public tempo_config::EnumTParser<tempo_utils::SeverityFilter> {
    public:
        explicit SeverityFilterParser(tempo_utils::SeverityFilter defaultSeverityFilter)
            : EnumTParser({
            {"Default", tempo_utils::SeverityFilter::kDefault},
            {"Silent", tempo_utils::SeverityFilter::kSilent},
            {"ErrorsOnly", tempo_utils::SeverityFilter::kErrorsOnly},
            {"WarningsAndErrors", tempo_utils::SeverityFilter::kWarningsAndErrors},
            {"Verbose", tempo_utils::SeverityFilter::kVerbose},
            {"VeryVerbose", tempo_utils::SeverityFilter::kVeryVerbose},
            }, defaultSeverityFilter) {}
        SeverityFilterParser() : SeverityFilterParser(tempo_utils::SeverityFilter::kDefault) {}
    };
}

#endif // TEMPO_COMMAND_COMMAND_CONVERSIONS_H