/*
 * ......................................&&.........................
 * ....................................&&&..........................
 * .................................&&&&............................
 * ...............................&&&&..............................
 * .............................&&&&&&..............................
 * ...........................&&&&&&....&&&..&&&&&&&&&&&&&&&........
 * ..................&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&..............
 * ................&...&&&&&&&&&&&&&&&&&&&&&&&&&&&&.................
 * .......................&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&.........
 * ...................&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&...............
 * ..................&&&   &&&&&&&&&&&&&&&&&&&&&&&&&&&&&............
 * ...............&&&&&@  &&&&&&&&&&..&&&&&&&&&&&&&&&&&&&...........
 * ..............&&&&&&&&&&&&&&&.&&....&&&&&&&&&&&&&..&&&&&.........
 * ..........&&&&&&&&&&&&&&&&&&...&.....&&&&&&&&&&&&&...&&&&........
 * ........&&&&&&&&&&&&&&&&&&&.........&&&&&&&&&&&&&&&....&&&.......
 * .......&&&&&&&&.....................&&&&&&&&&&&&&&&&.....&&......
 * ........&&&&&.....................&&&&&&&&&&&&&&&&&&.............
 * ..........&...................&&&&&&&&&&&&&&&&&&&&&&&............
 * ................&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&............
 * ..................&&&&&&&&&&&&&&&&&&&&&&&&&&&&..&&&&&............
 * ..............&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&....&&&&&............
 * ...........&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&......&&&&............
 * .........&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&.........&&&&............
 * .......&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&...........&&&&............
 * ......&&&&&&&&&&&&&&&&&&&...&&&&&&...............&&&.............
 * .....&&&&&&&&&&&&&&&&............................&&..............
 * ....&&&&&&&&&&&&&&&.................&&...........................
 * ...&&&&&&&&&&&&&&&.....................&&&&......................
 * ...&&&&&&&&&&.&&&........................&&&&&...................
 * ..&&&&&&&&&&&..&&..........................&&&&&&&...............
 * ..&&&&&&&&&&&&...&............&&&.....&&&&...&&&&&&&.............
 * ..&&&&&&&&&&&&&.................&&&.....&&&&&&&&&&&&&&...........
 * ..&&&&&&&&&&&&&&&&..............&&&&&&&&&&&&&&&&&&&&&&&&.........
 * ..&&.&&&&&&&&&&&&&&&&&.........&&&&&&&&&&&&&&&&&&&&&&&&&&&.......
 * ...&&..&&&&&&&&&&&&.........&&&&&&&&&&&&&&&&...&&&&&&&&&&&&......
 * ....&..&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&...........&&&&&&&&.....
 * .......&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&..............&&&&&&&....
 * .......&&&&&.&&&&&&&&&&&&&&&&&&..&&&&&&&&...&..........&&&&&&....
 * ........&&&.....&&&&&&&&&&&&&.....&&&&&&&&&&...........&..&&&&...
 * .......&&&........&&&.&&&&&&&&&.....&&&&&.................&&&&...
 * .......&&&...............&&&&&&&.......&&&&&&&&............&&&...
 * ........&&...................&&&&&&.........................&&&..
 * .........&.....................&&&&........................&&....
 * ...............................&&&.......................&&......
 * ................................&&......................&&.......
 * .................................&&..............................
 * ..................................&..............................
 * 
 * @Date: 2024-01-16 20:40:50
 * @LastEditors: KraHsu && 1191393280@qq.com
 * @LastEditTime: 2024-01-17 01:51:55
 * Copyright (c) 2024 by KraHsu, All Rights Reserved. 
 */

#include "infantry_matrix.h"

namespace infantry {
    [[noreturn]] void MatrixErrorHandler(const char *msg) {
        logErrorWithTag("Matrix", "%s", msg);
        while (true) {
            //TODO: MatrixErrorHandler
        }
    }

    void MatrixAssert(bool expression, char *msg) {
        if (!expression) {
            MatrixErrorHandler(msg);
        }
    }

    LogOut &operator<<(LogOut &out, const Matrix &matrix) {
        for (int i = 0; i < matrix._rows; ++i) {
            for (int j = 0; j < matrix._cols; ++j) {
                out << matrix._data[i][j] << " ";
            }
            out << endl;
        }
        return out;
    }

    Matrix::Matrix(int r, int c) : _rows(r), _cols(c) {}

    Matrix &Matrix::init() {
        if (_data == nullptr) {
            _data = new float *[_rows];
            for (int i = 0; i < _rows; ++i) {
                _data[i] = new float[_cols]();
            }
        }
        return *this;
    }

