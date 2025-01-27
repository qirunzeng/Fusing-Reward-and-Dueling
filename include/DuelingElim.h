//
// Created by Qirun Zeng on 26/1/2025.
//

#ifndef DUELING_ELIM_H
#define DUELING_ELIM_H

#include "algorithms.h"
#include <iostream>
#include <numeric>

class alg::DuelingElim {
private:
    int time_slot;
    std::vector<std::vector<int>> duel_counter; // M
    std::vector<std::vector<double>> duel_estimate;  // \nu
    std::vector<int> candidates2;
    double reward_regret, duel_regret;


    void WarmUp(opr::regrets *&regrets) {
        for (int k = 0; k < K; ++k) {
            for (int j = 0; j < K; ++j) {
                if (k != j) {
                    DuelUpdate(duel_estimate, duel_counter, k, j, duel_regret);
                    const int c = candidates2[rand() % candidates2.size()];
                    reward_regret += (env::expectations[0] - env::expectations[c]);
                    time_slot++;
                    if (time_slot % sep == 0) {
                        regrets[time_slot / sep] = {reward_regret, duel_regret};
                    }
                }
            }
        }
    }

    void DecisionMaking()  {
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
            const int c3 = candidates2[rand() % candidates2.size()];
            reward_regret += (env::expectations[0] - env::expectations[c3]);
            EliminationPhase2();
        } else {
            DuelUpdate(duel_estimate, duel_counter, candidates2[0], candidates2[0], duel_regret);
            duel_regret += (env::expectations[0] - env::expectations[candidates2[0]]);
        }
    }

    void EliminationPhase2()  {
        std::vector<int> new_candidates;
        for (const int k : candidates2) {
            bool keep = true;
            for (const int l : candidates2) {
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

    DuelingElim() : time_slot(0), reward_regret(0.0), duel_regret(0.0) {
        duel_estimate.resize(K, std::vector<double>(K, 0));
        duel_counter.resize(K, std::vector(K, 0));
        candidates2.resize(K);
        std::iota(candidates2.begin(), candidates2.end(), 0); // from 0 to K-1
    }

    void Run(opr::regrets *&regrets){
        WarmUp(regrets);
        while (time_slot++ < T) {
            DecisionMaking();
            if (time_slot % sep == 0) {
                regrets[time_slot / sep] = {reward_regret, duel_regret};
            }
        }
        std::cout << "Dueling Elimination: ";
        for (const int k : candidates2) {
            std::cout << k << " ";
        }
        std::cout << std::endl;
    }

};
#endif //DUELING_ELIM_H
