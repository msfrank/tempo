#ifndef TEMPO_TRACING_TRACING_TYPES_H
#define TEMPO_TRACING_TRACING_TYPES_H

#include <string>
#include <vector>

#include <absl/container/flat_hash_map.h>
#include <absl/time/time.h>

#include <tempo_schema/attr.h>
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

    enum class FailurePropagation {
        NoPropagation,          /**< span does not request any propagation of failure. */
        PropagatesToParent,     /**< span requests that parent be marked failed. */
    };

    enum class FailureCollection {
        IgnoresPropagation,     /**< requests from children to be marked failed are ignored. */
        AnyChildFailed,         /**< a failure propagating from any child will mark the span as failed. */
        AllChildrenFailed,      /**< span is marked failed if all children have propagated failure. */
    };

    struct LogEntry {
        absl::Time ts;
        LogSeverity severity;
        tempo_schema::AttrMap fields;

        LogEntry(absl::Time ts, LogSeverity severity, const tempo_schema::AttrMap &fields);
    };

    struct SpanData {
        const uint32_t spanIndex;
        const tempo_utils::SpanId spanId;
        const uint32_t parentIndex;
        tempo_utils::SpanId parentId;
        std::string operationName;
        tu_int64 startTimeMillisSinceEpoch = -1;
        tu_int64 endTimeMillisSinceEpoch = -1;
        tu_int64 activeTimeNanosSinceEpoch = -1;
        absl::Duration activeDuration;
        std::vector<uint32_t> children;
        tempo_schema::AttrMap tags;
        std::vector<LogEntry> logs;
        FailurePropagation propagation = FailurePropagation::NoPropagation;
        FailureCollection collection = FailureCollection::IgnoresPropagation;
        bool failed = false;
        bool complete = false;

        SpanData(tu_uint32 index, tempo_utils::SpanId id);
        SpanData(tu_uint32 spanIndex, tempo_utils::SpanId spanId, tu_uint32 parentIndex, tempo_utils::SpanId parentId);
    };

    // forward declarations
    namespace internal {
        class SpansetReader;
    }
}

#endif // TEMPO_TRACING_TRACING_TYPES_H