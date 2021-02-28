/* Copyright 2021 YesunHuang & guch8017 */
//
// Created by guch8017 on 2021/2/10.
//

#ifndef INCLUDE_MATRIXMAPPER_H_
#define INCLUDE_MATRIXMAPPER_H_
#include <assert.h>
#include <iostream>
#include <vector>
#include <cmath>
#include "./complex.h"

struct TensorMatrix {
    size_t x;
    ayaji::Complex* data;
    explicit TensorMatrix(size_t size) {
        x = size;
        data = new ayaji::Complex[x * x];
    }

    ~TensorMatrix() { delete[] data; }

    inline void set(size_t _x, size_t _y, ayaji::Complex value) {
        data[_x * x + _y] = value;
    }

    inline ayaji::Complex get(size_t _x, size_t _y) {
        return data[_x * x + _y];
    }

    friend inline std::ostream& operator<< (std::ostream &s, const TensorMatrix &matrix) {
        s << '[';
        for (size_t i = 0; i < matrix.x; ++i) {
            auto xoff = matrix.x * i;
            s << '[';
            for (size_t j = 0; j < matrix.x; ++i) {
                s << matrix.data[xoff + j] << ", ";
            }
            s << ']' << std::endl;
        }
        s << ']';
        return s;
    }
};

class MatrixMapper {
private:
    std::vector<int> size;
    std::vector<size_t> off;
    std::vector<size_t> off_op;
    size_t length;
    ayaji::Complex* data;
    bool repaired;
    ayaji::Complex trace;

    void rho(TensorMatrix& _rho,
             int length,
             int* curIndex,
             int sum_x,
             int sum_y) {
        // 注意：此处不要多线程化，使用了大量非线程安全操作，多线程绝对会出bug
        if (length == size.size()) {
            _rho.set(sum_x, sum_y, get(curIndex));
        } else {
            for (int i = 0; i < size[length]; ++i) {
                curIndex[length] = i;
                size_t off_x = sum_x + off_op[length] * i;
                for (int j = 0; j < size[length]; ++j) {
                    curIndex[length + 1] = j;
                    rho(_rho, length + 2, curIndex, off_x,
                        sum_y + off_op[length + 1] * j);
                }
            }
        }
    }

    void pRho(MatrixMapper& map,
              const std::vector<int>& mode,
              int depth,
              int curSize,
              int targetSize,
              int* targetIndex,
              ayaji::Complex value) {
        /*
        if(mode[depth] == 0){
            // 不要的index
            if(curSize == targetSize){
                map.set(targetIndex, value);
            } else{

            }
        }else{
            // 要的index

        }
         */
    }

    inline ayaji::Complex get(const int* index) {
        size_t offset = 0;
        for (int i = 0; i < off.size(); i += 2) {
            // 此处不考虑越界情况
            offset += off[i] * index[i] + off[i + 1] * index[i + 1];
        }
        return get(offset);
    }

    // TODO: 此处仅用double存储阶乘值，可能发生上溢出，如果有需要可改为 unsigned long long 或者 double
    void subRepair(int depth, int offset, int mul, int ind, bool tra) {
        if (depth == size.size()) {
            data[offset] *= ayaji::Complex(sqrt(mul), 0);
            if (tra) {
                trace += data[offset];
            }
            return;
        }
        int factor = 1;
        subRepair(depth + 1, offset, mul, 0, tra && (((depth + 1) & 1) || (ind == 0)));
        for (int i = 1; i < size[depth]; ++i) {
            factor *= i;
            subRepair(depth + 1, offset + i * off[depth], mul * factor, i,
                tra && (((depth + 1) & 1) || (ind == i)));
        }
    }

