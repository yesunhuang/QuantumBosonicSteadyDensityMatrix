/* Copyright 2021 YesunHuang */

#include "EpDeriver.h"

ayaji::Complex EpDeriver::calEp(std::vector<Term> expression, std::vector<int> index){
    ayaji::Complex result=ayaji::Complex();
    // 遍历表达式
    for(std::vector<Term>::iterator
                term=expression.begin();
        term!=expression.end();term++){
        ayaji::Complex termVal=ayaji::Complex();
        termVal+=((*term).coef*(*term).coefFactor);
        double realTermVal=1;
        // 遍历当前项中的不同因子
        for(std::vector<Factor>::iterator
                    factor=(*term).factors.begin();
            factor!=(*term).factors.end();factor++){
            // 获取对应的行列索引
            int row=index[2*((*factor).mode-1)];
            int col=index[2*(*factor).mode-1];
            for (int i = row+1; i <= row+(*factor).bra; i++)
                realTermVal*=i;
            for (int i = col+1; i <= col+(*factor).ket; i++)
                realTermVal*=i;
        }
        termVal*=ayaji::Complex(realTermVal,0);
        result+=termVal;
    }
    return result;
}

ayaji::Complex EpDeriver::calEp(std::vector<int> index){
    return calEp(expression, index);
}

