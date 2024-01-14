/**
 * @file Infantry/algorithm
 * @author CharlesHsu
 * @date 12/24/2023
 */
#pragma once

#include <cmath>
#include "main.h"
#include "infantry_log.h"
#include "initializer_list"

namespace infantry {
    [[noreturn]] void MatrixErrorHandler(char *msg);

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
