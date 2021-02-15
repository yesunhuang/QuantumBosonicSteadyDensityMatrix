//
// Created by guch8017 on 2021/2/10.
//

#include <iostream>
#include <functional>
// TODO: 更改为<omp.h> 此处为macOS适配，先写成这样
// TODO: 提取头文件
#include <omp.h>
#include <MatrixMapper.h>

class DPSolver{
private:
    /**
     * 超参数
     */
    double alpha;
    double minusAlpha;  // 1 - alpha
    double epsilon;
    double maxRecurveTimes;

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
     * 函数Neighbor 由预处理给出
     * 给出l0, r0, ..., lM-1, rM-1, 返回其邻居坐标列表
     * 以vector方式传入参数
     * 以vector<vector>方式返回
     */
    std::function<std::vector<std::vector<int>>(std::vector<int>)> mapper;

    MatrixMapper mapSrc;
    MatrixMapper mapDst;

    /**
     * 计算 sigma_{i\in neighbour}P(l_0^i,\cdots,k_{M-1}^i} \rho'_{l...}
     */
     inline std::complex<double> leftSum(const std::vector<int> &indexArray){
        std::complex<double> sum = 0;
        auto neighbourIndex = mapper(indexArray);
        for(int i = 0; i < neighborSize; ++i){
            sum += function[i + 1](neighbourIndex[i]) * mapSrc.get(neighbourIndex[i]);
        }
        return sum;
     }

    void doRun(int depth, const std::vector<int> &index){
        if(depth == matrixSizeArray.size()){
            std::complex<double> s1 = leftSum(index);
            std::complex<double> P0 = function[0](index);
            std::complex<double> self = mapSrc.get(index);
            std::complex<double> newValue = alpha / P0 * s1 + minusAlpha * self;
            if(fit) {
                // 判断该单位是否满足收敛需求
                std::complex<double> modValue = P0 * self - s1;
                double testValue = hypot(modValue.imag(), modValue.real()) / std::max((P0 * self).real(), epsilon);
                if (testValue > epsilon) {
                    fit = false;
                }
            }
            mapDst.set(index, newValue);
        }
        int loopSize = matrixSizeArray[depth];
        // 主循环体
        // 所有数据都是从mapSrc读取写入到mapDst，故不会出现读写访问冲突，不加锁
#pragma omp parallel for
        for(int i = 0; i < loopSize; ++i) {
            for (int j = 0; j < loopSize; ++j) {
                auto nInd = std::vector<int>(index);
                nInd.push_back(i);
                nInd.push_back(j);
                doRun(depth + 1, nInd);
            }
        }
    }

public:

    /**
     * 执行求解器
     * @return 若精度满足要求则返回true，若达到迭代上限则返回false
     */
    bool run(){
        // 预设存储的值
        mapDst.copy(mapSrc);
        // 最外层循环为大迭代，不考虑并行关系
        for(int i = 0; i < maxRecurveTimes; ++i){
            fit = true;
            doRun(0, std::vector<int>());
            mapSrc.copy(mapDst);
            if(fit){
                return true;
            }
        }
        return false;
    }
};