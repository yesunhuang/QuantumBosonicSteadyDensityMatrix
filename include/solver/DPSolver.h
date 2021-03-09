/* Copyright 2021 YesunHuang & guch8017 */
//
// Created by 顾超 on 2021/2/16.
//

#ifndef INCLUDE_SOLVER_DPSOLVER_H_
#define INCLUDE_SOLVER_DPSOLVER_H_

#include <iostream>
#if __APPLE__
    // For macOS's OpenMP Library
    #include "/usr/local/include/omp.h"
#else
    #include <omp.h>
#endif
#include "./complex.h"
#include "expression/EpDeriver.h"
#include "./MatrixMapper.h"

class DPSolver {
private:
    /**
     * 超参数
     */
    const double alpha;
    const double minusAlpha;  // 1 - alpha
    const double epsilon;
    const int maxRecurveTimes;

    /**
     * 矩阵大小列表
     * 注意：m[0] = 3 代表第一层为 3 * 3
     */
    const std::vector<int> matrixSizeArray;

    EpDeriver epd;
    /**
     * 结果是否收敛
     */
    bool fit;

    /**
     * 邻居列表
     * 给出各个位的偏移值
     * 例如 原点 [0, 0] mapper[0]值为 [1, 1] 则代表其邻居为[0+1, 0+1] = [1, 1]
     * 注意此处的邻居对应的index应与function一一对应
     */
    std::vector<std::vector<int>> mapper;
    std::vector<int> absoluteOffset;

    MatrixMapper* mapSrc;
    MatrixMapper* mapDst;

    /**
     * 计算 sigma_{i\in neighbour}P(l_0^i,\cdots,k_{M-1}^i} \rho'_{l...}
     */
    inline ayaji::Complex leftSum(const std::vector<int>& indexArray);

    /**
     * 给出原点坐标，根据偏移计算并返回所有邻居坐标
     * @param root 原点坐标
     * @return 所有邻居坐标
     */
    inline std::vector<std::vector<int>> getNeighbours(
        const std::vector<int>& root);

    /**
     * 给出原点坐标，返回第 index 个邻居坐标
     * @param root 原点坐标
     * @param index 邻居编号
     * @return 邻居坐标
     */
    inline std::vector<int> getNeighbour(std::vector<int> root, int index);

    void doRun(int depth, const std::vector<int>& index);

    std::vector<int> getOpposite(const std::vector<int>& root);

public:
    DPSolver(const std::vector<int> &matrixSize,
             EpDeriver epDeriver,
             double alpha,
             double epsilon,
             size_t maxRetTime)
        : matrixSizeArray(matrixSize),
          mapper(epDeriver.neighbourIndexes),
          epd(epDeriver),
          alpha(alpha),
          minusAlpha(1 - alpha),
          epsilon(epsilon),
          maxRecurveTimes(maxRetTime) {
#ifdef DEBUG
        assert(mapper.size() == function.size() - 1 &&
               "Number of mappers doesn't fit number of functions");
#endif
        mapSrc = new MatrixMapper(matrixSize);
        mapDst = new MatrixMapper(matrixSize);
        for(auto map: mapper){
            absoluteOffset.push_back(mapSrc->getOffset(map));
        }
    }

    ~DPSolver() {
        delete mapSrc;
        delete mapDst;
    }

    /**
     * 执行求解器
     * @return 若精度满足要求则返回true，若达到迭代上限则返回false
     */
    bool run();

    MatrixMapper* getResult() { return mapSrc; }
};

#endif  // INCLUDE_SOLVER_DPSOLVER_H_
