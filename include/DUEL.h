//
// Created by Qirun Zeng on 23/1/2025.
//

#ifndef DUEL_H
#define DUEL_H

#include "algorithms.h"

#include <iostream>
#include <numeric>
#include <vector>

class alg::DUEL {
    int time_slot;
    std::vector<std::vector<int>> counter; // M
    std::vector<std::vector<double>> estimate;  // \nu
    std::vector<int> candidates; // \mathcal C
    double regret;

    void WarmUp(double*& regrets) {
        for (int k = 0; k < K; ++k) {
            for (int j = 0; j < K; ++j) {
                if (k != j) {
                    DuelUpdate(estimate, counter, k, j, regret);
                    regrets[++time_slot] = regret;
                }
            }
        }
    }

    void Elimination() {
        std::vector<int> new_candidates;
        for (const int k : candidates) {
            bool keep = true;
            for (const auto l : candidates) {
                if (k != l && estimate[k][l] + opr::ConfidenceRadius(K, time_slot, delta, counter[k][l]) < 0.5) {
                    keep = false;
                    break;
                }
            }
            if (keep) {
                new_candidates.push_back(k);
            }
        }
        candidates = new_candidates;
    }

    void DecisionMaking()  {
        if (const int size = static_cast<int>(candidates.size()); size > 1) {
            int duel_arm1 = 0, duel_arm2 = 0;
            for (int i = 0, min_duel_count = std::numeric_limits<int>::max(); i < size; ++i) {
                // (k_{t, 1}, k_{t, 2}) \gets arg min_{(k, k^{'}) \in \mathcal C \times \mathcal C} M_{k, k^{'}, t}
                const int k = candidates[i]; // Only choose from candidates
                for (int j = i + 1; j < size; ++j) {
                    if (const int l = candidates[j]; counter[k][l] < min_duel_count) {
                        min_duel_count = counter[duel_arm1 = k][duel_arm2 = l];
                    }
                }
            }
            DuelUpdate(estimate, counter, duel_arm1, duel_arm2, regret);
            Elimination();
        } else { // size = 1
            DuelUpdate(estimate, counter, candidates[0], candidates[0], regret);
        }
    }

public:
    DUEL() {
        time_slot = 0;
        regret = 0;
        counter.resize(K, std::vector<int>(K));
        estimate.resize(K, std::vector<double>(K));
        candidates.resize(K);
        std::iota(candidates.begin(), candidates.end(), 0);
    }

    void Run(double *&regrets) {
        WarmUp(regrets);
        while (time_slot++ < T) {
            DecisionMaking();
            regrets[time_slot] = regret;
        }
        std::cout << "DUEL: ";
        for (const auto k : candidates) {
            std::cout << k << " ";
        }
        std::cout << std::endl;
    }
};

#endif //DUEL_H
