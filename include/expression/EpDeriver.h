//
// Created by 顾超 on 2021/2/16.
//

#ifndef QUANTUM_EPDERIVER_H
#define QUANTUM_EPDERIVER_H

#include <iostream>
#include "MatrixMapper.h"
#include "./complex.h"

/**
 * @name: Factor
 * @fuction: 记录单个模式的乘子，见样例中[mode,[m,n]]
 */
struct Factor{
    int mode;
    int bra;  // m
    int ket;  // n
};

/**
 * @name: Term
 * @fuction: 预处理后的项，一项由多个Factors组成
 */
struct Term{
    ayaji::Complex coef;
    // 因为前一个只有引用，所以单独开了一个空间存乘在系数上的因子(比如-1)
    ayaji::Complex coefFactor;
    std::vector<Factor> factors;
};

/**
 * @name: RawTerm
 * @fuction: 输入的原始项[Coef,int[]],见哈密顿量表示法
 */
struct RawTerm{
    ayaji::Complex coef; // 只存复数的引用
    std::vector<int> termOp;
};

class EpDeriver{
private:
    std::vector<Term> expression;

    /**
     * @name: calEp(Expression expression,std::vector<int> index)
     * @fuction: 计算表达式值
     * @param {*expression}:符号表达式
     * @param {*index}:元素索引
     * @return {*int}:值
     */
    /*
    这个东西在使用时要特别注意，无论你是不是在求邻居的系数，你输入给它的index是当前
    矩阵元的index，即别把邻居的index传给它！！！
    */
    static ayaji::Complex calEp(std::vector<Term> expression,
                                std::vector<int> index);
public:
    EpDeriver(){
        //TODO
    };


    /**
     * @name: calEp(std::vector<int> index)
     * @fuction: 计算表达式值，供外部调用
     * @param {*index}:元素索引
     * @return {*int}:值
     */
    ayaji::Complex calEp(std::vector<int> index);
};

#endif //QUANTUM_EPDERIVER_H
