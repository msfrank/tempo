#include <gtest/gtest.h>

#include <absl/synchronization/notification.h>

#include <tempo_tracing/internal/thread_context.h>
#include <tempo_tracing/trace_context.h>

void
create_context_and_wait(
    std::shared_ptr<tempo_tracing::TraceContext> &context,
    absl::Notification &notifyReady,
    absl::Notification &notifyDone)
{
    TU_ASSIGN_OR_RAISE (context, tempo_tracing::TraceContext::makeContext("test"));
    notifyReady.Notify();
    notifyDone.WaitForNotificationWithTimeout(absl::Seconds(5));
}

class MultiThreading : public ::testing::Test {
protected:
    std::shared_ptr<tempo_tracing::TraceContext> context;
    absl::Notification notifyReady;
    absl::Notification notifyDone;
    std::thread m_thread;

    void SetUp() override {
        std::thread t(create_context_and_wait, std::ref(context), std::ref(notifyReady), std::ref(notifyDone));
        m_thread = std::move(t);
        notifyReady.WaitForNotificationWithTimeout(absl::Seconds(5));
    }
    void TearDown() override {
        notifyDone.Notify();
        m_thread.join();
        tempo_tracing::internal::reset_thread_context();
    }
};

TEST_F(MultiThreading, ContextOperationsCannotCrossThreads)
{
    ASSERT_THROW({ context->isEmpty(); }, tempo_utils::StatusException);
}

TEST_F(MultiThreading, ImmutableAccessorsCanCrossThreads)
{
    ASSERT_NO_THROW({ context->getName(); });
    ASSERT_EQ ("test", context->getName());
}
