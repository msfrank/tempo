
#include <tempo_tracing/tracing_types.h>

tempo_tracing::LogEntry::LogEntry(
    absl::Time ts,
    LogSeverity severity,
    const tempo_schema::AttrMap &fields)
    : ts(ts),
      severity(severity),
      fields(fields)
{
}

tempo_tracing::SpanData::SpanData(uint32_t index, tempo_utils::SpanId id)
    : spanIndex(index),
      spanId(id),
      parentIndex(kInvalidAddressU32),
      parentId(),
      startTime(),
      endTime(),
      complete(false)
{
}

tempo_tracing::SpanData::SpanData(
    tu_uint32 spanIndex,
    tempo_utils::SpanId spanId,
    tu_uint32 parentIndex,
    tempo_utils::SpanId parentId)
    : spanIndex(spanIndex),
      spanId(spanId),
      parentIndex(parentIndex),
      parentId(parentId),
      startTime(),
      endTime(),
      complete(false)
{
}