//
// Created by 顾超 on 2021/2/16.
//

#ifndef QUANTUM_DPSOLVER_H
#define QUANTUM_DPSOLVER_H

//
// Created by guch8017 on 2021/2/10.
//

#include <iostream>
#include <omp.h>
#include "complex.h"
#include <MatrixMapper.h>

class DPSolver {
private:
    /**
     * 超参数
     */
    ayaji::Complex alpha;
    ayaji::Complex minusAlpha;  // 1 - alpha
    double epsilon;
    int maxRecurveTimes;

    std::vector<int> matrixSizeArray;

    /**
     * 结果是否收敛
     */
    bool fit;

    /**
     * 邻居数量 由预处理给出
     */
    size_t neighborSize;

    /**
     * 函数P 由预处理给出
     * P[0] 为分母上的值， 其余对应各个邻居
     */
    std::vector<std::function<double(std::vector<int>)>> function;

    /**
     * 邻居列表
     * 给出各个位的偏移值
     * 例如 原点 [0, 0] mapper[0]值为 [1, 1] 则代表其邻居为[0+1, 0+1] = [1, 1]
     * 注意此处的邻居对应的index应与function一一对应
     */
    std::vector<std::vector<int>> mapper;

    MatrixMapper *mapSrc;
    MatrixMapper *mapDst;

    /**
     * 计算 sigma_{i\in neighbour}P(l_0^i,\cdots,k_{M-1}^i} \rho'_{l...}
     */
    inline ayaji::Complex leftSum(const std::vector<int> &indexArray);

    /**
     * 给出原点坐标，根据偏移计算并返回所有邻居坐标
     * @param root 原点坐标
     * @return 所有邻居坐标
     */
    inline std::vector<std::vector<int>> getNeighbours(const std::vector<int> &root);

    /**
     * 给出原点坐标，返回第 index 个邻居坐标
     * @param root 原点坐标
     * @param index 邻居编号
     * @return 邻居坐标
     */
    inline std::vector<int> getNeighbour(std::vector<int> root, int index);

    void doRun(int depth, const std::vector<int> &index);

public:

    DPSolver(std::vector<int> matrixSize, std::vector<std::vector<int>> mapper,
             std::vector<std::function<double(std::vector<int>)>> function, double alpha, double epsilon,
             size_t maxRetTime) : matrixSizeArray(matrixSize), mapper(mapper), function(function),
                                  alpha(ayaji::Complex(alpha, 0)),
                                  minusAlpha(ayaji::Complex(1 - alpha, 0)),
                                  epsilon(epsilon),
                                  maxRecurveTimes(maxRetTime){
#ifdef DEBUG
        assert(mapper.size() == function.size() - 1 && "Number of mappers doesn't fit number of functions");
#endif
        mapSrc = new MatrixMapper(matrixSize);
        mapDst = new MatrixMapper(matrixSize);

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

    MatrixMapper* getResult(){ return mapSrc; }
};

#endif //QUANTUM_DPSOLVER_H
