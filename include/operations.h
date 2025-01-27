//
// Created by Qirun Zeng on 15/1/2025.
//

#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <cmath>
#include <vector>

namespace opr {

    struct regrets {
        double reward_r;
        double duel_r;
    };

    inline double ConfidenceRadius(int K, int t, double delta, int count);

    inline double KLDiv(double p, double q);

    inline double DistinguishBound(int t, int K, double (*f)(double x));

    inline double f1(double x);

    inline double square(double x);
}

double opr::ConfidenceRadius(const int K, const int t, const double delta, const int count) {
    return sqrt(2 * log(K * t / delta)) / count;
}

double opr::KLDiv(const double p, const double q) {
    if (p == q) {
        return 0;
    }
    if (p + q == 0 && p * q == 1) {
        return std::numeric_limits<double>::max();
    }
    if (p == 0) {
        return -log(1-q);
    }
    if (p == 1) {
        return -log(q);
    }
    if (q == 0) {
        return -log(1-p);
    }
    if (q == 1) {
        return -log(p);
    }
    return p * log (p/q) + (1-p) * log((1-p) / (1-q));
}

double opr::DistinguishBound(const int t, const int K, double (*f)(double x)) {
    return log(t) + f(static_cast<double>(K));
}

double opr::f1(const double x) {
    return 0.3 * pow(x, 1.01);
}

double opr::square(const double x) {
    return 0.05 * pow(x, 1.01);
}


#endif //OPERATIONS_H
