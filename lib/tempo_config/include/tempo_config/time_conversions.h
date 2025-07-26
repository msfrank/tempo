#ifndef TEMPO_CONFIG_TIME_CONVERSIONS_H
#define TEMPO_CONFIG_TIME_CONVERSIONS_H

#include <absl/time/time.h>

#include "abstract_converter.h"

namespace tempo_config {

    class TimeParser : public AbstractConverter<absl::Time> {
    public:
        explicit TimeParser(std::string_view timeFormat);
        TimeParser(std::string_view timeFormat, const absl::Time &timeDefault);
        tempo_utils::Status convertValue(const ConfigNode &node, absl::Time &time) const override;

    private:
        std::string m_timeFormat;
        Option<absl::Time> m_default;
    };

    class DurationParser : public AbstractConverter<absl::Duration> {
    public:
        DurationParser();
        explicit DurationParser(const absl::Duration &durationDefault);
        tempo_utils::Status convertValue(const ConfigNode &node, absl::Duration &duration) const override;

    private:
        Option<absl::Duration> m_default;
    };

    class TimeZoneParser : public AbstractConverter<absl::TimeZone> {
    public:
        TimeZoneParser();
        explicit TimeZoneParser(const absl::TimeZone &tzDefault);
        tempo_utils::Status convertValue(const ConfigNode &node, absl::TimeZone &tz) const override;

    private:
        Option<absl::TimeZone> m_default;
    };
}

#endif // TEMPO_CONFIG_TIME_CONVERSIONS_H
