#ifndef TEMPO_TRACING_TRACING_TYPES_H
#define TEMPO_TRACING_TRACING_TYPES_H

#include <string>
#include <vector>

#include <absl/container/flat_hash_map.h>
#include <absl/time/time.h>

#include <tempo_utils/attr.h>
#include <tempo_utils/tracing.h>

namespace tempo_tracing {

    constexpr tu_uint32 kInvalidAddressU32 = 0xFFFFFFFF;

    enum class SpansetVersion {
        Unknown,
        Version1,
    };

    enum class LogSeverity {
        kFatal,
        kError,
        kWarn,
        kInfo,
        kVerbose,
        kVeryVerbose,
    };

    struct LogEntry {
        absl::Time ts;
        LogSeverity severity;
        tempo_utils::AttrMap fields;

        LogEntry(absl::Time ts, LogSeverity severity, const tempo_utils::AttrMap &fields);
    };

    struct SpanData {
        const uint32_t spanIndex;
        const tempo_utils::SpanId spanId;
        const uint32_t parentIndex;
        tempo_utils::SpanId parentId;
        std::string operationName;
        absl::Time startTime;
        absl::Time endTime;
        absl::Duration activeTime;
        std::vector<uint32_t> children;
        tempo_utils::AttrMap tags;
        std::vector<LogEntry> logs;
        bool error;
        bool complete;

        SpanData(tu_uint32 index, tempo_utils::SpanId id);
        SpanData(tu_uint32 spanIndex, tempo_utils::SpanId spanId, tu_uint32 parentIndex, tempo_utils::SpanId parentId);
    };

    // forward declarations
    namespace internal {
        class SpansetReader;
    }
}

#endif // TEMPO_TRACING_TRACING_TYPES_H