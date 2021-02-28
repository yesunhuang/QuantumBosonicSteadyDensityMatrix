/* Copyright 2021 YesunHuang & guch8017*/

#ifndef INCLUDE_EXPRESSION_EPDERIVER_H_
#define INCLUDE_EXPRESSION_EPDERIVER_H_

#include <functional>
#include <iostream>
#include <vector>
#include "./complex.h"
#include "MatrixMapper.h"

/**
 * @name: Factor
 * @function: 记录单个模式的乘子，见样例中[mode,[m,n]]
 */
struct Factor {
    int mode;
    int bra;  // m
    int ket;  // n
};

/**
 * @name: Term
 * @function: 预处理后的项，一项由多个Factors组成
 */
struct Term {
    ayaji::Complex* coef;
    // 因为前一个只有引用，所以单独开了一个空间存乘在系数上的因子(比如-1)
    ayaji::Complex coefFactor;
    std::vector<Factor> factors;
};

/**
 * @name: RawTerm
 * @function: 输入的原始项[Coef,int[]],见哈密顿量表示法
 */
struct RawTerm {
    ayaji::Complex coef;  // 只存复数的引用
    std::vector<int> termOp;
};

class EpDeriver {
private:
    std::vector<RawTerm> H;
    std::vector<RawTerm> co_ps;
    std::vector<int> rawIndex;

    void buildHEps();

    void buildCollapseEps();

    bool isSame(const std::vector<int>& a, const std::vector<int>& b) {
        if (a.size() != b.size())
            return false;
        for (int i = 0; i < a.size(); i++)
            if (a[i] != b[i])
                return false;
        return true;
    }

    int findInside(const std::vector<std::vector<int>>& array,
                   const std::vector<int>& target) {
        for (int i = 0; i < array.size(); i++) {
            if (isSame(array[i], target))
                return i;
        }
        return -1;
    }

public:
    // 乘在当前元上的表达式
    std::vector<Term> masterEps;
    // 邻居相对,size()为邻居个数
    std::vector<std::vector<int>> neighbourIndexes;
    // 乘在邻居上的表达式，和坐标一一对应
    std::vector<std::vector<Term>> neighborEps;
    /**
     * @name: EpDeriver(std::vector<RawTerm> hamiltonian,
              std::vector<RawTerm> collapse,
              std::vector<int> index)
     * @des: 预处理
     * @param {*hamiltonian} 哈密顿量
     * @param {*collapse} 坍塌算符
     * @param {*index} 全是零的索引
     * @return {*EpDriver}
     */
    EpDeriver(const std::vector<RawTerm>& hamiltonian,
              const std::vector<RawTerm>& collapse,
              const std::vector<int>& index)
        : H(hamiltonian), co_ps(collapse), rawIndex(index) {
        buildHEps();
        buildCollapseEps();
    }
    /**
     * @brief 计算表达式值
     * 
     * @name: calEp(Expression expression,std::vector<int> index)
     * @param {*expression}:符号表达式
     * @param {*index}:元素索引
     * @return {*int}:值
     */
    static ayaji::Complex calEp(const std::vector<Term>& expression,
                                const std::vector<int>& index);

    ayaji::Complex calNeighbourEP(const std::vector<int>& root, int index) {
#ifdef DEBUG
        assert(index >= neighbourIndexes.size() &&
               "Neighbour index out of range");
#endif
        return calEp(neighborEps[index], root);
    }

    ayaji::Complex calMasterEP(const std::vector<int>& root) {
        return calEp(masterEps, root);
    }

    /**
     * @name: updateCoef
     * @function: 更新系数,不用再预处理一次
     * @param {*hamiltonian} 哈密顿量
     * @return {*collapse} 坍塌算符
     */
    void updateCoef(std::vector<RawTerm> hamiltonian,
                    std::vector<RawTerm> collapse);
};

#endif  // INCLUDE_EXPRESSION_EPDERIVER_H_
