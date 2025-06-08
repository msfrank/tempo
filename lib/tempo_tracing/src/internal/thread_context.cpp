
#include <tempo_tracing/internal/thread_context.h>

thread_local tempo_tracing::internal::ThreadContext thread_context;

tempo_tracing::internal::ThreadContext&
tempo_tracing::internal::get_thread_context()
{
    return thread_context;
}

void
tempo_tracing::internal::reset_thread_context()
{
    thread_context.currentContext = nullptr;
    thread_context.contexts.clear();
}
