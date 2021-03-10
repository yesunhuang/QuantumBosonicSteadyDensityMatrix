//
// Created by guch8017 on 2021/2/10.
//

#include <vector>
#include <random>
#include <omp.h>
#include "./complex.h"
#include "./MatrixMapper.h"
#include "expression/EpDeriver.h"

#define REC_TIMES 8

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

    void doRun(std::vector<int> index, int depth, int _i) {
        if (depth == matrixSizeArray.size()) {
            std::vector<int> root = index;
            do {
                size_t nextNeighbourIndex = getNextNeighbour();
                std::vector<int> nextNeighbour = getNeighbour(root, nextNeighbourIndex);
                // 更新原值
                dst[_i]->set(root, src->get(root) + alpha *
                                                    (gamma * neighbourCnt * epDeriver.calMasterEP(root) /
                                                     epDeriver.calNeighbourEP(nextNeighbour, nextNeighbourIndex) *
                                                     src->get(nextNeighbour) - src->get(root)));
                root = nextNeighbour;
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
        for (int j = 0; j < maxRecTimes; ++j) {
//#pragma omp parallel for
            for (int i = 0; i < REC_TIMES; ++i) {
                doRun(std::vector<int>(), 0, i);
            }
            // 求平均
//#pragma omp parallel for
            for (int i = 0; i < src->getLength(); ++i) {
                ayaji::Complex res;
                for (int k = 0; k < REC_TIMES; ++k) {
                    res += dst[k]->get(i);
                }
                src->set(i, res / REC_TIMES);
            }
        }
    }

    TDSolver(std::vector<int> matrixSize, EpDeriver expression, double alpha, double gamma, int maxRecTimes) :
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
        uid = std::uniform_int_distribution<unsigned>(0, expression.neighbourIndexes.size() - 1);
    }

    MatrixMapper *getResult() { return src; }
};