//
// Created by guch8017 on 2021/2/10.
//

#ifndef QUANTUM_MATRIXMAPPER_H
#define QUANTUM_MATRIXMAPPER_H
#include <iostream>
#include <vector>


struct TensorMatrix{
    size_t x;
    ayaji::Complex *data;
    TensorMatrix(size_t size){
        x = size;
        data = new ayaji::Complex[x ^ 2];
    }

    ~TensorMatrix(){
        delete [] data;
    }

    inline void set(size_t _x, size_t _y, ayaji::Complex value){
        data[_x * x + _y] = value;
    }

    inline ayaji::Complex get(size_t _x, size_t _y){
        return data[_x * x + _y];
    }
};

class MatrixMapper{
private:
    std::vector<int> size;
    std::vector<size_t> off;
    std::vector<size_t> off_op;
    size_t length;
    ayaji::Complex *data;

    void rho(TensorMatrix& _rho, int length, int * curIndex, int sum_x, int sum_y){
        if (length == size.size()){
            _rho.set(sum_x, sum_y, get(curIndex));
        } else{
            for(int i = 0; i < size[length]; ++i){
                curIndex[length] = i;
                size_t off_x = sum_x + off_op[length] * i;
                for(int j = 0; j < size[length]; ++j){
                    curIndex[length + 1] = j;
                    rho(_rho, length + 2, curIndex, off_x, sum_y + off_op[length + 1] * j);
                }
            }
        }
    }

    inline ayaji::Complex get(const int * index){
        size_t offset = 0;
        for(int i = 0; i < off.size(); i += 2){
            // 此处不考虑越界情况
            offset += off[i] * index[i] + off[i + 1] * index[i + 1];
        }
        return get(offset);
    }

public:
    /**
     * 初始化
     * @param size 矩阵维度列表 注意：size[m]=n 代表 第m维上有 n * n 个元素而非 n 个
     */
    MatrixMapper(std::vector<int> size){
        this->size.resize(size.size() * 2);
        for(int i = 0; i < size.size(); ++i){
            this->size[i] = this->size[i + 1] = size[i];
        }
        int length = 1;
        int offset = 1;
        off.resize(size.size() * 2);
        off_op.resize(size.size());
        for(int i = this->size.size() - 1; i >= 0; --i){
            off[i] = offset;
            offset *= this->size[i];
        }
        // 预处理偏移2，加速生成张量矩阵的速度
        offset = 1;
        for(int i = size.size() - 1; i >= 0; --i){
            off_op[i] = offset;
            offset *= size[i];
        }
        this->length = length;
        this->data = new ayaji::Complex[length];
    }

    ~MatrixMapper(){
        delete [] data;
    }


    /**
     * 给出 l0, r0, ..., lM-1, rM-1
     * 返回对应位置上的元素值
     * 若任意坐标出现越界此方法应返回 0
     **/
    inline ayaji::Complex get(const std::vector<int>& list){
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

    /**
     * 设定矩阵对应位置的值
     * @param list 坐标列表
     * @param value 将要设定的值
     */
    void set(const std::vector<int>& list, ayaji::Complex value){
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

    // ===============  Output Function  ===================

    TensorMatrix rowRho(){
        int len = 1;
        for(int i = 0; i < size.size(); i += 2){
            len *= size[i];
        }
        TensorMatrix ret(len);
        int* l = (int *)malloc(sizeof(int) * size.size());
        rho(ret, 0, l, 0, 0);
        free(l);
        return ret;
    }
};

#endif //QUANTUM_MATRIXMAPPER_H
