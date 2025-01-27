//
// Created by Qirun Zeng on 23/1/2025.
//

#ifndef ELIMINATION_H
#define ELIMINATION_H

#include "algorithms.h"
#include <iostream>
#include <numeric>

class alg::elimination {
private:
    int time_slot;
    std::vector<std::vector<int>> duel_counter; // M
    std::vector<std::vector<double>> duel_estimate;  // \nu
    std::vector<int> reward_counter; // N
    std::vector<double> reward_estimate;  // \mu
    std::vector<int> candidates; // \mathcal C

    void DecisionMaking()  {
        if (const int size = static_cast<int>(candidates.size()); size > 1) {
            int duel_arm1 = 0, duel_arm2 = 0;
            for (int i = 0, min_duel_count = std::numeric_limits<int>::max(); i < size; ++i) {
                // (k_{t, 1}, k_{t, 2}) \gets arg min_{(k, k^{'}) \in \mathcal C \times \mathcal C} M_{k, k^{'}, t}
                const int k = candidates[i]; // Only choose from candidates
                for (int j = i + 1; j < size; ++j) {
                    if (const int l = candidates[j]; duel_counter[k][l] < min_duel_count) {
                        min_duel_count = duel_counter[duel_arm1 = k][duel_arm2 = l];
                    }
                }
            }
            const int reward_arm = *std::min_element(candidates.begin(), candidates.end(), [&](const int a, const int b) {
                return reward_counter[a] < reward_counter[b];
            });
            StatisticsUpdate(duel_estimate, duel_counter, reward_estimate, reward_counter, duel_arm1, duel_arm2, reward_arm, reward_regret, duel_regret);
            EliminationPhase();
        } else {
            StatisticsUpdate(duel_estimate, duel_counter, reward_estimate, reward_counter, candidates[0], candidates[0], candidates[0], reward_regret, duel_regret);
        }
    }

    void EliminationPhase()  {
        std::vector<int> new_candidates;
        const int max_reward_can = *std::max_element(candidates.begin(), candidates.end(), [&](const int a, const int b) {
            return reward_estimate[a] < reward_estimate[b];
        });
        const double confidence_lower_bound = reward_estimate[max_reward_can] - opr::ConfidenceRadius(K, time_slot, delta, reward_counter[max_reward_can]);
        for (const int k : candidates) {
            bool keep = true;
            for (const auto l : candidates) {
                if (k != l && duel_estimate[k][l] + opr::ConfidenceRadius(K, time_slot, delta, duel_counter[k][l]) < 0.5) {
                    keep = false;
                    break;
                }
            }
            if (reward_estimate[k] + opr::ConfidenceRadius(K, time_slot, delta, reward_counter[k]) <= confidence_lower_bound) {
                keep = false;
            }
            if (keep) {
                new_candidates.push_back(k);
            }
        }
        candidates = new_candidates;
    }


public:
    double reward_regret;

    double duel_regret;

    elimination() : time_slot(0), reward_regret(0.0), duel_regret(0.0) {
        duel_estimate.resize(K, std::vector<double>(K, 0));
        duel_counter.resize(K, std::vector(K, 0));
        reward_counter.resize(K, 0);
        reward_estimate.resize(K, 0);
        candidates.resize(K);
        std::iota(candidates.begin(), candidates.end(), 0); // from 0 to K-1
    }

    void Run(opr::regrets *&regrets){
        WarmUpPhase(duel_estimate, duel_counter, reward_estimate, reward_counter, reward_regret, duel_regret, regrets, time_slot);
        while (time_slot++ < T) {
            DecisionMaking();
            if (time_slot % sep == 0) {
                regrets[time_slot / sep] = {reward_regret, duel_regret};
            }
        }
        std::cout << "Elimination Fusion: ";
        for (const int k : candidates) {
            std::cout << k << " ";
        }
        std::cout << std::endl;
    }

};


#endif //ELIMINATION_H
