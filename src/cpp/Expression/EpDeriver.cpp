/* Copyright 2021 YesunHuang */

#include "EpDeriver.h"

void EpDeriver::buildHEps() {
    //处理哈密顿量
    for (std::vector<RawTerm>::iterator
                 rawTerm = (*this).H.begin();
         rawTerm != (*this).H.end(); rawTerm++) {
        // 左右一起处理了
        std::vector<int> neighborIndexL = rawIndex;
        std::vector<int> neighborIndexR = rawIndex;
        std::vector<Factor> neignborTermL = std::vector<Factor>();
        std::vector<Factor> neignborTermR = std::vector<Factor>();
        for (std::vector<int>::iterator
                     ops = (*rawTerm).termOp.begin();
             ops != (*rawTerm).termOp.end(); ops++) {
            int mode = (*ops - 1) / 2 + 1;
            bool creat = ((*ops) % 2 != 0);
            if (neignborTermL.size() == 0 || mode != (*neignborTermL.end()).mode) {
                Factor factor = {mode, 0, 0};
                neignborTermL.push_back(factor);
                neignborTermR.push_back(factor);
            }
            // 预处理算符作用
            if (creat) {
                neighborIndexL[2 * (mode - 1)]--;
                neighborIndexR[2 * mode - 1]++;
                (*neignborTermR.end()).ket++;
            } else {
                neighborIndexL[2 * (mode - 1)]++;
                neighborIndexR[2 * mode - 1]--;
                (*neignborTermL.end()).bra++;
            }
        }
        Term termL = {&(*rawTerm).coef, ayaji::Complex(0, 1), neignborTermL};
        Term termR = {&(*rawTerm).coef, ayaji::Complex(0, -1), neignborTermR};
        if (isSame(rawIndex, neighborIndexL)) {
            termL.coefFactor *= ayaji::Complex(-1, 0);
            termR.coefFactor *= ayaji::Complex(-1, 0);
            masterEps.push_back(termL);
            masterEps.push_back(termR);
            continue;
        }
        int pos = findInside(neighbourIndexes, neighborIndexL);
        if (pos == -1) {
            std::vector<Term> newExpression = std::vector<Term>();
            newExpression.push_back(termL);
            neighbourIndexes.push_back(neighborIndexL);
            neighborEps.push_back(newExpression);
        } else
            neighborEps[pos].push_back(termL);

        pos = findInside(neighbourIndexes, neighborIndexR);
        if (pos == -1) {
            std::vector<Term> newExpression = std::vector<Term>();
            newExpression.push_back(termR);
            neighbourIndexes.push_back(neighborIndexR);
            neighborEps.push_back(newExpression);
        } else
            neighborEps[pos].push_back(termR);
    }
}

void EpDeriver::buildCollapseEps() {
    //处理坍塌算符
    for (std::vector<RawTerm>::iterator
                 rawTerm = (*this).co_ps.begin();
         rawTerm != (*this).co_ps.end(); rawTerm++) {
        // 一次整体处理一项
        if ((*rawTerm).termOp.size() != 1)
            throw "Do not support this kind of Collapse Operator yet";
        int mode = ((*rawTerm).termOp[0] - 1) / 2 + 1;
        bool creat = ((*rawTerm).termOp[0] % 2 != 0);
        std::vector<int> neighborIndex = rawIndex;
        std::vector<Factor> neighborTerm = std::vector<Factor>();
        Term termM;
        if (creat) {
            neighborIndex[2 * (mode - 1)] -= 1;
            neighborIndex[2 * mode - 1] -= 1;
            termM = {&(*rawTerm).coef, ayaji::Complex(1, 0), neighborTerm};
            std::vector<Factor> TermLR = std::vector<Factor>();
            Term termLR = {&(*rawTerm).coef, ayaji::Complex(1, 0), TermLR};
            masterEps.push_back(termLR);
        } else {
            neighborIndex[2 * (mode - 1)] += 1;
            neighborIndex[2 * mode - 1] += 1;
            Factor factorM = {mode, 1, 1};
            neighborTerm.push_back(factorM);
            termM = {&(*rawTerm).coef, ayaji::Complex(1, 0), neighborTerm};
            Factor factorL = {mode, 1, 0};
            Factor factorR = {mode, 0, 1};
            std::vector<Factor> TermL = std::vector<Factor>();
            std::vector<Factor> TermR = std::vector<Factor>();
            TermL.push_back(factorL);
            TermR.push_back(factorR);
            Term termL = {&(*rawTerm).coef, ayaji::Complex(0.5, 0), TermL};
            Term termR = {&(*rawTerm).coef, ayaji::Complex(0.5, 0), TermR};
            masterEps.push_back(termL);
            masterEps.push_back(termR);
        }
        int pos = findInside(neighbourIndexes, neighborIndex);
        if (pos == -1) {
            std::vector<Term> newExpression = std::vector<Term>();
            newExpression.push_back(termM);
            neighbourIndexes.push_back(neighborIndex);
            neighborEps.push_back(newExpression);
        } else
            neighborEps[pos].push_back(termM);
    }
}

ayaji::Complex EpDeriver::calEp(const std::vector<Term>& expression,
                     const std::vector<int>& index) {
    ayaji::Complex result = ayaji::Complex();
    // 遍历表达式
    for (auto term = expression.begin();
         term != expression.end(); term++) {
        ayaji::Complex termVal = ayaji::Complex();
        termVal += ((*(*term).coef) * (*term).coefFactor);
        double realTermVal = 1;
        // 遍历当前项中的不同因子
        for (auto factor = (*term).factors.begin();
             factor != (*term).factors.end(); factor++) {
            // 获取对应的行列索引
            int row = index[2 * ((*factor).mode - 1)];
            int col = index[2 * (*factor).mode - 1];
            for (int i = row + 1; i <= row + (*factor).bra; i++)
                realTermVal *= i;
            for (int i = col + 1; i <= col + (*factor).ket; i++)
                realTermVal *= i;
        }
        termVal *= ayaji::Complex(realTermVal, 0);
        result += termVal;
    }
    return result;
}

void EpDeriver::updateCoef(std::vector<RawTerm> hamiltonian,
                           std::vector<RawTerm> collapse) {
    if (hamiltonian.size() != (*this).H.size()) {
        throw "update Hamiltonian error!";
    }
    for (int i = 0; i < hamiltonian.size(); i++) {
        (*this).H[i].coef.setReal(hamiltonian[i].coef.getReal());
        (*this).H[i].coef.setImage(hamiltonian[i].coef.getImage());
    }
    if (collapse.size() != (*this).co_ps.size()) {
        throw "update Collapse error!";
    }
    for (int i = 0; i < collapse.size(); i++) {
        (*this).co_ps[i].coef.setReal(collapse[i].coef.getReal());
        (*this).co_ps[i].coef.setImage(collapse[i].coef.getImage());
    }
}