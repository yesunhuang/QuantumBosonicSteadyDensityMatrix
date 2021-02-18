//
// Created by guch8017 on 2021/2/10.
//

#ifndef QUANTUM_MATRIXMAPPER_H
#define QUANTUM_MATRIXMAPPER_H
#include "IMatrixMapper.h"

class MatrixMapper: public IMatrixMapper{
private:
    std::vector<int> size;
    std::vector<size_t> off;
    size_t length;
    ayaji::Complex *data;

public:
    MatrixMapper(std::vector<int> size){
        this->size = size;
        int length = 1;
        int offset = 1;
        off.resize(size.size());
        for(int i = size.size() - 1; i >= 0; --i){
            off[i] = offset;
            offset *= size[i];
        }
        this->length = length;
        this->data = new ayaji::Complex[length];
    }

    ~MatrixMapper(){
        delete [] data;
    }

    inline ayaji::Complex get(std::vector<int> list) override{
        size_t offset = 0;
        for(int i = 0; i < list.size(); i += 2){
            if(size[i] < list[i] || size[i + 1] < list[i + 1]) return ayaji::Complex(0, 0);
            offset += off[i] * list[i] + off[i + 1] * list[i + 1];
        }
        return get(offset);
    }

    inline ayaji::Complex get(size_t offset){
        assert(offset >= length && "Index out of range");
        return data[offset];
    }

    void set(std::vector<int> list, ayaji::Complex value) override{
        size_t offset = 0;
        for(int i = 0; i < list.size(); i += 2){
            if(size[i] < list[i] || size[i + 1] < list[i + 1]) return;
            offset += off[i] * list[i] + off[i + 1] * list[i + 1];
        }
        set(offset, value);
    }

    inline void set(size_t offset, ayaji::Complex value){
        assert(offset >= length && "Index out of range");
        data[offset] = value;
    }
};

#endif //QUANTUM_MATRIXMAPPER_H
