
#include <tempo_tracing/error_walker.h>
#include <tempo_tracing/internal/spanset_reader.h>

tempo_tracing::ErrorWalker::ErrorWalker()
{
}

tempo_tracing::ErrorWalker::ErrorWalker(std::shared_ptr<const internal::SpansetReader> reader)
    : m_reader(reader)
{
}

tempo_tracing::ErrorWalker::ErrorWalker(const ErrorWalker &other)
    : m_reader(other.m_reader)
{
}

bool
tempo_tracing::ErrorWalker::isValid() const
{
    if (m_reader == nullptr)
        return false;
    return m_reader->isValid();
}

tempo_tracing::SpanWalker
tempo_tracing::ErrorWalker::getError(tu_uint32 index) const
{
    if (!isValid())
        return {};
    if (m_reader->numErrors() <= index)
        return {};
    return SpanWalker(m_reader, m_reader->getError(index));
}

int
tempo_tracing::ErrorWalker::numErrors() const
{
    if (isValid())
        return m_reader->numErrors();
    return 0;
}