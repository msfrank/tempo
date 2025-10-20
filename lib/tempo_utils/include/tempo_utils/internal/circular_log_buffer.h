#ifndef TEMPO_UTILS_INTERNAL_CIRCULAR_LOG_BUFFER_H
#define TEMPO_UTILS_INTERNAL_CIRCULAR_LOG_BUFFER_H

#include <absl/time/time.h>
#include <boost/circular_buffer.hpp>

#include <tempo_utils/logging.h>

namespace tempo_utils::internal {

    struct BufferEntry {
        absl::Time ts;
        LogSeverity severity;
        const char *filePath;
        int lineNr;
        std::string message;
    };

    class CircularLogBuffer final {
    public:
        explicit CircularLogBuffer(int numSlots);

        bool bufferLog(
            const absl::Time &ts,
            LogSeverity severity,
            const char *filePath,
            int lineNr,
            std::string_view message);
        bool flushTo(AbstractLogSink *sink);

    private:
        boost::circular_buffer<BufferEntry> m_buffer;
    };
}

#endif // TEMPO_UTILS_INTERNAL_CIRCULAR_LOG_BUFFER_H