    inline ayaji::Complex doAvgMoment(int* index,
        const std::vector<int> &order, int depth, int mul) {
        if (depth == size.size()) {
            return ayaji::Complex(mul, 0) * get(index);
        }
        ayaji::Complex ret(0, 0);
        for (int i = 0; i < size[depth]; ++i) {
            index[depth] = i;
            auto pw = std::pow(i, order[depth]);
            ret += doAvgMoment(index, order, depth + 1, mul * pw);
        }
        return ret;
    }

public:
    /**
     * 初始化
     * @param size 矩阵维度列表 注意：size[m]=n 代表 第m维上有 n * n 个元素而非
     * n 个
     */
    explicit MatrixMapper(std::vector<int> size) {
        this->repaired = false;
        this->size.resize(size.size() * 2);
        for (int i = 0; i < size.size(); ++i) {
            this->size[i] = this->size[i + 1] = size[i];
        }
        int length = 1;
        int offset = 1;
        off.resize(size.size() * 2);
        off_op.resize(size.size());
        for (int i = this->size.size() - 1; i >= 0; --i) {
            off[i] = offset;
            offset *= this->size[i];
        }
        length = offset;
        // 预处理偏移2，加速生成张量矩阵的速度
        offset = 1;
        for (int i = size.size() - 1; i >= 0; --i) {
            off_op[i] = offset;
            offset *= size[i];
        }
        this->length = length;
        this->data = new ayaji::Complex[length];
        int i = 0;
        ayaji::Complex init(static_cast<double>(1.0) / length, 0);
        // 据说这样会快
        for (i = 0; i < length / 8; ++i) {
            data[i + 0] = init;
            data[i + 1] = init;
            data[i + 2] = init;
            data[i + 3] = init;
            data[i + 4] = init;
            data[i + 5] = init;
            data[i + 6] = init;
            data[i + 7] = init;
        }
        for (i = i * 8; i < length; ++i) {
            data[i] = init;
        }
    }

    ~MatrixMapper() { delete[] data; }

    /**
     * 给出 l0, r0, ..., lM-1, rM-1
     * 返回对应位置上的元素值
     * 若任意坐标出现越界此方法应返回 0
     **/
    inline ayaji::Complex get(const std::vector<int>& list) {
        size_t offset = 0;
        for (int i = 0; i < list.size(); i += 2) {
            if (size[i] < list[i] || size[i + 1] < list[i + 1])
                return ayaji::Complex(0, 0);
            offset += off[i] * list[i] + off[i + 1] * list[i + 1];
        }
        return get(offset);
    }

    inline ayaji::Complex get(size_t offset) {
        assert(offset < length && "Index out of range");
        return data[offset];
    }

    /**
     * 设定矩阵对应位置的值
     * @param list 坐标列表
     * @param value 将要设定的值
     */
    void set(const std::vector<int>& list, ayaji::Complex value) {
        size_t offset = 0;
        for (int i = 0; i < list.size(); i += 2) {
            if (size[i] < list[i] || size[i + 1] < list[i + 1])
                return;
            offset += off[i] * list[i] + off[i + 1] * list[i + 1];
        }
        set(offset, value);
    }

    inline void set(size_t offset, ayaji::Complex value) {
        assert(offset < length && "Index out of range");
        data[offset] = value;
    }

    // ===============  Output Function  ===================

    /**
     * 约化还原归一
     * 该函数应在计算完成后调用
     * 该函数只应该被调用一次
     */
    void repair() {
        trace = ayaji::Complex(0, 0);
        subRepair(0, 0, 1, -1, true);
        // 据说这样会快
        int i;
        for (i = 0; i < length / 8; ++i) {
            data[i + 0] /= trace;
            data[i + 1] /= trace;
            data[i + 2] /= trace;
            data[i + 3] /= trace;
            data[i + 4] /= trace;
            data[i + 5] /= trace;
            data[i + 6] /= trace;
            data[i + 7] /= trace;
        }
        for (i = i * 8; i < length; ++i) {
            data[i] /= trace;
        }
    }

    TensorMatrix rowRho() {
        if (!repaired) {
            repair();
            repaired = true;
        }
        int len = 1;
        for (int i = 0; i < size.size(); i += 2) {
            len *= size[i];
        }
        TensorMatrix ret(len);
        int* l = reinterpret_cast<int*>(malloc(sizeof(int) * size.size()));
        rho(ret, 0, l, 0, 0);
        free(l);
        return ret;
    }

    void partialRho(std::vector<int> traceMode) { "TODO"; }

    /**
     * 模式均值矩
     * @param order 每个模式需要的矩的次数
     * @return 模式均值矩
     */
    ayaji::Complex avgMoment(const std::vector<int> &order) {
        if (!repaired) {
            repair();
            repaired = true;
        }
        int* index = reinterpret_cast<int*>(malloc(sizeof(int) * size.size()));
        return doAvgMoment(index, order, 0, 1);
    }

    inline size_t getLength(){
        return length;
    }
};

#endif  // INCLUDE_MATRIXMAPPER_H_
