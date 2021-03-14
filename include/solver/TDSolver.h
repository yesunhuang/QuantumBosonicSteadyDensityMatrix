//
// Created by guch8017 on 2021/2/10.
//

#include <vector>
#include <random>
#include <omp.h>
#include <ctime>
#include "./complex.h"
#include "./MatrixMapper.h"
#include "expression/EpDeriver.h"

#define REC_TIMES 32

class TDSolver {
private:
    /**
     * Hyper parameters
     */
    double alpha;
    double gamma;
    int maxRecTimes;

    std::default_random_engine rdEngine;
    std::uniform_int_distribution<unsigned> uid;

    EpDeriver epDeriver;

    std::vector<int> matrixSizeArray;
    std::vector<std::vector<int>> mapper;
    size_t neighbourCnt;

    MatrixMapper *src;
    std::vector<MatrixMapper *> dst;

    inline size_t getNextNeighbour() {
        return uid(rdEngine);
    }

    inline std::vector<int> getNeighbour(std::vector<int> root, int index) {
        auto offset = mapper[index];
        for (int i = 0; i < root.size(); ++i) {
            root[i] += offset[i];
        }
        return root;
    }

    inline bool inbound(const std::vector<int> &index) {
        for (int i = 0; i < index.size(); ++i) {
            if (index[i] < 0 || index[i] >= matrixSizeArray[i / 2]) {
                return false;
            }
        }
        return true;
    }

    static std::vector<int> getOpposite(const std::vector<int> &root) {
        std::vector<int> tmp = std::vector<int>();
        for (int i = 0; i < root.size(); i += 2) {
            tmp.push_back(root[i + 1]);
            tmp.push_back(root[i]);
        }
        return tmp;
    }

    void doRun(const std::vector<int>& index, int depth, int _i) {
        if (depth == matrixSizeArray.size()) {
            std::vector<int> root = index;
            ayaji::Complex rootValue = src->get(root);  // 用于保存更新后的值，防止出现多线程读写冲突，顺带提高性能
            do {
                size_t nextNeighbourIndex = getNextNeighbour();
                std::vector<int> nextNeighbour = getNeighbour(root, nextNeighbourIndex);
                ayaji::Complex result = rootValue + alpha * (gamma * neighbourCnt * epDeriver.calNeighbourEP(nextNeighbour, nextNeighbourIndex) / epDeriver.calMasterEP(root) * src->get(nextNeighbour) - src->get(root));
                dst[_i]->set(root, result);
                ayaji::Complex resultCj = result.conj();
                dst[_i]->set(getOpposite(root), resultCj);
                root = nextNeighbour;
                rootValue = result;
            } while (inbound(root));
        } else {
            for (int i = 0; i < matrixSizeArray[depth]; ++i) {
                auto nIndex = index;
                nIndex.push_back(i);
                nIndex.push_back(i);
                doRun(nIndex, depth + 1, _i);
            }
        }
    }

public:
    void run() {
        for (int j = 0; j < maxRecTimes / REC_TIMES; ++j) {
#pragma omp parallel for
            for (int i = 0; i < REC_TIMES; ++i) {
                doRun(std::vector<int>(), 0, i);
            }
            // 求平均
#pragma omp parallel for
            for (int i = 0; i < src->getLength(); ++i) {
                ayaji::Complex res;
                for (int k = 0; k < REC_TIMES; ++k) {
                    res += dst[k]->get(i);
                }
                src->set(i, res / REC_TIMES);
            }
        }
    }

    TDSolver(const std::vector<int> &matrixSize, const EpDeriver &expression, double alpha, double gamma,
             int maxRecTimes) :
            alpha(alpha),
            gamma(gamma),
            maxRecTimes(maxRecTimes),
            matrixSizeArray(matrixSize),
            epDeriver(expression),
            mapper(epDeriver.neighbourIndexes),
            neighbourCnt(epDeriver.neighbourIndexes.size()) {
        src = new MatrixMapper(matrixSize);
        dst.resize(REC_TIMES);
        for (int i = 0; i < REC_TIMES; ++i) {
            dst[i] = new MatrixMapper(matrixSize);
        }
        rdEngine.seed(time(nullptr));
        uid = std::uniform_int_distribution<unsigned>(0, expression.neighbourIndexes.size() - 1);
    }

    MatrixMapper *getResult() { return src; }
};