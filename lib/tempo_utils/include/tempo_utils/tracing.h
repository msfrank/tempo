#ifndef TEMPO_UTILS_TRACING_H
#define TEMPO_UTILS_TRACING_H

#include <absl/time/time.h>

#include "integer_types.h"

namespace tempo_utils {

    struct TraceId {

    public:
        TraceId();
        TraceId(tu_uint64 hi, tu_uint64 lo);
        TraceId(tu_uint32 ts, tu_uint32 r1, tu_uint32 r2, tu_uint32 r3);
        TraceId(const TraceId &other);

        bool isValid() const;
        absl::Time getTimestamp() const;
        tu_uint64 getHi() const;
        tu_uint64 getLo() const;

        tu_uint32 getTS() const;
        tu_uint32 getR1() const;
        tu_uint32 getR2() const;
        tu_uint32 getR3() const;

        bool operator==(const TraceId &other) const;
        bool operator!=(const TraceId &other) const;

        static TraceId generate();

    private:
        tu_uint32 m_ts;
        tu_uint32 m_r1;
        tu_uint32 m_r2;
        tu_uint32 m_r3;
    };

    struct SpanId {

    public:
        SpanId();
        SpanId(tu_uint64 id);
        SpanId(tu_uint32 r1, tu_uint32 r2);
        SpanId(const SpanId &other);

        bool isValid() const;
        tu_uint64 getId() const;

        tu_uint32 getR1() const;
        tu_uint32 getR2() const;

        bool operator==(const SpanId &other) const;
        bool operator!=(const SpanId &other) const;

        static SpanId generate();

    private:
        tu_uint32 m_r1;
        tu_uint32 m_r2;
    };
}

#endif // TEMPO_UTILS_TRACING_H