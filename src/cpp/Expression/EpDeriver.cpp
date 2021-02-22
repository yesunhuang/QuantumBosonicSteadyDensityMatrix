/* Copyright 2021 YesunHuang */
#include <iostream>
#include <functional>
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
    ayaji::Complex *coef;
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
    std::vector<RawTerm> H;
    std::vector<RawTerm> co_ps;
    std::vector<int> rawIndex;

    void buildHEps() {
        //处理哈密顿量
        for (std::vector<RawTerm>::iterator
                rawTerm=(*this).H.begin();
                rawTerm!=(*this).H.end();rawTerm++) {
            // 左右一起处理了
            std::vector<int> neighborIndexL=rawIndex;
            std::vector<int> neighborIndexR=rawIndex;
            std::vector<Factor> neignborTermL=std::vector<Factor>();
            std::vector<Factor> neignborTermR=std::vector<Factor>();
            for (std::vector<int>::iterator
                    ops=(*rawTerm).termOp.begin();
                    ops!=(*rawTerm).termOp.end();ops++) {
                int mode=(*ops-1)/2+1;
                bool creat=((*ops)%2!=0);
                if (neignborTermL.size()==0||mode!=(*neignborTermL.end()).mode) {
                    Factor factor={mode,0,0};
                    neignborTermL.push_back(factor);
                    neignborTermR.push_back(factor);
                }
                // 预处理算符作用
                if (creat) {
                    neighborIndexL[2*(mode-1)]--;
                    neighborIndexR[2*mode-1]++;
                    (*neignborTermR.end()).ket++;
                }
                else
                {
                    neighborIndexL[2*(mode-1)]++;
                    neighborIndexR[2*mode-1]--;
                    (*neignborTermL.end()).bra++;
                }
            }
            Term termL={&(*rawTerm).coef,ayaji::Complex(0,1),neignborTermL};
            Term termR={&(*rawTerm).coef,ayaji::Complex(0,-1),neignborTermR};
            if (isSame(rawIndex,neighborIndexL)) {
                termL.coefFactor*=ayaji::Complex(-1,0);
                termR.coefFactor*=ayaji::Complex(-1,0);
                masterEps.push_back(termL);
                masterEps.push_back(termR);
                continue;
            }
            int pos=findInside(neignborIndexs,neighborIndexL);
            if (pos==-1)
            {
                std::vector<Term> newExpression=std::vector<Term>();
                newExpression.push_back(termL);
                neignborIndexs.push_back(neighborIndexL);
                neighborEps.push_back(newExpression);
            }
            else
                neighborEps[pos].push_back(termL);

            pos=findInside(neignborIndexs,neighborIndexR);
            if (pos==-1)
            {
                std::vector<Term> newExpression=std::vector<Term>();
                newExpression.push_back(termR);
                neignborIndexs.push_back(neighborIndexR);
                neighborEps.push_back(newExpression);
            }
            else
                neighborEps[pos].push_back(termR);
        }
    }

    void buildCollapseEps() {
        //处理坍塌算符
        for (std::vector<RawTerm>::iterator
                rawTerm=(*this).co_ps.begin();
                rawTerm!=(*this).co_ps.end();rawTerm++) {
            // 一次整体处理一项
            if ((*rawTerm).termOp.size()!=1)
                throw "Do not support this kind of Collapse Operator yet";
            int mode=((*rawTerm).termOp[0]-1)/2+1;
            bool creat=((*rawTerm).termOp[0]%2!=0);
            std::vector<int> neighborIndex=rawIndex;
            std::vector<Factor> neighborTerm=std::vector<Factor>();
            Term termM;
            if (creat) {
                neighborIndex[2*(mode-1)]-=1;
                neighborIndex[2*mode-1]-=1;
                termM={&(*rawTerm).coef,ayaji::Complex(1,0),neighborTerm};
                std::vector<Factor> TermLR=std::vector<Factor>();
                Term termLR={&(*rawTerm).coef,ayaji::Complex(1,0),TermLR};
                masterEps.push_back(termLR);
            } else {
                neighborIndex[2*(mode-1)]+=1;
                neighborIndex[2*mode-1]+=1;
                Factor factorM={mode,1,1};
                neighborTerm.push_back(factorM);
                termM={&(*rawTerm).coef,ayaji::Complex(1,0),neighborTerm};
                Factor factorL={mode,1,0};
                Factor factorR={mode,0,1};
                std::vector<Factor> TermL=std::vector<Factor>();
                std::vector<Factor> TermR=std::vector<Factor>();
                TermL.push_back(factorL);
                TermR.push_back(factorR);
                Term termL={&(*rawTerm).coef,ayaji::Complex(0.5,0),TermL};
                Term termR={&(*rawTerm).coef,ayaji::Complex(0.5,0),TermR};
                masterEps.push_back(termL);
                masterEps.push_back(termR);
            }
            int pos=findInside(neignborIndexs,neighborIndex);
            if (pos==-1)
            {
                std::vector<Term> newExpression=std::vector<Term>();
                newExpression.push_back(termM);
                neignborIndexs.push_back(neighborIndex);
                neighborEps.push_back(newExpression);
            }
            else
                neighborEps[pos].push_back(termM);
        }
    }

    bool isSame(std::vector<int> a,std::vector<int> b) {
        if (a.size()!=b.size()) return false;
        for (int i=0;i<a.size();i++) 
            if (a[i]!=b[i])
                return false;
        return true;
    }

    int findInside(std::vector<std::vector<int>> array,std::vector<int> target) {
        for (int i=0;i<array.size();i++) {
            if (isSame(array[i],target))
                return i;
        }
        return -1;
    }
