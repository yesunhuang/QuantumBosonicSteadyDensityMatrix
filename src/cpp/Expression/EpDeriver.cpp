/* Copyright 2021 YesunHuang */
#include <iostream>
#include <functional>
#include "MatrixMapper.h"
#include "complex.h"

/**
 * @name: Factor
 * @fuction: 记录单个模式的乘子，见样例中[mode,[m,n]]
 */
struct Factor
{
   int mode;
   int bra;//m
   int ket;//n
};

/**
 * @name: Term
 * @fuction: 一项由多个Factors组成
 */
struct Term
{
    ayaji::Complex coef;
    std::vector<Factor> factors;
};

/**
 * @name: Expression
 * @fuction: 一个表达式由多个Term组成
 */
struct Expression
{
    std::vector<Term> Terms;
};

class EpDeriver{

}