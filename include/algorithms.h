//
// Created by Qirun Zeng on 15/1/2025.
//

#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "operations.h"
#include "environment.h"
#include <fstream>

namespace alg {
    inline int K;

    inline int T = 200000;

    inline double delta;

    inline double alpha = 0.5; // alpha

    inline void DuelUpdate(std::vector<std::vector<double>>& estimate, std::vector<std::vector<int>>& pull_counter, int arm1, int arm2, double& regret)  {
        if (arm1 == arm2) {
            estimate[arm1][arm2] = (estimate[arm1][arm2] * pull_counter[arm1][arm2] + 1) / (pull_counter[arm1][arm2] + 2);
            pull_counter[arm1][arm2] += 2;
            regret += (env::preference_matrix[0][arm1] + env::preference_matrix[0][arm2] - 1) / 2; // * (1-alpha);
            return ;
        }
        const int won = env::fdb::Duel(arm1, arm2);
        const int lost = arm1 + arm2 - won; // The arm lost
        estimate[won][lost] = (estimate[won][lost] * pull_counter[won][lost] + 1) / (pull_counter[won][lost]+1);
        estimate[lost][won] = estimate[lost][won] * pull_counter[lost][won] / (pull_counter[lost][won]+1);
        // Then update the counter
        pull_counter[arm1][arm2]++;
        pull_counter[arm2][arm1]++;

        // update regret
        regret += (env::preference_matrix[0][arm1] + env::preference_matrix[0][arm2] - 1) / 2; // * (1-alpha);
    }

    inline void RewardUpdate(std::vector<double>&estimate, std::vector<int>& counter, int arm, double& regret) {
        const double reward = env::fdb::PullArm(arm);
        estimate[arm] = (estimate[arm] * counter[arm] + reward) / (counter[arm] + 1);
        counter[arm]++;
        regret += env::expectations[0] - env::expectations[arm];
    }


    inline void StatisticsUpdate(std::vector<std::vector<double>>& duel_estimate, std::vector<std::vector<int>>& duel_counter, std::vector<double>&reward_estimate, std::vector<int>& reward_counter, int arm1, int arm2, int arm, double& reward_regret, double& duel_regret) {
        DuelUpdate(duel_estimate, duel_counter, arm1, arm2, duel_regret);
        RewardUpdate(reward_estimate, reward_counter, arm, reward_regret);
    }

    inline void WarmUpPhase(std::vector<std::vector<double>>& duel_estimate, std::vector<std::vector<int>>& duel_counter, std::vector<double>&reward_estimate, std::vector<int>& reward_counter, double& reward_regret, double& duel_regret, opr::regrets* & regrets, int& time_slot)  {
        // for (int _ = 0; _ < 5; ++_) {
            for (int k = 0; k < K; ++k) {
                for (int j = 0; j < K; ++j) {
                    if (k != j) {
                        StatisticsUpdate(duel_estimate, duel_counter, reward_estimate, reward_counter, k, j, k,reward_regret, duel_regret);

                        regrets[++time_slot] = {reward_regret, duel_regret};
                    }
                }
            }
        // }
    }

    class UCB;

    class DUEL;

    class RewardElim;

    class DuelingElim;

    class elimination;

    class decomposition;
} // namespace alg

#endif // ALGORITHMS_H
