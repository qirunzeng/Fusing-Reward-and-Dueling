//
// Created by Qirun Zeng on 2025/1/10.
//


#include "decomposition.h"
#include "elimination.h"
#include "environment.h"
#include "operations.h"
#include "RewardElim.h"
#include "DuelingElim.h"
#include <iostream>
#include <vector>


#define ROUND 100
const std::string path = "../out.nosync/raw.txt";

#define REGRET_DATA 12

#define RUN_GAP         0
#define RUN_GAP_MAT     0
#define RUN_WITH_T      0
#define RUN_WITH_ALPHA  1

#define SEP ">>=====================================================================================================================<<"


void output_data(std::ofstream& outfile, const opr::regrets *alg1_r, const opr::regrets *alg2_regrets1, const opr::regrets *alg2_regrets2, const opr::regrets *reward_elim_r, const opr::regrets *dueling_elim_r, const opr::regrets *alg2_r05) {
    outfile << "---> results:" << std::endl;
    outfile << "# T            # ..." << std::endl;
    for (int i = 0; i <= REGRET_DATA; i++) {
        outfile << 0 << " ";
    }
    outfile << std::endl;
    for (int i = 1; i <= alg::T; ++i) {
        outfile<<std::left <<std::setw(14) << i << " ";
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

        outfile<<std::fixed<<std::setprecision(2)<<std::left<<std::setw(8)<<alg2_r05[i].reward_r*0.5 + alg2_r05[i].duel_r*0.5 << std::endl;   // DecoFusion, 0.5
    }
}

void output_data2(std::ofstream& outfile, const opr::regrets *alg1_r, const opr::regrets *alg2_regrets1, const opr::regrets *alg2_regrets2, const opr::regrets *reward_elim_r, const opr::regrets *dueling_elim_r, const opr::regrets *alg2_r05, const double GAP) {
    outfile << std::left << std::setw(14) << GAP << " ";
    outfile<<std::fixed<<std::setprecision(2)<<std::left<<std::setw(8)<<reward_elim_r[alg::T].reward_r*0.5 +dueling_elim_r[alg::T].duel_r*0.5 <<" ";  //ElimNoFusion
    outfile<<std::fixed<<std::setprecision(2)<<std::left<<std::setw(8)<<alg1_r[alg::T].reward_r * 0.5 + alg1_r[alg::T].duel_r * 0.5 << " ";           //ElimFusion
    outfile<<std::fixed<<std::setprecision(2)<<std::left<<std::setw(8)<<alg2_regrets1[alg::T].reward_r*0.5 + alg2_regrets2[alg::T].duel_r*0.5 << " "; // MEDNoFusion
    outfile<<std::fixed<<std::setprecision(2)<<std::left<<std::setw(8)<<alg2_r05[alg::T].reward_r*0.5 + alg2_r05[alg::T].duel_r*0.5 << std::endl;           // DecoFusion, 0.5
}

void output(std::ofstream& outfile) {
    outfile << "---> seed:  " << env::seed  << std::endl;
    outfile << "---> delta: " << alg::delta << std::endl;
    outfile << "---> alpha: " << alg::alpha << std::endl;
    outfile << "---> expectations:" << std::endl;
    for (const auto &exp : env::expectations) {
        outfile << std::fixed << std::setprecision(2) << std::left << std::setw(5) << exp << " ";
    }
    outfile << std::endl;
    outfile << "---> preference matrix:" << std::endl;
    for (const auto& line : env::preference_matrix) {
        for (const auto& element : line) {
            outfile << std::fixed << std::setprecision(2) << std::left << std::setw(5) << element << " ";
        }
        outfile << std::endl;
    }
}

