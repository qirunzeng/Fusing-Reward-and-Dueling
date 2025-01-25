//
// Created by Qirun Zeng on 25/1/2025.
//

#ifndef SEPARATION_H
#define SEPARATION_H


#include "algorithms.h"
#include <iostream>
#include <numeric>

class alg::separation {
private:
    int time_slot;
    std::vector<std::vector<int>> duel_counter; // M
    std::vector<std::vector<double>> duel_estimate;  // \nu
    std::vector<int> reward_counter; // N
    std::vector<double> reward_estimate;  // \mu
    std::vector<int> candidates1, candidates2;
    double reward_regret, duel_regret;

    void DecisionMaking()  {
        if (const int size1 = static_cast<int>(candidates1.size()); size1 > 1) {
            const int reward_arm = *std::min_element(candidates1.begin(), candidates1.end(), [&](const int a, const int b) {
                return reward_counter[a] < reward_counter[b];
            });
            RewardUpdate(reward_estimate, reward_counter, reward_arm, reward_regret);
            EliminationPhase1();
        } else {
            RewardUpdate(reward_estimate, reward_counter, candidates1[0], reward_regret);
        }

        if (const int size2 = static_cast<int>(candidates2.size()); size2 > 1) {
            int duel_arm1 = 0, duel_arm2 = 0;
            for (int i = 0, min_duel_count = std::numeric_limits<int>::max(); i < size2; ++i) {
                // (k_{t, 1}, k_{t, 2}) \gets arg min_{(k, k^{'}) \in \mathcal C \times \mathcal C} M_{k, k^{'}, t}
                const int k = candidates2[i]; // Only choose from candidates
                for (int j = i + 1; j < size2; ++j) {
                    if (const int l = candidates2[j]; duel_counter[k][l] < min_duel_count) {
                        min_duel_count = duel_counter[duel_arm1 = k][duel_arm2 = l];
                    }
                }
            }
            DuelUpdate(duel_estimate, duel_counter, duel_arm1, duel_arm2, duel_regret);
            EliminationPhase2();
        } else {
            DuelUpdate(duel_estimate, duel_counter, candidates2[0], candidates2[0], duel_regret);
        }
    }

    void EliminationPhase1()  {
        std::vector<int> new_candidates;
        const int max_reward_can = *std::max_element(candidates1.begin(), candidates1.end(), [&](const int a, const int b) {
            return reward_estimate[a] < reward_estimate[b];
        });
        const double confidence_lower_bound = reward_estimate[max_reward_can] - opr::ConfidenceRadius(K, time_slot, delta, reward_counter[max_reward_can]);
        for (const int k : candidates1) {
            if (reward_estimate[k] + opr::ConfidenceRadius(K, time_slot, delta, reward_counter[k]) > confidence_lower_bound) {
                new_candidates.push_back(k);
            }
        }
        candidates1 = new_candidates;
    }

    void EliminationPhase2()  {
        std::vector<int> new_candidates;
        for (const int k : candidates2) {
            bool keep = true;
            for (const auto l : candidates2) {
                if (k != l && duel_estimate[k][l] + opr::ConfidenceRadius(K, time_slot, delta, duel_counter[k][l]) < 0.5) {
                    keep = false;
                    break;
                }
            }
            if (keep) {
                new_candidates.push_back(k);
            }
        }
        candidates2 = new_candidates;
    }


public:

    separation() : time_slot(0), reward_regret(0.0), duel_regret(0.0) {
        duel_estimate.resize(K, std::vector<double>(K, 0));
        duel_counter.resize(K, std::vector(K, 0));
        reward_counter.resize(K, 0);
        reward_estimate.resize(K, 0);
        candidates1.resize(K);
        std::iota(candidates1.begin(), candidates1.end(), 0); // from 0 to K-1
        candidates2.resize(K);
        std::iota(candidates2.begin(), candidates2.end(), 0); // from 0 to K-1
    }

    void Run(opr::regrets *&regrets){
        WarmUpPhase(duel_estimate, duel_counter, reward_estimate, reward_counter, reward_regret, duel_regret, regrets, time_slot);
        while (time_slot++ < T) {
            DecisionMaking();
            regrets[time_slot] = {reward_regret, duel_regret};
        }
        std::cout << "Can_r: ";
        for (const int k : candidates1) {
            std::cout << k << " ";
        }
        std::cout << "||| Can_d: ";
        for (const int k : candidates2) {
            std::cout << k << " ";
        }
        std::cout << std::endl;
    }

};


#endif //SEPARATION_H
