
#include "base_tracing_fixture.h"

void
BaseTracingFixture::SetUp()
{
    recorder = tempo_tracing::TraceRecorder::create();
}
