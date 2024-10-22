/* Copyright 2021 YesunHuang & guch8017 */
//
// Created by guch8017 on 2021/2/10.
//

#ifndef INCLUDE_MATRIXMAPPER_H_
#define INCLUDE_MATRIXMAPPER_H_

#include <cassert>
#include <iostream>
#include <utility>
#include <vector>
#include <cmath>
#include "./complex.h"

struct TensorMatrix {
    size_t x;
    ayaji::Complex *data;

    explicit TensorMatrix(size_t size) {
        x = size;
        data = new ayaji::Complex[x * x];
    }

    ~TensorMatrix() { delete[] data; }

    inline void set(size_t _x, size_t _y, const ayaji::Complex &value) const {
        data[_x * x + _y] = value;
    }

    inline ayaji::Complex get(size_t _x, size_t _y) const {
        return data[_x * x + _y];
    }

    friend inline std::ostream &operator<<(std::ostream &s, const TensorMatrix &matrix) {
        s << '[';
        for (size_t i = 0; i < matrix.x; ++i) {
            auto xoff = matrix.x * i;
            s << '[';
            for (size_t j = 0; j < matrix.x; ++j) {
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
    ayaji::Complex *data;
    bool repaired;
    ayaji::Complex trace;

    /**
     * 生成张量的辅助函数
     * @param _rho
     * @param _length
     * @param curIndex
     * @param sum_x
     * @param sum_y
     */
    void rho(TensorMatrix *_rho,
             int _length,
             int *curIndex,
             int sum_x,
             int sum_y) {
        // 注意：此处不要多线程化，使用了大量非线程安全操作，多线程绝对会出bug
        if (_length == size.size()) {
            _rho->set(sum_x, sum_y, get(curIndex));
        } else {
            for (int i = 0; i < size[_length]; ++i) {
                curIndex[_length] = i;
                size_t off_x = sum_x + off_op[_length / 2] * i;
                for (int j = 0; j < size[_length]; ++j) {
                    curIndex[_length + 1] = j;
                    rho(_rho, _length + 2, curIndex, off_x,
                        sum_y + off_op[_length / 2] * j);
                }
            }
        }
    }

    /**
     * 还原归一化条件
     * @param depth
     * @param offset
     * @param mul
     * @param ind
     * @param tra
     */
    // TODO: 此处仅用double存储阶乘值，可能发生上溢出，如果有需要可改为 unsigned long long 或者 double
    void subRepair(int depth, int offset, unsigned long long mul, int ind, bool tra) {
        if (depth == size.size()) {
            data[offset] *= ayaji::Complex(sqrt(mul), 0);
            if (tra) {
                trace += data[offset];
            }
            return;
        }
        unsigned long long factor = 1;
        subRepair(depth + 1, offset, mul, 0, tra && (depth % 2 == 0 || (ind == 0)));
        for (int i = 1; i < size[depth]; ++i) {
            factor *= i;
            subRepair(depth + 1, offset + i * off[depth], mul * factor, i,
                      tra && (depth % 2 == 0 || (ind == i)));
        }
    }

    /**
     * [Deparated] 暴力还原
     * @param depth
     * @param index
     */
    void subRepairTest(int depth, std::vector<int> index) {
        if (depth == size.size()) {
            int d = 1;
            for (int i : index) {
                int j = 1;
                for (int k = 2; k < i + 1; ++k) {
                    j *= k;
                }
                d *= j;
            }
            set(index, get(index) * ayaji::Complex(sqrt(d), 0), false);
            bool _trace = true;
            for (int i = 0; i < index.size(); i += 2) {
                if (index[i] != index[i + 1]) {
                    _trace = false;
                    break;
                }
            }
            if (_trace) {
                this->trace += get(index);
            }
        } else {
            for (int i = 0; i < size[depth]; ++i) {
                std::vector<int> ind2 = index;
                ind2.push_back(i);
                subRepairTest(depth + 1, ind2);
            }
        }
    }

    /**
     * 计算模式均值矩的辅助函数
     * @param index
     * @param order
     * @param depth
     * @param mul
     * @return
     */
    inline ayaji::Complex doAvgMoment(int *index,
                                      const std::vector<int> &order, int depth, int mul) {
        if (depth == size.size()) {
            return mul * get(index);
        }
        ayaji::Complex ret(0, 0);
        for (int i = 0; i < size[depth]; ++i) {
            index[depth] = i;
            index[depth + 1] = i;
            auto pw = std::pow(i, order[depth / 2]);
            ret += doAvgMoment(index, order, depth + 2, mul * pw);
        }
        return ret;
    }

    /**
     * 备用的初始化函数，仅沿对角线初始
     * @param index 外部调用应传入一空的vector
     * @param value 传入 1 / length
     */
    void _init(std::vector<int> index, ayaji::Complex value) {
        if (index.size() == size.size()) {
            set(index, value, false);
        } else {
            for (int i = 0; i < size[index.size()]; ++i) {
                auto ind2 = index;
                ind2.push_back(i);
                ind2.push_back(i);
                _init(ind2, value);
            }
        }
    }

public:
    /**
     * 初始化
     * @param size 矩阵维度列表 注意：size[m]=n 代表 第m维上有 n * n 个元素而非
     * n 个
     */
    explicit MatrixMapper(const std::vector<int> &size) {
        this->repaired = false;
        this->size.resize(size.size() * 2);
        for (int i = 0; i < size.size() * 2; i += 2) {
            this->size[i] = this->size[i + 1] = size[i / 2];
        }
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
        this->data = new ayaji::Complex[length];
        int i = 0;
        ayaji::Complex init(static_cast<double>(1.0) / offset, 0);

        /*
        for (i = 0; i < length; ++i) {
            data[i] = init;
        }
        */

        data[0] = ayaji::Complex(1, 0);
        //_init(std::vector<int>(), init);
    }

    ~MatrixMapper() { delete[] data; }

    /**
     * 给出 l0, r0, ..., lM-1, rM-1
     * 返回对应位置上的元素值
     * 若任意坐标出现越界此方法应返回 0
     **/
    inline ayaji::Complex get(const std::vector<int> &list) {
        size_t offset = 0;
        for (int i = 0; i < list.size(); i += 2) {
            if (size[i] <= list[i] || size[i + 1] <= list[i + 1]
                || list[i] < 0 || list[i + 1] < 0)
                return ayaji::Complex(0, 0);
            offset += off[i] * list[i] + off[i + 1] * list[i + 1];
        }
        return get(offset);
    }

    inline ayaji::Complex get(int offset, const std::vector<int> &list) {
        for (int i = 0; i < list.size(); i += 2) {
            if (size[i] <= list[i] || size[i + 1] <= list[i + 1] || list[i] < 0 || list[i + 1] < 0)
                return ayaji::Complex();
        }
        return get(offset);
    }

    /**
     * 由坐标偏移获取偏移
     * @param offset
     * @return
     */
    inline ayaji::Complex get(size_t offset) {
        //assert(offset < length && "Index out of range");
        return data[offset];
    }

    /**
     * 设定矩阵对应位置的值
     * @param list 坐标列表
     * @param value 将要设定的值
     */
    inline void set(const std::vector<int> &list, ayaji::Complex value) {
        set(list, std::move(value), true);
    }

    void set(const std::vector<int> &list, ayaji::Complex value, bool checkZero) {
        size_t offset = 0;
        for (int i = 0; i < list.size(); i += 2) {
            if (size[i] <= list[i] || size[i + 1] <= list[i + 1]
                || list[i] < 0 || list[i + 1] < 0)
                return;
            offset += off[i] * list[i] + off[i + 1] * list[i + 1];
        }
        if (!offset && checkZero) {
            return;
        }
        set(offset, value);
    }

    inline void set(size_t offset, ayaji::Complex value) {
        //assert(offset < length && "Index out of range");
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
        int i;
        for (i = 0; i < length; ++i) {
            data[i] /= trace;
        }
    }

    TensorMatrix *rowRho() {
        if (!repaired) {
            repair();
            repaired = true;
        }
        int len = 1;
        for (int i = 0; i < size.size(); i += 2) {
            len *= size[i];
        }
        auto *ret = new TensorMatrix(len);
        int *l = reinterpret_cast<int *>(malloc(sizeof(int) * size.size()));
        rho(ret, 0, l, 0, 0);
        free(l);
        return ret;
    }

    void partialRho(std::vector<int> traceMode) {
        // TODO
    }

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
        int *index = reinterpret_cast<int *>(malloc(sizeof(int) * size.size()));
        auto ret = doAvgMoment(index, order, 0, 1);
        free(index);
        return ret;
    }

    inline size_t getLength() const {
        return length;
    }

    /**
     * 根据给出的坐标列表返回偏移值
     * @param list 坐标
     * @return 偏移值
     */
    inline int getOffset(const std::vector<int> &list) {
        int offset = 0;
        for (int i = 0; i < list.size(); i += 2) {
            offset += off[i] * list[i] + off[i + 1] * list[i + 1];
        }
        return offset;
    }

/**
 * 获取对应坐标的值
 * @param index 坐标列表
 * @return
 */
    inline ayaji::Complex get(const int *index) {
        size_t offset = 0;
        for (int i = 0; i < off.size(); i += 2) {
            // 此处不考虑越界情况
            offset += off[i] * index[i] + off[i + 1] * index[i + 1];
        }
        return get(offset);
    }
};

#endif  // INCLUDE_MATRIXMAPPER_H_
