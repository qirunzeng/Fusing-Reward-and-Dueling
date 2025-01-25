//
// Created by Qirun Zeng on 23/1/2025.
//

#ifndef UCB_H
#define UCB_H

#include "algorithms.h"

class alg::UCB {
    int time_slot;
    double regret;
    std::vector<double> estimate;
    std::vector<int> counter;
    std::vector<int> candidates;

public:
    void WarmUp(double *&regrets) {
        for (int k = 0; k < K; ++k) {
            for (int j = 1; j < K; ++j) {
                RewardUpdate(estimate, counter, k, regret);
                regrets[++time_slot] = regret;
            }
        }
    }

    void DecisionMaking() {
        if (const int size = static_cast<int>(candidates.size()); size > 1) {
            const int reward_arm = *min_element(candidates.begin(), candidates.end(), [&](const int a, const int b) {
                return counter[a] < counter[b];
            });
            RewardUpdate(estimate, counter, reward_arm, regret);
            Elimination();
        } else {
            RewardUpdate(estimate, counter, candidates[0], regret);
        }
    }

    void Elimination() {
        std::vector<int> new_candidates;
        const int max_reward_can = *std::max_element(candidates.begin(), candidates.end(), [&](const int a, const int b) {
            return estimate[a] < estimate[b];
        });
        const double confidence_lower_bound = estimate[max_reward_can] - opr::ConfidenceRadius(K, time_slot, delta, counter[max_reward_can]);
        for (const int k : candidates) {
            if (estimate[k] + opr::ConfidenceRadius(K, time_slot, delta, counter[k]) > confidence_lower_bound) {
                new_candidates.push_back(k);
            }
        }
        candidates = new_candidates;
    }

    void Run(double *&regrets) {
        WarmUp(regrets);
        while (time_slot++ < T) {
            DecisionMaking();
            Elimination();
            regrets[time_slot] = regret;
        }
        std::cout << "UCB:  ";
        for (const int k : candidates) {
            std::cout << k << " ";
        }
        std::cout << std::endl;
    }

    UCB() : time_slot(0), regret(0) {
        estimate.resize(K);
        counter.resize(K);
        candidates.resize(K);
        std::iota(candidates.begin(), candidates.end(), 0);
    }
};


#endif //UCB_H
