//
// Created by Qirun Zeng on 2025/1/10.
//


// #include "algorithms.h"
#include "decomposition.h"
#include "elimination.h"
// #include "DUEL.h"
// #include "UCB.h"
#include "environment.h"
#include "operations.h"
#include "RewardElim.h"
#include "DuelingElim.h"
#include <iostream>
#include <vector>


#define ROUND 100
const std::string path = "../out.nosync/raw.txt";

#define REGRET_DATA 14

void output_data(std::ofstream& outfile, const opr::regrets *alg1_r, const opr::regrets *alg2_regrets1, const opr::regrets *alg2_regrets2, const opr::regrets *alg2_r, const opr::regrets *reward_elim_r, const opr::regrets *dueling_elim_r, const opr::regrets *alg2_r01, const opr::regrets *alg2_r09) {
    outfile << "---> results:" << std::endl;
    outfile << "# T            # ..." << std::endl;
    for (int i = 0; i <= REGRET_DATA; i++) {
        outfile << 0 << " ";
    }
    outfile << std::endl;
    for (int i = 1; i <= alg::T; ++i) {
        outfile << std::left << std::setw(14) << i << " ";
        outfile<<std::fixed<<std::setprecision(2)<<std::left<<std::setw(8)<<reward_elim_r[i].reward_r << " "; // Elim No Fusion
        outfile<<std::fixed<<std::setprecision(2)<<std::left<<std::setw(8)<<dueling_elim_r[i].duel_r  << " "; // Elim No Fusion
        outfile<<std::fixed<<std::setprecision(2)<<std::left<<std::setw(8)<<reward_elim_r[i].reward_r*0.5 + reward_elim_r[i].duel_r*0.5 <<" ";//ElimFusion-Reward
        outfile<<std::fixed<<std::setprecision(2)<<std::left<<std::setw(8)<<dueling_elim_r[i].reward_r*0.5+dueling_elim_r[i].duel_r*0.5 <<" ";//ElimFusion-Dueling
        outfile<<std::fixed<<std::setprecision(2)<<std::left<<std::setw(8)<<reward_elim_r[i].reward_r*0.5 +dueling_elim_r[i].duel_r*0.5 <<" ";//ElimNoFusion
        outfile<<std::fixed<<std::setprecision(2)<<std::left<<std::setw(8)<<alg1_r[i].reward_r * 0.5 + alg1_r[i].duel_r * 0.5 << " ";         //ElimFusion
        outfile<<std::fixed<<std::setprecision(2)<<std::left<<std::setw(8)<<alg2_regrets1[i].reward_r << " "; // MEDNoFusion
        outfile<<std::fixed<<std::setprecision(2)<<std::left<<std::setw(8)<<alg2_regrets2[i].duel_r   << " "; // MEDNoFusion
        outfile<<std::fixed<<std::setprecision(2)<<std::left<<std::setw(8)<<alg2_regrets1[i].reward_r*0.5 + alg2_regrets1[i].duel_r*0.5 << " "; // DMED (Reward)
        outfile<<std::fixed<<std::setprecision(2)<<std::left<<std::setw(8)<<alg2_regrets2[i].reward_r*0.5 + alg2_regrets2[i].duel_r*0.5 << " "; // RMED (Dueling)
        outfile<<std::fixed<<std::setprecision(2)<<std::left<<std::setw(8)<<alg2_regrets1[i].reward_r*0.5 + alg2_regrets2[i].duel_r*0.5 << " "; // MEDNoFusion
        outfile<<std::fixed<<std::setprecision(2)<<std::left<<std::setw(8)<<alg2_r[i].reward_r*0.5 + alg2_r[i].duel_r*0.5 << " ";   // DecoFusion, 0.5
        outfile<<std::fixed<<std::setprecision(2)<<std::left<<std::setw(8)<<alg2_r01[i].reward_r*0.1 + alg2_r[i].duel_r*0.9 << " "; // DecoFusion, 0.9
        outfile<<std::fixed<<std::setprecision(2)<<std::left<<std::setw(8)<<alg2_r09[i].reward_r*0.9 + alg2_r[i].duel_r*0.1 << std::endl; // DecoFusion, 0.1
    }
}

void output(std::ofstream& outfile) {
    outfile << ">>=====================================================================================================================<<" << std::endl;
    outfile << "Average result over " << ROUND << " rounds of " << alg::K << " arms over " << alg::T << " iterations per round." << std::endl;
    outfile << "---> seed:  " << env::seed  << std::endl;
    outfile << "---> delta: " << alg::delta << std::endl;
    outfile << "---> alpha: " << alg::alpha << std::endl;
    outfile << "---> expectations:" << std::endl;
    for (const auto &exp : env::expectations) {
        outfile << std::fixed << std::setprecision(2) << std::left << std::setw(4) << exp << " ";
    }
    outfile << std::endl;
    outfile << "---> preference matrix:" << std::endl;
    for (const auto& line : env::preference_matrix) {
        for (const auto& element : line) {
            outfile << std::fixed << std::setprecision(2) << std::left << std::setw(4) << element << " ";
        }
        outfile << std::endl;
    }
}

