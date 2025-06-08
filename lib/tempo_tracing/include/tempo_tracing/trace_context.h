#ifndef TEMPO_TRACING_THREAD_CONTEXT_H
#define TEMPO_TRACING_THREAD_CONTEXT_H

#include <tempo_utils/status.h>

#include "trace_recorder.h"

namespace tempo_tracing {

    class TraceContext {
    public:
        TraceContext(std::string_view name, std::shared_ptr<TraceRecorder> recorder);
        ~TraceContext();

        // thread local methods
        static TraceContext *currentContext();
        static TraceContext *getContext(std::string_view name);
        static tempo_utils::Result<TraceContext *> switchCurrent(std::string_view name);
        static tempo_utils::Status clearCurrent();
        static tempo_utils::Result<TraceContext *> makeContext(std::string_view name = {});
        static tempo_utils::Result<TraceContext *> makeContextAndSwitch(std::string_view name = {});
        static tempo_utils::Result<TempoSpanset> finishContext(std::string_view name);

        std::string getName() const;
        tempo_utils::TraceId getTraceId() const;

        bool isActive() const;
        void activate();
        void deactivate();

        std::shared_ptr<TraceSpan> pushSpan(
            FailurePropagation propagation,
            FailureCollection collection);
        std::shared_ptr<TraceSpan> popSpan();
        std::shared_ptr<TraceSpan> popSpanAndCheck(const tempo_utils::SpanId &spanId);
        std::shared_ptr<TraceSpan> peekSpan() const;
        std::shared_ptr<TraceSpan> peekSpanAndCheck(const tempo_utils::SpanId &spanId) const;

        bool isEmpty() const;
        std::shared_ptr<TraceSpan> getSpan(int index) const;
        int numSpans() const;

        tempo_utils::Result<TempoSpanset> finish();

    private:
        std::string m_name;
        std::shared_ptr<TraceRecorder> m_recorder;
        std::vector<std::shared_ptr<TraceSpan>> m_spanStack;
        bool m_isActive;

        friend class EnterScope;
        friend class CurrentScope;
        friend class ExitScope;
    };
}

#endif // TEMPO_TRACING_THREAD_CONTEXT_H
