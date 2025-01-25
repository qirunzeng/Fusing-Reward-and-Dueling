//
// Created by Qirun Zeng on 2025/1/10.
//


#include "algorithms.h"
#include "decomposition.h"
#include "elimination.h"
// #include "DUEL.h"
// #include "UCB.h"
#include "environment.h"
#include "operations.h"
#include "separation.h"
#include <iostream>
#include <vector>


#define ROUND 100
const std::string path = "../out/raw.txt";

#define REGRET_DATA 9

void output_data(std::ofstream& outfile, const opr::regrets *alg1_regrets, const opr::regrets *alg2_regrets, const opr::regrets *baseline_r) {
    outfile << "---> results:" << std::endl;
    outfile << "# T            # UCB    regret    # DUEL   regret # baseline r # reward regret(1) # duel regret(1)   # regret(1)        # reward regret(2) # duel regret(2)   # regret(2)" << std::endl;
    for (int i = 0; i <= REGRET_DATA; i++) {
        outfile << 0 << " ";
    }
    outfile << std::endl;
    for (int i = 1; i <= alg::T; ++i) {
        outfile << std::left << std::setw(14) << i << " ";
        outfile << std::fixed << std::setprecision(3) << std::left << std::setw(18) << (baseline_r[i].reward_r) << " ";
        outfile << std::fixed << std::setprecision(3) << std::left << std::setw(18) << (baseline_r[i].duel_r) << " ";
        outfile << std::fixed << std::setprecision(3) << std::left << std::setw(18) << (alg::alpha * baseline_r[i].reward_r + (1 - alg::alpha) * baseline_r[i].duel_r) << " ";
        outfile << std::fixed << std::setprecision(3) << std::left << std::setw(18) << (alg1_regrets[i].reward_r) << " ";
        outfile << std::fixed << std::setprecision(3) << std::left << std::setw(18) << (alg1_regrets[i].duel_r) << " ";
        outfile << std::fixed << std::setprecision(3) << std::left << std::setw(18) << (alg::alpha*alg1_regrets[i].reward_r + (1-alg::alpha)*alg1_regrets[i].duel_r) << " ";
        outfile << std::fixed << std::setprecision(3) << std::left << std::setw(18) << (alg2_regrets[i].reward_r) << " ";
        outfile << std::fixed << std::setprecision(3) << std::left << std::setw(18) << (alg2_regrets[i].duel_r) << " ";
        outfile << std::fixed << std::setprecision(3) << std::left << std::setw(18) << (alg::alpha*alg2_regrets[i].reward_r + (1-alg::alpha)*alg2_regrets[i].duel_r) << std::endl;
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
        // outfile << exp << " ";
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

    alg::delta = 1.0 / static_cast<double>(1LL * alg::K * alg::K * alg::T * alg::T);

    // auto *UCB_regrets = new double[alg::T+1], *DUEL_regrets = new double[alg::T+1];
    auto * baseline_r = new opr::regrets[alg::T+1], *alg1_regrets = new opr::regrets[alg::T+1], *alg2_regrets = new opr::regrets[alg::T+1];

    # if 1 // 根据 T 变化
    std::ofstream outfile(path, std::ios::app);

    output(outfile);

    for (int i = 0; i < ROUND; ++i) {
        std::cout << "=====> run = " << i+1 << std::endl;
        // alg::UCB ucb;
        // ucb.Run(UCB_regrets);
        // alg::DUEL duel;
        // duel.Run(DUEL_regrets);
        alg::separation baseline;
        baseline.Run(baseline_r);
        alg::elimination algo1;
        algo1.Run(alg1_regrets);
        alg::decomposition algo2;
        algo2.Run(alg2_regrets);
        output_data(outfile, alg1_regrets, alg2_regrets, baseline_r);
    }
    outfile.close();
    std::cout << "Data written to file: " << path << std::endl;
    #endif

    #if 0 // 根据 alpha 变化
    const std::string path_alpha = "../out/alpha.txt";
    std::ofstream outfile_alpha(path_alpha, std::ios::app);

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
