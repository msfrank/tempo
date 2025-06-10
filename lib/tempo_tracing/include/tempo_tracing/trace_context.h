#ifndef TEMPO_TRACING_THREAD_CONTEXT_H
#define TEMPO_TRACING_THREAD_CONTEXT_H

#include <thread>
#include <tempo_utils/status.h>

#include "trace_recorder.h"

namespace tempo_tracing {

    class TraceContext {
    public:
        ~TraceContext();

        static std::shared_ptr<TraceContext> currentContext();
        static tempo_utils::Result<std::shared_ptr<TraceContext>> switchCurrent(std::string_view name);
        static tempo_utils::Status clearCurrent();

        static bool hasContext(std::string_view name);
        static std::shared_ptr<TraceContext> getContext(std::string_view name);

        static tempo_utils::Result<std::shared_ptr<TraceContext>> makeContext(std::string_view name = {});
        static tempo_utils::Result<std::shared_ptr<TraceContext>> makeContextAndSwitch(std::string_view name = {});
        static tempo_utils::Result<TempoSpanset> finishContext(std::string_view name);

        static tempo_utils::Result<std::shared_ptr<TraceContext>> makeUnownedContext(
            std::shared_ptr<TraceRecorder> recorder,
            std::string_view name = {});
        static tempo_utils::Result<std::shared_ptr<TraceContext>> makeUnownedContextAndSwitch(
            std::shared_ptr<TraceRecorder> recorder,
            std::string_view name = {});
        static tempo_utils::Result<std::shared_ptr<TraceRecorder>> releaseContext(std::string_view name);

        std::string getName() const;
        tempo_utils::TraceId getTraceId() const;
        std::thread::id getThreadId() const;
        bool isUnowned() const;

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
        std::thread::id m_tid;
        std::shared_ptr<TraceRecorder> m_recorder;
        bool m_unowned;
        std::vector<std::shared_ptr<TraceSpan>> m_spanStack;
        bool m_isActive;

        TraceContext(std::string_view name, std::shared_ptr<TraceRecorder> recorder, bool unowned);
        void checkCurrentThreadOrThrow() const;

        friend class EnterScope;
        friend class CurrentScope;
        friend class ExitScope;
    };

    class ReleaseContext {
    public:
        explicit ReleaseContext(std::string_view name);
        explicit ReleaseContext(std::shared_ptr<TraceContext> context);
        ~ReleaseContext();
    private:
        std::shared_ptr<TraceContext> m_context;
    };
}

#endif // TEMPO_TRACING_THREAD_CONTEXT_H
