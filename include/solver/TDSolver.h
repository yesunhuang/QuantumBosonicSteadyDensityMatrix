/* Copyright 2021 YesunHuang & guch8017 */
//
// Created by Yesun on 2021/3/10.
//

#ifndef INCLUDE_SOLVER_TDSOLVER_H_
#define INCLUDE_SOLVER_TDSOLVER_H_

#include <vector>
#include <random>
#include <omp.h>
#include "./complex.h"
#include "./MatrixMapper.h"
#include "expression/EpDeriver.h"

#define REC_TIMES 8

class TDSolver {
private:
    /**
     * Hyper parameters
     */
    double alpha;
    double gamma;
    int maxRecTimes;

    std::default_random_engine rdEngine;
    std::uniform_int_distribution<unsigned> uid;

    EpDeriver epDeriver;

    std::vector<int> matrixSizeArray;
    std::vector<std::vector<int>> mapper;
    size_t neighbourCnt;

    MatrixMapper *src;
    std::vector<MatrixMapper *> dst;

    inline size_t getNextNeighbour();

    inline std::vector<int> getNeighbour(std::vector<int> root, int index);

    inline bool inbound(const std::vector<int> &index);

    void doRun(std::vector<int> index, int depth, int _i);

public:
    void run();

    TDSolver(std::vector<int> matrixSize, EpDeriver expression, double alpha, double gamma, int maxRecTimes) :
            alpha(alpha),
            gamma(gamma),
            maxRecTimes(maxRecTimes),
            matrixSizeArray(matrixSize),
            epDeriver(expression),
            mapper(epDeriver.neighbourIndexes) {
        src = new MatrixMapper(matrixSize);
        dst.resize(REC_TIMES);
        for (int i = 0; i < REC_TIMES; ++i) {
            dst[i] = new MatrixMapper(matrixSize);
        }
        uid = std::uniform_int_distribution<unsigned>(0, expression.neighbourIndexes.size() - 1);
    }

    MatrixMapper *getResult() { return src; }
};

#endif  // INCLUDE_SOLVER_TDSOLVER_H_
