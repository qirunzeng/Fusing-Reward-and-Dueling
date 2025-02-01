//
// Created by Qirun Zeng on 15/1/2025.
//

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <vector>
#include <cmath>

namespace env {
    inline unsigned seed;

    static inline int best_arm = 0;

    inline std::vector expectations =
        //{0.9, 0.84, 0.78, 0.72, 0.66};// , 0.60, 0.54, 0.48};
    {
        0.86, 0.80, 0.75, 0.70,
        0.65, 0.60, 0.55, 0.50,
        0.45, 0.40, 0.35, 0.30,
        0.25, 0.20, 0.15, 0.10
    };
    //
    // {
    //     0.86, 0.80, 0.76, 0.72,
    //     0.68, 0.64, 0.60, 0.56,
    //     0.52, 0.48, 0.44, 0.40,
    //     0.36, 0.32, 0.28, 0.24
    // };
    // {0.96, 0.9, 0.84, 0.78, 0.72, 0.66, 0.60, 0.54, 0.48, 0.42, 0.36, 0.30, 0.24, 0.18, 0.12, 0.06};
    inline std::vector<std::vector<double>> preference_matrix =
    // {
    //     {0.50, 0.53, 0.56, 0.59, 0.62}, //, 0.65, 0.68, 0.71},
    //     {0.47, 0.50, 0.53, 0.56, 0.59}, //, 0.62, 0.65, 0.62},
    //     {0.44, 0.47, 0.50, 0.53, 0.56}, //, 0.59, 0.62, 0.65},
    //     {0.41, 0.44, 0.47, 0.50, 0.53}, // 0.56, 0.59, 0.62},
    //     {0.38, 0.41, 0.44, 0.47, 0.50} //, 0.53, 0.56, 0.59},
    //     // {0.35, 0.38, 0.41, 0.44, 0.47, 0.50, 0.53, 0.56},
    //     // {0.32, 0.35, 0.38, 0.41, 0.44, 0.47, 0.50, 0.53},
    //     // {0.29, 0.32, 0.35, 0.38, 0.41, 0.44, 0.47, 0.50}
    // };
    {
        {0.50, 0.54, 0.57, 0.60, 0.63, 0.65, 0.69, 0.71, 0.73, 0.76, 0.78, 0.82, 0.86, 0.91, 0.95, 0.98},
        {0.46, 0.50, 0.54, 0.58, 0.61, 0.64, 0.67, 0.70, 0.74, 0.76, 0.79, 0.81, 0.84, 0.87, 0.89, 0.92},
        {0.43, 0.46, 0.50, 0.54, 0.58, 0.60, 0.63, 0.66, 0.69, 0.72, 0.76, 0.79, 0.83, 0.85, 0.88, 0.91},
        {0.40, 0.42, 0.46, 0.50, 0.54, 0.58, 0.61, 0.64, 0.66, 0.69, 0.72, 0.76, 0.79, 0.82, 0.85, 0.88},
        {0.37, 0.39, 0.42, 0.46, 0.50, 0.54, 0.56, 0.59, 0.63, 0.66, 0.69, 0.72, 0.76, 0.78, 0.82, 0.86},
        {0.35, 0.36, 0.40, 0.42, 0.46, 0.50, 0.54, 0.57, 0.59, 0.63, 0.67, 0.70, 0.73, 0.76, 0.79, 0.82},
        {0.31, 0.33, 0.37, 0.39, 0.44, 0.46, 0.50, 0.54, 0.58, 0.61, 0.64, 0.68, 0.71, 0.72, 0.75, 0.79},
        {0.29, 0.30, 0.34, 0.36, 0.41, 0.43, 0.46, 0.50, 0.54, 0.57, 0.59, 0.62, 0.65, 0.68, 0.72, 0.76},
        {0.27, 0.26, 0.31, 0.34, 0.37, 0.41, 0.42, 0.46, 0.50, 0.54, 0.58, 0.61, 0.63, 0.66, 0.69, 0.73},
        {0.24, 0.24, 0.28, 0.31, 0.34, 0.37, 0.39, 0.43, 0.46, 0.50, 0.54, 0.56, 0.59, 0.62, 0.66, 0.69},
        {0.22, 0.21, 0.24, 0.28, 0.31, 0.33, 0.36, 0.41, 0.42, 0.46, 0.50, 0.54, 0.56, 0.58, 0.61, 0.65},
        {0.18, 0.19, 0.21, 0.24, 0.28, 0.30, 0.32, 0.38, 0.39, 0.44, 0.46, 0.50, 0.54, 0.57, 0.58, 0.62},
        {0.14, 0.16, 0.17, 0.21, 0.24, 0.27, 0.29, 0.35, 0.37, 0.41, 0.44, 0.46, 0.50, 0.54, 0.56, 0.59},
        {0.09, 0.13, 0.15, 0.18, 0.22, 0.24, 0.28, 0.32, 0.34, 0.38, 0.42, 0.43, 0.46, 0.50, 0.54, 0.56},
        {0.05, 0.11, 0.12, 0.15, 0.18, 0.21, 0.25, 0.28, 0.31, 0.34, 0.39, 0.42, 0.44, 0.46, 0.50, 0.54},
        {0.02, 0.08, 0.09, 0.12, 0.14, 0.18, 0.21, 0.24, 0.27, 0.31, 0.35, 0.38, 0.41, 0.44, 0.46, 0.50}
    };

    namespace fdb { // feedback
        int Duel(int arm1, int arm2);

        int PullArm(int arm);
    }
}

inline int env::fdb::Duel(const int arm1, const int arm2) {
    return rand() % RAND_MAX <= preference_matrix[arm1][arm2] * RAND_MAX ? arm1 : arm2; // with prob exp_m[a1][a2] return arm1, else arm2
}

inline int env::fdb::PullArm(const int arm) {
    return static_cast<double>(rand() % RAND_MAX) / RAND_MAX <= expectations[arm];
}

#endif //ENVIRONMENT_H
