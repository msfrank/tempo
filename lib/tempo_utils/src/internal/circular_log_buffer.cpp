
#include <tempo_utils/internal/circular_log_buffer.h>

tempo_utils::internal::CircularLogBuffer::CircularLogBuffer(int numSlots)
    : m_buffer(numSlots)
{
}

bool
tempo_utils::internal::CircularLogBuffer::bufferLog(
    const absl::Time &ts,
    LogSeverity severity,
    const char *filePath,
    int lineNr,
    std::string_view message)
{
    BufferEntry entry;
    entry.ts = ts;
    entry.severity = severity;
    entry.filePath = filePath;
    entry.lineNr = lineNr;
    entry.message = message;
    m_buffer.push_back(std::move(entry));
    return true;
}

bool
tempo_utils::internal::CircularLogBuffer::flushTo(AbstractLogSink *sink)
{
    for (const auto &entry : m_buffer) {
        sink->writeLog(entry.ts, entry.severity, entry.filePath, entry.lineNr, entry.message);
    }
    m_buffer.clear();
    return true;
}
