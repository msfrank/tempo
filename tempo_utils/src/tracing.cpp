
#include <random>

#include <tempo_utils/tracing.h>

/**
 *
 * @return
 */
static tu_uint32 generate_u32() {
    static thread_local std::mt19937 randengine{std::random_device()()};
    static thread_local std::uniform_int_distribution<tu_uint32> u32gen;
    return u32gen(randengine);
}

tempo_utils::TraceId::TraceId()
    : m_ts(0), m_r1(0), m_r2(0), m_r3(0)
{
}

tempo_utils::TraceId::TraceId(tu_uint64 hi, tu_uint64 lo)
    : m_ts(static_cast<tu_uint32>(hi >> 32)),
      m_r1(static_cast<tu_uint32>(hi & 0x00000000FFFFFFFF)),
      m_r2(static_cast<tu_uint32>(lo >> 32)),
      m_r3(static_cast<tu_uint32>(lo & 0x00000000FFFFFFFF))
{
}

tempo_utils::TraceId::TraceId(tu_uint32 ts, tu_uint32 r1, tu_uint32 r2, tu_uint32 r3)
    : m_ts(ts), m_r1(r1), m_r2(r2), m_r3(r3)
{
}

tempo_utils::TraceId::TraceId(const TraceId &other)
    : m_ts(other.m_ts), m_r1(other.m_r1), m_r2(other.m_r2), m_r3(other.m_r3)
{
}

bool
tempo_utils::TraceId::isValid() const
{
    return !(m_ts == 0 && m_r1 == 0 && m_r2 == 0 && m_r3 == 0);
}

absl::Time
tempo_utils::TraceId::getTimestamp() const
{
    return absl::FromUnixSeconds(m_ts);
}

tu_uint64
tempo_utils::TraceId::getHi() const
{
    return (static_cast<tu_uint64>(m_ts) << 32) |  static_cast<tu_uint64>(m_r1);
}

tu_uint64
tempo_utils::TraceId::getLo() const
{
    return (static_cast<tu_uint64>(m_r2) << 32) |  static_cast<tu_uint64>(m_r3);
}

tu_uint32
tempo_utils::TraceId::getTS() const
{
    return m_ts;
}

tu_uint32
tempo_utils::TraceId::getR1() const
{
    return m_r1;
}

tu_uint32
tempo_utils::TraceId::getR2() const
{
    return m_r2;
}

tu_uint32
tempo_utils::TraceId::getR3() const
{
    return m_r3;
}

bool
tempo_utils::TraceId::operator==(const TraceId &other) const
{
    return m_ts == other.m_ts
           && m_r1 == other.m_r1
           && m_r2 == other.m_r2
           && m_r3 == other.m_r3;
}

bool
tempo_utils::TraceId::operator!=(const TraceId &other) const
{
    return m_ts != other.m_ts
           || m_r1 != other.m_r1
           || m_r2 != other.m_r2
           || m_r3 != other.m_r3;
}

tempo_utils::TraceId
tempo_utils::TraceId::generate()
{
    auto ts = ToUnixSeconds(absl::UnixEpoch());
    return TraceId(static_cast<tu_uint32>(ts), generate_u32(), generate_u32(), generate_u32());
}

tempo_utils::SpanId::SpanId()
    : m_r1(0), m_r2(0)
{
}

tempo_utils::SpanId::SpanId(tu_uint64 id)
    : m_r1(static_cast<tu_uint32>(id >> 32)),
      m_r2(static_cast<tu_uint32>(id & 0x00000000FFFFFFFF))
{
}

tempo_utils::SpanId::SpanId(tu_uint32 r1, tu_uint32 r2)
    : m_r1(r1), m_r2(r2)
{
}

tempo_utils::SpanId::SpanId(const SpanId &other)
    : m_r1(other.m_r1), m_r2(other.m_r2)
{
}

bool
tempo_utils::SpanId::isValid() const
{
    return !(m_r1 == 0 && m_r2 == 0);
}

tu_uint64
tempo_utils::SpanId::getId() const
{
    return (static_cast<tu_uint64>(m_r1) << 32) |  static_cast<tu_uint64>(m_r2);
}

tu_uint32
tempo_utils::SpanId::getR1() const
{
    return m_r1;
}

tu_uint32
tempo_utils::SpanId::getR2() const
{
    return m_r2;
}

bool
tempo_utils::SpanId::operator==(const SpanId &other) const
{
    return m_r1 == other.m_r1 && m_r2 == other.m_r2;
}

bool
tempo_utils::SpanId::operator!=(const SpanId &other) const
{
    return m_r1 != other.m_r1 || m_r2 != other.m_r2;
}

tempo_utils::SpanId
tempo_utils::SpanId::generate()
{
    return SpanId(generate_u32(), generate_u32());
}
