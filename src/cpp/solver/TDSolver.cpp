/* Copyright 2021 YesunHuang & guch8017 */
//
// Created by guch8017 on 2021/2/10.
//

#include "solver/TDSolver.h"

inline size_t TDSolver::getNextNeighbour() {
    return uid(rdEngine);
}

inline std::vector<int> TDSolver::getNeighbour(std::vector<int> root, int index) {
    auto offset = mapper[index];
    for (int i = 0; i < root.size(); ++i) {
        root[i] += offset[i];
    }
    return root;
}

inline bool TDSolver::inbound(const std::vector<int> &index) {
    for (int i = 0; i < index.size(); ++i) {
        if (index[i] < 0 || index[i] >= matrixSizeArray[i / 2]) {
            return false;
        }
    }
    return true;
}

void TDSolver::doRun(std::vector<int> index, int depth, int _i) {
    if (depth == matrixSizeArray.size()) {
        std::vector<int> root = index;
        do {
            size_t nextNeighbourIndex = getNextNeighbour();
            std::vector<int> nextNeighbour = getNeighbour(root, nextNeighbourIndex);
            // 更新原值
            dst[_i]->set(root, src->get(root) + alpha * (gamma * epDeriver.calMasterEP(root) /
                                                         epDeriver.calNeighbourEP(nextNeighbour,
                                                                                  nextNeighbourIndex) *
                                                         src->get(nextNeighbourIndex) - src->get(root)));
            root = nextNeighbour;
        } while (inbound(root));
    } else {
        for (int i = 0; i < matrixSizeArray[depth]; ++i) {
            auto nIndex = index;
            nIndex.push_back(i);
            nIndex.push_back(i);
            doRun(nIndex, depth + 1, _i);
        }
    }
}

void TDSolver::run() {
    for (int j = 0; j < maxRecTimes; ++j) {
#pragma omp parallel for
        for (int i = 0; i < REC_TIMES; ++i) {
            doRun(std::vector<int>(), 0, i);
        }
            // 求平均
#pragma omp parallel for
        for (int i = 0; i < src->getLength(); ++i) {
            ayaji::Complex res;
            for (int k = 0; k < REC_TIMES; ++k) {
                res += dst[k]->get(i);
            }
            src->set(i, res / REC_TIMES);
        }
    }
}
