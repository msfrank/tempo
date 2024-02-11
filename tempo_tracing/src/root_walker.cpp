
#include <tempo_tracing/internal/spanset_reader.h>
#include <tempo_tracing/root_walker.h>

tempo_tracing::RootWalker::RootWalker()
{
}

tempo_tracing::RootWalker::RootWalker(std::shared_ptr<const internal::SpansetReader> reader)
    : m_reader(reader)
{
    TU_ASSERT (m_reader != nullptr);
}

tempo_tracing::RootWalker::RootWalker(const RootWalker &other)
    : m_reader(other.m_reader)
{
}

bool
tempo_tracing::RootWalker::isValid() const
{
    if (m_reader == nullptr)
        return false;
    return m_reader->isValid();
}

tempo_tracing::SpanWalker
tempo_tracing::RootWalker::getRoot(tu_uint32 index) const
{
    if (!isValid())
        return {};
    if (m_reader->numRoots() <= index)
        return {};
    return SpanWalker(m_reader, m_reader->getRoot(index));
}

int
tempo_tracing::RootWalker::numRoots() const
{
    if (isValid())
        return m_reader->numRoots();
    return 0;
}