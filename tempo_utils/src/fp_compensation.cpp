#include <array>
#include <numeric>
#include <cmath>

#include <tempo_utils/fp_compensation.h>

double kahan_sum(double const values[], int count)
{
    double sum = 0.0;
    double c = 0.0;
    for (int i = 0; i < count; i++) {
        double y = values[i] - c;
        double t = sum + y;
        c = (t - sum) - y;
        sum = t;
    }
    return sum;
}

double kahan_add(double dbl1, double dbl2)
{
    double values[2] = {dbl1, dbl2};
    return kahan_sum(values, 2);
}

double kahan_sub(double dbl1, double dbl2)
{
    double values[2] = {dbl1, dbl2};
    return kahan_sum(values, 2);
}

double neumaier_sum(double const values[], int count)
{
    if (count == 0)
        return 0;
    if (count == 1)
        return values[0];
    double sum = values[0];
    double c = 0.0;                          // A running compensation for lost low-order bits.
    for (int i = 1; i < count; i++) {
        double t = sum + values[i];
        if (std::abs(sum) >= std::abs(values[i]))
            c += (sum - t) + values[i];     // If sum is bigger, low-order digits of input[i] are lost.
        else
            c += (values[i] - t) + sum;     // Else low-order digits of sum are lost
        sum = t;
    }
    return sum + c;                         // Correction only applied once in the very end
}

double neumaier_add(double dbl1, double dbl2)
{
    double values[2] = {dbl1, dbl2};
    return neumaier_sum(values, 2);
}

double neumaier_sub(double dbl1, double dbl2)
{
    double values[2] = {dbl1, -dbl2};
    return neumaier_sum(values, 2);
}