public:
    // 乘在当前元上的表达式
    std::vector<Term> masterEps;
    // 邻居相对,size()为邻居个数
    std::vector<std::vector<int>> neignborIndexs;
    // 乘在邻居上的表达式，和坐标一一对应
    std::vector<std::vector<Term>> neighborEps;
    /**
     * @name: EpDeriver(std::vector<RawTerm> hamiltonian,
              std::vector<RawTerm> collapse,
              std::vector<int> index)
     * @fuction: 预处理
     * @param {*hamiltonian} 哈密顿量
     * @param {*collapse} 坍塌算符
     * @param {*index} 全是零的索引
     * @return {*EpDriver}
     */
    EpDeriver(std::vector<RawTerm> hamiltonian,
              std::vector<RawTerm> collapse,
              std::vector<int> index) {
        (*this).H=hamiltonian;
        (*this).co_ps=collapse;
        (*this).rawIndex=index;
        (*this).masterEps=std::vector<Term>();
        (*this).neignborIndexs=std::vector<std::vector<int>>();
        (*this).neighborEps=std::vector<std::vector<Term>>();
        buildHEps();
        buildCollapseEps();
    }
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
                        std::vector<int> index) {
        ayaji::Complex result=ayaji::Complex();
        // 遍历表达式
        for (std::vector<Term>::iterator 
                term=expression.begin();
                term!=expression.end();term++) {
            ayaji::Complex termVal=ayaji::Complex();
            termVal+=((*(*term).coef)*(*term).coefFactor);
            double realTermVal=1;
            // 遍历当前项中的不同因子
            for (std::vector<Factor>::iterator 
                    factor=(*term).factors.begin();
                    factor!=(*term).factors.end();factor++) {
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

    /**
     * @name: updateCoef
     * @fuction: 更新系数,不用再预处理一次
     * @param {*hamiltonian} 哈密顿量
     * @return {*collapse} 坍塌算符
     */
    void updateCoef(std::vector<RawTerm> hamiltonian,
                    std::vector<RawTerm> collapse){
        if (hamiltonian.size()!=(*this).H.size()) {
            throw "update Hamiltonian error!";
        }
        for (int i=0;i<hamiltonian.size();i++) {
            (*this).H[i].coef.setReal(hamiltonian[i].coef.getReal());
            (*this).H[i].coef.setImage(hamiltonian[i].coef.getImage());
        }
        if (collapse.size()!=(*this).co_ps.size()) {
            throw "update Collapse error!";
        }
        for (int i = 0; i < collapse.size(); i++)
        {
            (*this).co_ps[i].coef.setReal(collapse[i].coef.getReal());
            (*this).co_ps[i].coef.setImage(collapse[i].coef.getImage());
        }
    }
};