    Matrix::~Matrix() {
        for (int i = 0; i < _rows; ++i) {
            delete[] _data[i];
        }
        delete[] _data;
    }

    Matrix::Matrix(const Matrix &other) : _rows(other._rows), _cols(other._cols) {
        _data = new float *[_rows];
        for (int i = 0; i < _rows; ++i) {
            _data[i] = new float[_cols];
            for (int j = 0; j < _cols; ++j) {
                _data[i][j] = other._data[i][j];
            }
        }
    }

    Matrix &Matrix::operator=(const Matrix &other) {
        if (this != &other) {
            for (int i = 0; i < _rows; ++i) {
                delete[] _data[i];
            }
            delete[] _data;

            _rows = other._rows;
            _cols = other._cols;
            _data = new float *[_rows];
            for (int i = 0; i < _rows; ++i) {
                _data[i] = new float[_cols];
                for (int j = 0; j < _cols; ++j) {
                    _data[i][j] = other._data[i][j];
                }
            }
        }
        return *this;
    }

    float &Matrix::operator()(int i, int j) {
        MatrixAssert(i >= 0 && i < _rows && j >= 0 && j < _cols);
        return _data[i][j];
    }

    Matrix Matrix::operator+(const Matrix &other) {
        MatrixAssert(_rows == other._rows && _cols == other._cols);

        Matrix result(_rows, _cols);
        result.init();

        for (int i = 0; i < _rows; ++i) {
            for (int j = 0; j < _cols; ++j) {
                result(i, j) = _data[i][j] + other._data[i][j];
            }
        }
        return result;
    }

    Matrix Matrix::operator-(const Matrix &other) {
        MatrixAssert(_rows == other._rows && _cols == other._cols);

        Matrix result(_rows, _cols);
        result.init();

        for (int i = 0; i < _rows; ++i) {
            for (int j = 0; j < _cols; ++j) {
                result(i, j) = _data[i][j] - other._data[i][j];
            }
        }
        return result;
    }

    Matrix Matrix::operator*(const Matrix &other) {
        MatrixAssert(_cols == other._rows);

        Matrix result(_rows, other._cols);
        result.init();

        for (int i = 0; i < _rows; ++i) {
            for (int j = 0; j < other._cols; ++j) {
                for (int k = 0; k < _cols; ++k) {
                    result(i, j) += _data[i][k] * other._data[k][j];
                }
            }
        }
        return result;
    }

    Matrix Matrix::transpose() {

        Matrix transposed(_cols, _rows);
        transposed.init();

        for (int i = 0; i < _rows; ++i) {
            for (int j = 0; j < _cols; ++j) {
                transposed(j, i) = _data[i][j];
            }
        }
        return transposed;
    }

    Matrix Matrix::inverse() const {
        MatrixAssert(_rows == _cols);

        Matrix result(_rows, _cols * 2);
        result.init();

        // 初始化增广矩阵
        for (int i = 0; i < _rows; ++i) {
            for (int j = 0; j < _cols; ++j) {
                result(i, j) = _data[i][j];
                result(i, j + _cols) = (i == j) ? 1.0 : 0.0;
            }
        }

        // 高斯消元
        for (int i = 0; i < _rows; ++i) {
            // 使对角线上的元素为1
            float diag = result(i, i);
            if (fabs(diag) < 1e-10) {
                MatrixErrorHandler("Matrix is singular and cannot be inverted.");
            }
            for (int j = 0; j < 2 * _cols; ++j) {
                result(i, j) /= diag;
            }

            // 使其他行的当前列元素为0
            for (int k = 0; k < _rows; ++k) {
                if (k != i) {
                    float factor = result(k, i);
                    for (int j = 0; j < 2 * _cols; ++j) {
                        result(k, j) -= factor * result(i, j);
                    }
                }
            }
        }

        Matrix inv(_rows, _cols);
        inv.init();

        for (int i = 0; i < _rows; ++i) {
            for (int j = 0; j < _cols; ++j) {
                inv(i, j) = result(i, j + _cols);
            }
        }

        return inv;
    }

    Matrix &Matrix::setData(std::initializer_list<std::initializer_list<float>> list) {
        MatrixAssert(list.size() == _rows);
        auto rowIt = list.begin();
        for (int i = 0; i < _rows; ++i, ++rowIt) {
            MatrixAssert(rowIt->size() == _cols);
            auto colIt = rowIt->begin();
            for (int j = 0; j < _cols; ++j, ++colIt) {
                _data[i][j] = *colIt;
            }
        }
        return *this;
    }
} // infantry
