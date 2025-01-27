//
// Created by Qirun Zeng on 23/1/2025.
//

#ifndef DECOMPOSITION_H
#define DECOMPOSITION_H

#include <iostream>

#include "algorithms.h"
#include <unordered_set>
#include <vector>
#include <numeric>

class alg::decomposition {
private:
    int best_reward_arm, best_duel_arm;
    int to_pull, to_duel1, to_duel2;
    int time_slot;
    std::vector<std::vector<int>> duel_counter;   // M
    std::vector<std::vector<double>> duel_estimate;    // \nu
    std::vector<int> reward_counter;    // N
    std::vector<double> reward_estimate; // \mu
    std::vector<int> candidates; // \mathcal C, exploration arm set
    std::vector<int> candidates_trace;    // \mathcal E
    //! Arms indistinguishable by reward should then be performed by dueling, vice versa.
    std::vector<int> duel_indistinguishable; // \hat K^{(R)}_t
    std::vector<int> reward_indistinguishable; // \hat K^{(D)}_t
    std::vector<double> information_reward, information_duel;

    void FindNewArmToExplore() {
        std::unordered_set<int> candidate_trace_new;
        for (const auto k : candidates_trace) {
            candidate_trace_new.insert(k);
        }
        std::unordered_set<int> candidates_set;
        for (const auto k : candidates) {
            candidates_set.insert(k);
        }
        const double bound_t = opr::DistinguishBound(time_slot, K, opr::square);
        for (const auto k : duel_indistinguishable) {
            if (candidates_set.find(k) == candidates_set.end() && information_reward[k] <= bound_t) {
                candidate_trace_new.insert(k);
            }
        }
        for (const auto k : reward_indistinguishable) {
            if (candidates_set.find(k) == candidates_set.end() && information_duel[k] - information_duel[best_duel_arm] <= bound_t) {
                candidate_trace_new.insert(k);
            }
        }
        candidates_trace.clear();
        for (const auto k : candidate_trace_new) {
            candidates_trace.push_back(k);
        }
        sort(candidates_trace.begin(), candidates_trace.end());
    }

    void DecisionMaking() {
        const int explore_arm = candidates.back(); // fixed order -> The last one
        candidates.pop_back(); // remove it
        if (rand() % RAND_MAX > RAND_MAX * pow(alpha, 2) / (pow(alpha, 2) + pow(1-alpha, 2)) ) {
            // reward explore, duel exploit
            to_pull = explore_arm;
            to_duel1 = to_duel2 = best_reward_arm;
        } else { // reward exploit, duel explore
            int duel_arm = -1;
            std::unordered_set<int> potential_better; // \mathcal O
            for (const auto k : reward_indistinguishable) {
                if (k != explore_arm && duel_estimate[explore_arm][k] <= 0.5) {
                    potential_better.insert(k);
                }
            }
            if (potential_better.empty() || potential_better.find(best_duel_arm) != potential_better.end()) {
                duel_arm = best_duel_arm;
            } else {
                double min_dueling_estimate = std::numeric_limits<double>::max();
                for (const auto k : reward_indistinguishable) {
                    if (k == explore_arm) {
                        continue;
                    }
                    if (duel_estimate[explore_arm][k] < min_dueling_estimate) {
                        min_dueling_estimate = duel_estimate[explore_arm][k];
                        duel_arm = k;
                    }
                }
            }
            to_pull = best_duel_arm;
            to_duel1 = explore_arm;
            to_duel2 = duel_arm;
        }
    }