int main() {
    alg::K = static_cast<int>(env::expectations.size());

    env::seed = static_cast<unsigned>(time(nullptr));
    srand(env::seed);

    # if 1 // 根据 T 变化

    alg::delta = 1.0 / static_cast<double>(1LL * alg::T);
    auto * reward_elim_regrets = new opr::regrets[alg::T+1], * dueling_elim_regrets = new opr::regrets[alg::T+1], *alg1_regrets = new opr::regrets[alg::T+1], *alg2_regrets1 = new opr::regrets[alg::T+1], *alg2_regrets2 = new opr::regrets[alg::T+1], *alg2_regrets3 = new opr::regrets[alg::T+1], *alg2_regrets0_1 = new opr::regrets[alg::T+1], *alg2_regrets0_9 = new opr::regrets[alg::T+1];
    std::ofstream outfile(path);

    output(outfile);

    for (int i = 0; i < ROUND; ++i) {
        std::cout << "=====> run = " << i+1 << std::endl;
        alg::RewardElim RewardElim;
        RewardElim.Run(reward_elim_regrets);

        alg::DuelingElim DuelingElim;
        DuelingElim.Run(dueling_elim_regrets);

        alg::elimination algo1;
        algo1.Run(alg1_regrets);

        alg::alpha = 0;
        alg::decomposition algo2_1;
        algo2_1.Run(alg2_regrets1);
        alg::alpha = 1;
        alg::decomposition algo2_2; // always duel explore
        algo2_2.Run(alg2_regrets2);
        alg::alpha = 0.5;
        alg::decomposition algo2_3;
        algo2_3.Run(alg2_regrets3);

        alg::alpha = 0.1;
        alg::decomposition algo2_0_1;
        algo2_0_1.Run(alg2_regrets0_1);

        alg::alpha = 0.9;
        alg::decomposition algo2_0_9;
        algo2_0_9.Run(alg2_regrets0_9);
        alg::alpha = 0.5;

        output_data(outfile, alg1_regrets, alg2_regrets1, alg2_regrets2, alg2_regrets3, reward_elim_regrets, dueling_elim_regrets, alg2_regrets0_1, alg2_regrets0_9);
    }
    outfile.close();
    std::cout << "Data written to file: " << path << std::endl;
    #endif

    #if 0 // 根据 alpha 变化
    const std::string path_alpha = "../out.nosync/alpha.txt";
    std::ofstream outfile_alpha(path_alpha);

    outfile_alpha << ">>=====================================================================================================================<<" << std::endl;
    outfile_alpha << "Average result over " << ROUND << " rounds of " << alg::K << " arms over " << alg::T << " iterations per round." << std::endl;
    outfile_alpha << "---> seed:  " << env::seed  << std::endl;
    outfile_alpha << "---> delta: " << alg::delta << std::endl;
    outfile_alpha << "---> expectations:" << std::endl;
    for (const auto x : env::expectations) {
        outfile_alpha << x << " ";
    }
    outfile_alpha << std::endl;
    outfile_alpha << "---> preference matrix:" << std::endl;
    for (const auto &line : env::preference_matrix) {
        for (const auto &element : line) {
            outfile_alpha << std::fixed << std::setprecision(2) << std::left << std::setw(4) << element;
        }
        outfile_alpha << std::endl;
    }

    double duel_regrets[51][ROUND]; // range from 0.00 to 1.00, regrets[i] 表示 alpha = i * 0.02 的 regret
    double reward_regrets[51][ROUND];
    for (int i = 0; i <= 50; i++) {
        for (int j = 0; j < ROUND; ++j) {
            reward_regrets[i][j] = 0.0;
            duel_regrets[i][j] = 0;
        }
        alg::alpha = 0.02 * i;
        for (int j = 0; j < ROUND; ++j) {
            alg::decomposition alg2;
            alg2.RunAlpha(reward_regrets[i][j], duel_regrets[i][j]);
        }
        //
        std::cout << alg::alpha << std::endl;
    }
    for (int j = 0; j < ROUND; ++j) {
        outfile_alpha << "alpha  #reward regrets #duel regrets #regrets" << std::endl;
        for (int i = 0; i <= 50; i++) {
            outfile_alpha << (0.02 * i) << " " << reward_regrets[i][j] << " " << duel_regrets[i][j] << " " << ((0.02 * i) * reward_regrets[i][j] + (1 - 0.02 * i) * duel_regrets[i][j]) << std::endl;
        }
        outfile_alpha << std::endl;
    }

    outfile_alpha << std::endl;
    outfile_alpha.close();
    std::cout << "Data written to file: " << path_alpha << std::endl;
    #endif
    return 0;
}
