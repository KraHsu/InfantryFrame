/*
 *           佛曰:  
 *                   写字楼里写字间，写字间里程序员；  
 *                   程序人员写程序，又拿程序换酒钱。  
 *                   酒醒只在网上坐，酒醉还来网下眠；  
 *                   酒醉酒醒日复日，网上网下年复年。  
 *                   但愿老死电脑间，不愿鞠躬老板前；  
 *                   奔驰宝马贵者趣，公交自行程序员。  
 *                   别人笑我忒疯癫，我笑自己命太贱；  
 *                   不见满街漂亮妹，哪个归得程序员？
 * 
 * @Date: 2024-01-16 20:40:50
 * @LastEditors: KraHsu && 1191393280@qq.com
 * @LastEditTime: 2024-01-17 01:50:22
 * Copyright (c) 2024 by KraHsu, All Rights Reserved. 
 */

#pragma once

#include <cmath>
#include "main.h"
#include "infantry_log.h"
#include "initializer_list"

namespace infantry {
    [[noreturn]] void MatrixErrorHandler(const char *msg);

    void MatrixAssert(bool expression, char *msg = nullptr);

    class Matrix {
    private:
        float **_data{};
        uint16_t _rows, _cols;

    public:
        Matrix(int r, int c);

        ~Matrix();

        Matrix &init();

        Matrix(const Matrix &other);

        Matrix &setData(std::initializer_list<std::initializer_list<float>> list);

        Matrix &operator=(const Matrix &other);

        float &operator()(int i, int j);

        Matrix operator+(const Matrix &other);

        Matrix operator-(const Matrix &other);

        Matrix operator*(const Matrix &other);

        Matrix transpose();

        Matrix inverse() const;

        friend LogOut &operator<<(LogOut &out, const Matrix &matrix);
    };
} // infantry