    void DecompositionArmSetUpdate() {
        duel_indistinguishable.clear();
        reward_indistinguishable.clear();
        const double bound_t = opr::DistinguishBound(time_slot, K, opr::square);
        for (int k = 0; k < K; ++k) {
            if (information_reward[k] <= bound_t) {
                reward_indistinguishable.push_back(k);
            }
        }
        best_duel_arm = *std::min_element(reward_indistinguishable.begin(), reward_indistinguishable.end(), [&](const auto k1, const auto k2) {
            return information_duel[k1] < information_duel[k2];
        });
        const double info_best_duel_arm = information_duel[best_duel_arm];
        for (int k = 0; k < K; ++k) {
            if (information_duel[k] - info_best_duel_arm <= bound_t) {
                duel_indistinguishable.push_back(k);
            }
        }

        best_reward_arm = *std::max_element(duel_indistinguishable.begin(), duel_indistinguishable.end(), [&](const auto k1, const auto k2) {
            return reward_estimate[k1] < reward_estimate[k2];
        });
    }

    void InformationUpdate() {
        const double max_reward_estimate = reward_estimate[*std::max_element(duel_indistinguishable.begin(), duel_indistinguishable.end(), [&](const auto k1, const auto k2) {
            return reward_estimate[k1] < reward_estimate[k2];
        })];
        for (int k = 0; k < K; ++k) {
            information_reward[k] = reward_counter[k] * opr::KLDiv(reward_estimate[k], max_reward_estimate);
        }
        for (int k = 0; k < K; ++k) {
            information_duel[k] = 0;
            for (const auto l : reward_indistinguishable) {
                if (duel_estimate[k][l] < 0.5) {
                    information_duel[k] += duel_counter[k][l] * opr::KLDiv(duel_estimate[k][l], 0.5);
                }
            }
        }
    }

public:
    double reward_regret;

    double duel_regret;

    decomposition() : best_reward_arm(-1), best_duel_arm(-1), to_pull(0), to_duel1(0), to_duel2(0), time_slot(0), reward_regret(0.0), duel_regret(0.0) {
        duel_estimate.resize(K, std::vector<double>(K, 0));
        duel_counter.resize(K, std::vector(K, 0));
        reward_counter.resize(K, 0);
        reward_estimate.resize(K, 0);
        candidates.resize(K);
        std::iota(candidates.begin(), candidates.end(), 0);
        duel_indistinguishable.resize(K);
        std::iota(duel_indistinguishable.begin(), duel_indistinguishable.end(), 0);
        reward_indistinguishable.resize(K);
        std::iota(reward_indistinguishable.begin(), reward_indistinguishable.end(), 0);
        information_reward.resize(K, 0);
        information_duel.resize(K, 0);
    }

    void Run(opr::regrets* &regrets) {
        WarmUpPhase(duel_estimate, duel_counter, reward_estimate, reward_counter, reward_regret, duel_regret, regrets, time_slot);
        while (time_slot++ < T) {
            DecompositionArmSetUpdate();
            DecisionMaking();
            // Exploration Arm Set Construction
            FindNewArmToExplore();
            if (candidates.empty()) {
                candidates = candidates_trace;
                candidates_trace.clear();
            }
            StatisticsUpdate(duel_estimate, duel_counter, reward_estimate, reward_counter, to_duel1, to_duel2, to_pull, reward_regret, duel_regret);
            InformationUpdate();
            if (time_slot % sep == 0) {
                regrets[time_slot / sep] = { reward_regret, duel_regret };
            }
        }
    }

    void RunAlpha(double& reward_regret_total, double& duel_regret_total) {
        auto *regrets = new opr::regrets[K * K]; // Unuseful var, just used as a param transferred to WarmUpPhase.
        WarmUpPhase(duel_estimate, duel_counter, reward_estimate, reward_counter, reward_regret, duel_regret, regrets, time_slot);
        while (time_slot++ < T) {
            DecompositionArmSetUpdate();
            DecisionMaking();
            // Exploration Arm Set Construction
            FindNewArmToExplore();
            if (candidates.empty()) {
                candidates = candidates_trace;
                candidates_trace.clear();
            }
            StatisticsUpdate(duel_estimate, duel_counter, reward_estimate, reward_counter, to_duel1, to_duel2, to_pull, reward_regret, duel_regret);
            InformationUpdate();
        }
        delete [] regrets;
        reward_regret_total = reward_regret;
        duel_regret_total   =   duel_regret;
    }
};


#endif //DECOMPOSITION_H
