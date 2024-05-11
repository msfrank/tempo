#ifndef FP_COMPENSATION_H
#define FP_COMPENSATION_H

#include <cstdint>

double kahan_sum(double const values[], int count);
double kahan_add(double dbl1, double dbl2);
double kahan_sub(double dbl1, double dbl2);

double neumaier_sum(double const values[], int count);
double neumaier_add(double dbl1, double dbl2);
double neumaier_sub(double dbl1, double dbl2);

#endif // FP_COMPENSATION_H
