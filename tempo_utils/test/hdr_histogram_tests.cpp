
#include <gtest/gtest.h>

#include <tempo_utils/hdr_histogram.h>

TEST(HdrHistogram, TestInitialization)
{
    tempo_utils::HdrHistogram hdr1;
    ASSERT_FALSE(hdr1.isValid());
    tempo_utils::HdrHistogram hdr2(1, 1000000, 3);
    ASSERT_TRUE(hdr2.isValid());
    tempo_utils::HdrHistogram hdr3 = hdr1;
    ASSERT_FALSE(hdr3.isValid());
    tempo_utils::HdrHistogram hdr4 = hdr2;
    ASSERT_TRUE(hdr4.isValid());
}

TEST(HdrHistogram, TestRecordValuesAndGetStatistics)
{
    tempo_utils::HdrHistogram hdr(1, 100, 3);
    hdr.record(10);
    hdr.record(20);
    hdr.record(30);
    hdr.record(40);
    hdr.record(50);
    ASSERT_EQ(hdr.getMinValue(), 10);
    ASSERT_DOUBLE_EQ(hdr.getMean(), 30.0);
    ASSERT_EQ(hdr.getMaxValue(), 50);
    ASSERT_EQ(hdr.getPercentileValue(50.0), 30);
}