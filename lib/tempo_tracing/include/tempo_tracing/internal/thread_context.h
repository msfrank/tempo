#ifndef TEMPO_TRACING_INTERNAL_THREAD_CONTEXT_H
#define TEMPO_TRACING_INTERNAL_THREAD_CONTEXT_H

#include "../trace_context.h"

namespace tempo_tracing::internal {

    struct ThreadContext {
        absl::flat_hash_map<
            std::string,
            std::shared_ptr<tempo_tracing::TraceContext>> contexts;
        std::shared_ptr<tempo_tracing::TraceContext> currentContext;
    };

    ThreadContext& get_thread_context();
    void reset_thread_context();
}

#endif // TEMPO_TRACING_INTERNAL_THREAD_CONTEXT_H
