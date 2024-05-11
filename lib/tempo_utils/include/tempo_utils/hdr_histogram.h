#ifndef TEMPO_UTILS_HDR_HISTOGRAM_H
#define TEMPO_UTILS_HDR_HISTOGRAM_H

#include "integer_types.h"

namespace tempo_utils {

    /**
     *
     */
    class HdrHistogram {

    public:
        HdrHistogram();
        HdrHistogram(tu_int64 lowest_trackable_value, tu_int64 highest_trackable_value, int significant_figures);
        HdrHistogram(const HdrHistogram &other);
        ~HdrHistogram();

        bool isValid() const;

        bool record(tu_int64 value, tu_int64 count = 1);
        bool recordCorrected(tu_int64 value, tu_int64 expectedInterval, tu_int64 count = 1);

        tu_int64 getMinValue() const;
        tu_int64 getMaxValue() const;
        tu_int64 getPercentileValue(double percentile) const;
        double getMean() const;
        double getStddev() const;

        void reset();

    private:
        void *m_histogram;
    };
}

#endif // TEMPO_UTILS_HDR_HISTOGRAM_H