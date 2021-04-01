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
    auto offset = mapSrc->getOffset(indexArray);
    for (int i = 0; i < neighbourIndex.size(); ++i) {
        sum +=
            epd.calNeighbourEP(neighbourIndex[i], i) * mapSrc->get(offset + absoluteOffset[i], neighbourIndex[i]);
    }
    return sum;
}

std::vector<int> DPSolver::getOpposite(const std::vector<int>& root) {
    std::vector<int> tmp = std::vector<int>();
    for (int i = 0; i < root.size(); i += 2) {
        tmp.push_back(root[i + 1]);
        tmp.push_back(root[i]);
    }
    return tmp;
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
        mapDst->set(getOpposite(index), newValue.conj());
        return;
    }
    int loopSize = matrixSizeArray[depth];
    // 主循环体
    // 所有数据都是从mapSrc读取写入到mapDst，故不会出现读写访问冲突，不加锁
    #pragma omp parallel for
    for (int i = 0; i < loopSize; ++i) {
        for (int j = 0; j < loopSize; ++j) {
            {
                auto nInd = std::vector<int>(index);
                nInd.push_back(i);
                nInd.push_back(j);
                doRun(depth + 1, nInd);
            }
        }
    }
}

ayaji::Complex DPSolver::getZeroSum(int depth, int mul, int* index){
    if(depth == matrixSizeArray.size()) {
        return mul * mapSrc->get(index);
    } else {
        ayaji::Complex ret;
        // 从1开始，包含0的项必定结果为0
        for (int i = 1; i < matrixSizeArray[depth]; ++i) {
            index[depth * 2] = i;
            for (int j = 1; j < matrixSizeArray[depth]; ++j) {
                index[depth * 2 + 1] = j;
                ret += getZeroSum(depth + 1, mul * i * j, index);
            }
        }
        return ret;
    }
}

bool DPSolver::run() {
    // 最外层循环为大迭代，不考虑并行关系
    int* list = (int *)malloc(sizeof (int ) * matrixSizeArray.size() * 2);
    for (int i = 0; i < maxRecurveTimes; ++i) {
        fit = true;
        doRun(0, std::vector<int>());

        // 对全0点的修正
        mapDst->set(0, alpha * (1 - getZeroSum(0, 1, list)) + minusAlpha * mapSrc->get((size_t)0));

        std::swap(mapSrc, mapDst);
        if (fit) {
            free(list);
            return true;
        }
    }
    free(list);
    return false;
}
