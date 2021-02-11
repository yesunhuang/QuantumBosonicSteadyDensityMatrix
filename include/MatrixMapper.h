//
// Created by guch8017 on 2021/2/10.
//

#ifndef QUANTUM_MATRIXMAPPER_H
#define QUANTUM_MATRIXMAPPER_H
#include "IMatrixMapper.h"

class MatrixMapper: public IMatrixMapper{
public:
    std::complex<double> get(std::vector<int> list) override{
        return 0;
    }

    void set(std::vector<int> list, std::complex<double> value) override{
        ;
    }

    void copy(IMatrixMapper &matrix) override{
        ;
    }
};

#endif //QUANTUM_MATRIXMAPPER_H