int main() {
    alg::K = static_cast<int>(env::expectations.size());
    if (env::preference_matrix.size() != alg::K) {
        throw std::invalid_argument("Expectation matrix size does not match expected size.");
    }
    for (int i = 0; i < alg::K; ++i) {
        if (env::preference_matrix[i].size() != alg::K) {
        std::cout << env::preference_matrix[i].size() << std::endl;
            throw std::invalid_argument("Expectation matrix size does not match expected size.");
        }
    }

    env::seed = static_cast<unsigned>(time(nullptr));
    srand(env::seed);
    alg::delta = 1.0 / static_cast<double>(1LL * alg::T);

#if RUN_GAP
    const std::string path_gap = "../out.nosync/gap.txt";
    auto * gap_reward_elim_regrets = new opr::regrets[alg::T+1], * gap_dueling_elim_regrets = new opr::regrets[alg::T+1], *gap_alg1_regrets = new opr::regrets[alg::T+1], *gap_alg2_regrets0_5 = new opr::regrets[alg::T+1], *gap_alg2_r1 = new opr::regrets[alg::T+1], *gap_alg2_r2 = new opr::regrets[alg::T+1];
    std::ofstream outfile2(path_gap);

    outfile2 << SEP << std::endl;
    outfile2 << "Average result over " << ROUND << " rounds of " << alg::K << " arms over " << alg::T << " iterations per round." << std::endl;
    auto origin_exp = env::expectations;
    const double gaps[] = {0.06, 0.11, 0.16, 0.21};
    for (int i = 0; i < ROUND; ++i) {
        outfile2 << "# GAP" << std::endl;
        std::cout << "=====> Run = " << i+1 << std::endl;
        for (int x = 0; x < 4; ++x) {
            const double gap = gaps[x];
            std::cout << gap << std::endl;
            env::expectations[0] = 0.9;
            for (int j = 1; j < alg::K; ++j) {
                env::expectations[j] = env::expectations[j-1] - gap;
            }
            alg::RewardElim reward_elim;
            reward_elim.Run(gap_reward_elim_regrets);

            alg::DuelingElim dueling_elim;
            dueling_elim.Run(gap_dueling_elim_regrets);

            alg::elimination elim;
            elim.Run(gap_alg1_regrets);

            alg::alpha = 0;
            alg::decomposition decomp1;
            decomp1.Run(gap_alg2_r1);

            alg::alpha = 1;
            alg::decomposition decomp2;
            decomp2.Run(gap_alg2_r2);

            alg::alpha = 0.5;
            alg::decomposition decomp;
            decomp.Run(gap_alg2_regrets0_5);

            output_data2(outfile2, gap_alg1_regrets, gap_alg2_r1, gap_alg2_r2, gap_reward_elim_regrets, gap_dueling_elim_regrets, gap_alg2_regrets0_5, gap);
        }
    }
    env::expectations = origin_exp;
    outfile2.close();
#endif

#if RUN_GAP_MAT
    const std::string path_gap1 = "../out.nosync/gap-mat.txt";
    auto * mat_gap_reward_elim_regrets = new opr::regrets[alg::T+1], * mat_gap_dueling_elim_regrets = new opr::regrets[alg::T+1], *mat_gap_alg1_regrets = new opr::regrets[alg::T+1], *mat_gap_alg2_regrets0_5 = new opr::regrets[alg::T+1], *mat_gap_alg2_r1 = new opr::regrets[alg::T+1], *mat_gap_alg2_r2 = new opr::regrets[alg::T+1];
    std::ofstream outfile3(path_gap1);

    outfile3 << SEP << std::endl;
    outfile3 << "Average result over " << ROUND << " rounds of " << alg::K << " arms over " << alg::T << " iterations per round." << std::endl;
    auto origin_mat = env::preference_matrix;
    const double mat_gaps[] = {0.03, 0.05, 0.07, 0.09, 0.11};
    for (int i = 0; i < ROUND; ++i) {
        outfile3 << "# GAP" << std::endl;
        std::cout << "=====> Run = " << i+1 << std::endl;
        for (int x = 0; x < 5; ++x) {
            const double gap = mat_gaps[x];
            std::cout << "Curr gap: " << gap << std::endl;
            for (int l = 0; l < alg::K; ++l) {
                for (int j = l; j < alg::K; ++j) {
                    if (l == j) {
                        env::preference_matrix[l][j] = 0.5;
                    } else {
                        env::preference_matrix[l][j] = env::preference_matrix[l][j-1] + gap;
                    }
                }
            }
            for (int l = 0; l < alg::K; ++l) {
                for (int j = 0; j < l; ++j) {
                    env::preference_matrix[l][j] = 0.5 - env::preference_matrix[j][l];
                }
            }
            alg::RewardElim reward_elim;
            reward_elim.Run(mat_gap_reward_elim_regrets);

            alg::DuelingElim dueling_elim;
            dueling_elim.Run(mat_gap_dueling_elim_regrets);

            alg::elimination elim;
            elim.Run(mat_gap_alg1_regrets);

            alg::alpha = 0;
            alg::decomposition decomp1;
            decomp1.Run(mat_gap_alg2_r1);

            alg::alpha = 1;
            alg::decomposition decomp2;
            decomp2.Run(mat_gap_alg2_r2);

            alg::alpha = 0.5;
            alg::decomposition decomp;
            decomp.Run(mat_gap_alg2_regrets0_5);

            output_data2(outfile3, mat_gap_alg1_regrets, mat_gap_alg2_r1, mat_gap_alg2_r2, mat_gap_reward_elim_regrets, mat_gap_dueling_elim_regrets, mat_gap_alg2_regrets0_5, gap);
        }
    }
    env::preference_matrix = origin_mat;
    outfile3.close();
#endif


# if RUN_WITH_T
    auto * reward_elim_regrets = new opr::regrets[alg::T+1], * dueling_elim_regrets = new opr::regrets[alg::T+1], *alg1_regrets = new opr::regrets[alg::T+1], *alg2_regrets1 = new opr::regrets[alg::T+1], *alg2_regrets2 = new opr::regrets[alg::T+1], *alg2_regrets0_5 = new opr::regrets[alg::T+1];
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
        alg::decomposition algo2_0_5;
        algo2_0_5.Run(alg2_regrets0_5);

        output_data(outfile, alg1_regrets, alg2_regrets1, alg2_regrets2, reward_elim_regrets, dueling_elim_regrets, alg2_regrets0_5);
    }
    outfile.close();
    std::cout << "Data written to file: " << path << std::endl;
