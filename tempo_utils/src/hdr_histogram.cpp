
#include <hdr/hdr_histogram.h>

#include <tempo_utils/hdr_histogram.h>
#include <tempo_utils/log_stream.h>

typedef struct hdr_histogram hdr_histogram_t;

tempo_utils::HdrHistogram::HdrHistogram()
{
    m_histogram = nullptr;
}

tempo_utils::HdrHistogram::HdrHistogram(
    tu_int64 lowestTrackableValue,
    tu_int64 highestTrackableValue,
    int significantFigures)
{
    TU_ASSERT(lowestTrackableValue > 0);
    TU_ASSERT(highestTrackableValue > 1);
    TU_ASSERT(1 <= significantFigures && significantFigures <= 5);
    m_histogram = nullptr;
    hdr_init(lowestTrackableValue, highestTrackableValue, significantFigures, (hdr_histogram_t **) &m_histogram);
}

tempo_utils::HdrHistogram::HdrHistogram(const HdrHistogram &other)
{
    if (other.m_histogram != nullptr) {
        size_t size = hdr_get_memory_size((hdr_histogram_t *) other.m_histogram);
        m_histogram = (hdr_histogram_t *) malloc(size);
        memcpy(m_histogram, other.m_histogram, size);
    } else {
        m_histogram = nullptr;
    }
}

tempo_utils::HdrHistogram::~HdrHistogram()
{
    if (m_histogram)
        free(m_histogram);
}

bool
tempo_utils::HdrHistogram::isValid() const
{
    return m_histogram != nullptr;
}

bool
tempo_utils::HdrHistogram::record(int64_t value, int64_t count)
{
    if (m_histogram)
        return hdr_record_values((hdr_histogram_t *) m_histogram, value, count);
    return false;
}

bool
tempo_utils::HdrHistogram::recordCorrected(int64_t value, int64_t expectedInterval, int64_t count)
{
    if (m_histogram)
        return hdr_record_corrected_values((hdr_histogram_t *) m_histogram, value, count, expectedInterval);
    return false;
}

int64_t
tempo_utils::HdrHistogram::getMinValue() const
{
    if (m_histogram)
        return hdr_min((hdr_histogram_t *) m_histogram);
    return 0;
}

int64_t
tempo_utils::HdrHistogram::getMaxValue() const
{
    if (m_histogram)
        return hdr_max((hdr_histogram_t *) m_histogram);
    return 0;
}

int64_t
tempo_utils::HdrHistogram::getPercentileValue(double percentile) const
{
    if (m_histogram)
        return hdr_value_at_percentile((hdr_histogram_t *) m_histogram, percentile);
    return 0;
}

double
tempo_utils::HdrHistogram::getMean() const
{
    if (m_histogram)
        return hdr_mean((hdr_histogram_t *) m_histogram);
    return 0.0;
}

double
tempo_utils::HdrHistogram::getStddev() const
{
    if (m_histogram)
        return hdr_stddev((hdr_histogram_t *) m_histogram);
    return 0.0;
}

void
tempo_utils::HdrHistogram::reset()
{
    if (m_histogram)
        hdr_reset((hdr_histogram_t *) m_histogram);
}
