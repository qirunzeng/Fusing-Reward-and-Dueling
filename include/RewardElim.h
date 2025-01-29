//
// Created by Qirun Zeng on 25/1/2025.
//

#ifndef REWARD_ELIM_H
#define REWARD_ELIM_H


#include "algorithms.h"
#include <iostream>
#include <numeric>

class alg::RewardElim {
private:
    int time_slot;
    std::vector<int> counter; // N
    std::vector<double> estimate;  // \mu
    std::vector<int> candidates;
    double reward_regret, duel_regret;


    void WarmUp(opr::regrets *&regrets) {
        // for (int _ = 0; _ < 5; ++_) {
            for (int k = 0; k < K; ++k) {
                for (int j = 0; j < K; ++j) {
                    if (k != j) {
                        // StatisticsUpdate(duel_estimate, duel_counter, estimate, counter, k, j, k,reward_regret, duel_regret);
                        RewardUpdate(estimate, counter, k, reward_regret);
                        const int c1 = candidates[rand() % candidates.size()], c2 = candidates[rand() % candidates.size()];
                        duel_regret += (env::preference_matrix[0][c1] + env::preference_matrix[0][c2] - 1) / 2;
                        regrets[++time_slot] = {reward_regret, duel_regret};
                    }
                }
            }
        // }
    }

    void DecisionMaking()  {
        if (const int size1 = static_cast<int>(candidates.size()); size1 > 1) {
            const int reward_arm = *std::min_element(candidates.begin(), candidates.end(), [&](const int a, const int b) {
                return counter[a] < counter[b];
            });
            RewardUpdate(estimate, counter, reward_arm, reward_regret);
            const int c1 = candidates[rand() % candidates.size()], c2 = candidates[rand() % candidates.size()];
            duel_regret += (env::preference_matrix[0][c1] + env::preference_matrix[0][c2] - 1) / 2;
            EliminationPhase1();
        } else {
            RewardUpdate(estimate, counter, candidates[0], reward_regret);
            duel_regret += (env::preference_matrix[0][candidates[0]] + env::preference_matrix[0][candidates[0]] - 1) / 2;
        }
    }

    void EliminationPhase1()  {
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

public:

    RewardElim() : time_slot(0), reward_regret(0.0), duel_regret(0.0) {
        counter.resize(K, 0);
        estimate.resize(K, 0);
        candidates.resize(K);
        std::iota(candidates.begin(), candidates.end(), 0); // from 0 to K-1
    }

    void Run(opr::regrets *&regrets){
        WarmUp(regrets);
        while (time_slot++ < T) {
            DecisionMaking();
            regrets[time_slot] = {reward_regret, duel_regret};
        }
        std::cout << "Reward Elimination: ";
        for (const int k : candidates) {
            std::cout << k << " ";
        }
        std::cout << std::endl;
    }

};


#endif //REWARD_ELIM_H
