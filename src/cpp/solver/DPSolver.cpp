/* Copyright 2021 YesunHuang & guch8017 */
//
// Created by guch8017 on 2021/2/10.
//

#include "solver/DPSolver.h"

inline std::vector<std::vector<int>> DPSolver::getNeighbours(
    const std::vector<int>& root) {
    std::vector<std::vector<int>> ret;
    for (auto offset : mapper) {
        auto index = root;
        for (int i = 0; i < root.size(); ++i) {
            index[i] += offset[i];
        }
        ret.push_back(index);
    }
    return ret;
}

inline std::vector<int> DPSolver::getNeighbour(std::vector<int> root,
                                               int index) {
    auto offset = mapper[index];
    for (int i = 0; i < root.size(); ++i) {
        root[i] += offset[i];
    }
    return root;
}

inline ayaji::Complex DPSolver::leftSum(const std::vector<int>& indexArray) {
    ayaji::Complex sum;
    auto neighbourIndex = getNeighbours(indexArray);
    for (int i = 0; i < neighbourIndex.size(); ++i) {
        sum +=
            epd.calNeighbourEP(indexArray, i) * mapSrc->get(neighbourIndex[i]);
    }
    return sum;
}

void DPSolver::doRun(int depth, const std::vector<int>& index) {
    if (depth == matrixSizeArray.size()) {
        ayaji::Complex s1 = leftSum(index);
        ayaji::Complex P0 = epd.calMasterEP(index);
        ayaji::Complex self = mapSrc->get(index);
        ayaji::Complex newValue = alpha / P0 * s1 + minusAlpha * self;
        if (fit) {
            // 判断该单位是否满足收敛需求
            ayaji::Complex modValue = P0 * self - s1;
            double testValue = hypot(modValue.getImage(), modValue.getReal()) /
                               std::max((P0 * self).getReal(), epsilon);
            if (testValue > epsilon) {
                fit = false;
            }
        }
        mapDst->set(index, newValue);
    }
    int loopSize = matrixSizeArray[depth];
    // 主循环体
    // 所有数据都是从mapSrc读取写入到mapDst，故不会出现读写访问冲突，不加锁
#pragma omp parallel for
    for (int i = 0; i < loopSize; ++i) {
        for (int j = 0; j < loopSize; ++j) {
            auto nInd = std::vector<int>(index);
            nInd.push_back(i);
            nInd.push_back(j);
            doRun(depth + 1, nInd);
        }
    }
}

bool DPSolver::run() {
    // TODO 预设存储的值

    // 最外层循环为大迭代，不考虑并行关系
    for (int i = 0; i < maxRecurveTimes; ++i) {
        fit = true;
        doRun(0, std::vector<int>());
        std::swap(mapSrc, mapDst);
        if (fit) {
            return true;
        }
    }
    return false;
}