#endif

#if RUN_WITH_ALPHA
    const std::string path_alpha = "../out.nosync/alpha.txt";
    std::ofstream outfile_alpha(path_alpha);

    outfile_alpha << SEP << std::endl;
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
            outfile_alpha << std::fixed << std::setprecision(2) << std::left << std::setw(5) << element;
        }
        outfile_alpha << std::endl;
    }

    constexpr double gap = 0.02;
    constexpr double start = 0.0;
    constexpr int tot = 50;

    double duel_regrets[tot+1][ROUND];  // range from 0.00 to 1.00, step 0.02
    double reward_regrets[tot+1][ROUND];
    for (int i = 0; i <= tot; i++) {
        for (int j = 0; j < ROUND; ++j) {
            reward_regrets[i][j] = 0.0;
            duel_regrets[i][j] = 0;
        }
        alg::alpha = gap * i + start;
        for (int j = 0; j < ROUND; ++j) {
            alg::decomposition alg2;
            alg2.RunAlpha(reward_regrets[i][j], duel_regrets[i][j]);
        }
        std::cout << alg::alpha << std::endl;
    }

    for (int j = 0; j < ROUND; ++j) {
        outfile_alpha << "alpha  #reward regrets #duel regrets #regrets" << std::endl;
        for (int i = 0; i <= tot; i++) {
            outfile_alpha << gap * i + start << " " << reward_regrets[i][j] << " " << duel_regrets[i][j] << " " << ((gap * i + start) * reward_regrets[i][j] + (1 - (gap*i + start)) * duel_regrets[i][j]) << std::endl;
        }
        outfile_alpha << std::endl;
    }

    outfile_alpha << std::endl;
    outfile_alpha.close();
    std::cout << "Data written to file: " << path_alpha << std::endl;
#endif
    return 0;
}
