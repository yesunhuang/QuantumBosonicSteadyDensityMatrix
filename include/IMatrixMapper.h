//
// Created by guch8017 on 2021/2/9.
//

#ifndef MATRIX_MAPPER_H
#define MATRIX_MAPPER_H

#include <vector>
#include "complex.h"

class IMatrixMapper{
public:
    IMatrixMapper(){
        // TODO
    }

    IMatrixMapper(IMatrixMapper& old){
        // TODO
    }

    /**
     * 给出 l0, r0, ..., lM-1, rM-1
     * 返回对应位置上的元素值
     * 若存在变换此方法应自动返回变换后的位置所代表的值
     * 若任意坐标出现越界此方法应返回 0
     **/
    virtual ayaji::Complex get(std::vector<int> list) = 0;


    /**
     * 设定矩阵对应位置的值，需考虑坐标变换
     * @param list 坐标列表
     * @param value 将要设定的值
     */
    virtual void set(std::vector<int> list, ayaji::Complex value) = 0;


    /**
     * 将当前函数内存储的数据复制到目标中（用于多线程处理）
     * @param matrix 目标矩阵
     */
    virtual void copy(IMatrixMapper& matrix) = 0;

protected:
    /**
     * PlaceHolder
     * 存储数据的地方，可以换成别的
     */
    void* pointer;
};

#endif
