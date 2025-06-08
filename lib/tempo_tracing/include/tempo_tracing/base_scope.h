#ifndef TEMPO_TRACING_BASE_SCOPE_H
#define TEMPO_TRACING_BASE_SCOPE_H

#include "trace_context.h"

namespace tempo_tracing {

    class BaseScope {
    public:
        explicit BaseScope(std::string_view contextName);

        virtual std::shared_ptr<TraceSpan> getSpan() const = 0;

        bool isValid() const;
        tempo_utils::Status getStatus() const;

        void setOperationName(std::string_view name);
        void setPropagation(FailurePropagation propagation);
        void setCollection(FailureCollection collection);
        void setFailed(bool failed);

        std::shared_ptr<SpanLog> appendLog(absl::Time ts, LogSeverity severity);
        std::shared_ptr<SpanLog> logMessage(std::string_view message, absl::Time ts, LogSeverity severity);
        std::shared_ptr<SpanLog> logMessage(std::string_view message, LogSeverity severity);
        std::shared_ptr<SpanLog> logStatus(const tempo_utils::Status &status, absl::Time ts, LogSeverity severity);
        std::shared_ptr<SpanLog> logStatus(const tempo_utils::Status &status, LogSeverity severity);

    protected:
        std::shared_ptr<TraceContext> m_context;
        tempo_utils::Status m_status;

        bool checkCurrentContext();
    };
}

#endif // TEMPO_TRACING_BASE_SCOPE_H
