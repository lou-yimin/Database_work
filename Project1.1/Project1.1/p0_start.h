#pragma once
//===----------------------------------------------------------------------===//
//
//
//
// p0_starter.h
//
// Identification: src/include/primer/p0_starter.h
//
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <stdexcept>
#include <vector>

//#include "common/exception.h"

namespace scudb {

    /**
     * The Matrix type defines a common
     * interface for matrix operations.
     */
    template <typename T>
    class Matrix {
    protected:
        /**
         * TODO(P0): Add implementation
         *
         * Construct a new Matrix instance.
         * @param rows The number of rows
         * @param cols The number of columns
         *
         */
        Matrix(int rows, int cols)
        {//构造函数
            this->cols_ = cols;
            this->rows_ = rows;
            linear_ = new T[rows * cols];
        }

        //row与col是矩阵的行数或者列数，下标从1开始
        /** The number of rows in the matrix */
        int rows_;
        /** The number of columns in the matrix */
        int cols_;

        /**
         * TODO(P0): Allocate the array in the constructor.                构造函数中分配矩数组
         * TODO(P0): Deallocate the array in the destructor.               析构函数中释放
         * A flattened array containing the elements of the matrix.
         */
        T* linear_;     //_________________________________________________________________

    public:
        /** @return The number of rows in the matrix */
        virtual int GetRowCount() const = 0; //纯虚函数

        /** @return The number of columns in the matrix */
        virtual int GetColumnCount() const = 0;  //纯虚函数

        /**
         * Get the (i,j)th matrix element.
         *
         * Throw OUT_OF_RANGE if either index is out of range.
         *
         * @param i The row index
         * @param j The column index
         * @return The (i,j)th matrix element
         * @throws OUT_OF_RANGE if either index is out of range
         */
        virtual T GetElement(int i, int j) const = 0;  //纯虚函数

        /**
         * Set the (i,j)th matrix element.
         *
         * Throw OUT_OF_RANGE if either index is out of range.
         *
         * @param i The row index
         * @param j The column index
         * @param val The value to insert
         * @throws OUT_OF_RANGE if either index is out of range
         */
        virtual void SetElement(int i, int j, T val) = 0;  //纯虚函数 

        /**
         * Fill the elements of the matrix from `source`.
         *
         * Throw OUT_OF_RANGE in the event that `source`
         * does not contain the required number of elements.
         *
         * @param source The source container
         * @throws OUT_OF_RANGE if `source` is incorrect size
         */
        virtual void FillFrom(const std::vector<T>& source) = 0;//source是另一个矩阵，直接实例化 

        /**
         * Destroy a matrix instance.
         * TODO(P0): Add implementation
         */
         //析构函数
        virtual ~Matrix() = default;
    };

    /**
     * The RowMatrix type is a concrete matrix implementation.
     * It implements the interface defined by the Matrix type.
     */
    template <typename T>
    class RowMatrix : public Matrix<T> {
    public:
        /**
         * TODO(P0): Add implementation
         *
         * Construct a new RowMatrix instance.
         * @param rows The number of rows
         * @param cols The number of columns
         */
        RowMatrix(int rows, int cols) : Matrix<T>(rows, cols)
        {
            //初始化二维数组
            data_ = new T* [rows];
            for (int i = 0; i < rows; i++)
            {
                data_[i] = new T[cols];
            }
            //使用这些指针指向线性数组的相应元素，T *linear_;T **data_;
            for (int i = 0; i < rows * cols; i++)
            {
                data_[i / cols][i % cols] = this->linear_[i];
            }
        }

        /**
         * TODO(P0): Add implementation
         * @return The number of rows in the matrix
         */
        int GetRowCount() const override
        {
            return this->rows_;
        }

        /**
         * TODO(P0): Add implementation
         * @return The number of columns in the matrix
         */
        int GetColumnCount() const override
        {
            return this->cols_;
        }

        /**
         * TODO(P0): Add implementation
         *
         * Get the (i,j)th matrix element.
         *
         * Throw OUT_OF_RANGE if either index is out of range.若下标越界抛出异常
         *
         * @param i The row index
         * @param j The column index
         * @return The (i,j)th matrix element
         * @throws OUT_OF_RANGE if either index is out of range
         */
        T GetElement(int i, int j) const override {
            if (i<0 || i>GetRowCount() - 1 || j<0 || j>GetColumnCount())
            {
                //try和catch应该都在main中
                //throw NotImplementedException( "RowMatrix::GetElement() not implemented." );
                throw std::out_of_range("OUT_OF_RANGE");
            }
            return data_[i][j];
        }

        /**
         * Set the (i,j)th matrix element.
         *
         * Throw OUT_OF_RANGE if either index is out of range.
         *
         * @param i The row index
         * @param j The column index
         * @param val The value to insert
         * @throws OUT_OF_RANGE if either index is out of range
         */
        void SetElement(int i, int j, T val) override
        {
            if (i<0 || i>GetRowCount() - 1 || j<0 || j>GetColumnCount())
            {
                //throw NotImplementedException( "RowMatrix::SetElement() not implemented." );
                throw std::out_of_range("OUT_OF_RANGE");
            }
            data_[i][j] = val;
        }

        /**
         * TODO(P0): Add implementation
         *
         * Fill the elements of the matrix from `source`.用source填data
         *
         * Throw OUT_OF_RANGE in the event that `source`
         * does not contain the required number of elements.
         *
         * @param source The source container
         * @throws OUT_OF_RANGE if `source` is incorrect size
         */
        void FillFrom(const std::vector<T>& source) override {
            if (source.size() != this->rows_ * this->cols_)
            {
                //throw NotImplementedException( "RowMatrix::FillFrom() not implemented." );
                throw std::out_of_range("OUT_OF_RANGE");
            }
            for (int i = 0; i < this->rows_ * this->cols_; i++)
            {
                data_[i / this->cols_][i % this->cols_] = source[i];
            }

        }

        /**
         * TODO(P0): Add implementation
         *
         * Destroy a RowMatrix instance.
         */
        ~RowMatrix() override = default;//默认析构函数，会自动销毁数组

    private:
        /**
         * A 2D array containing the elements of the matrix in row-major format.
         *
         * TODO(P0):
         * - Allocate the array of row pointers in the constructor.
         * - Use these pointers to point to corresponding elements of the `linear` array.
         * 使用这些指针指向线性数组的相应元素
         * - Don't forget to deallocate the array in the destructor.
         */
        T** data_;  //______________________________________________________________________________
    };

    /**
     * The RowMatrixOperations class defines operations
     * that may be performed on instances of `RowMatrix`.
     */
    template <typename T>
    class RowMatrixOperations {
    public:
        /**
         * Compute (`matrixA` + `matrixB`) and return the result.
         * Return `nullptr` if dimensions mismatch for input matrices.维度不匹配，返回nullptr
         * @param matrixA Input matrix
         * @param matrixB Input matrix
         * @return The result of matrix addition
         * 组合，类C内用了类B
         */
        static std::unique_ptr<RowMatrix<T>> Add(const RowMatrix<T>* matrixA, const RowMatrix<T>* matrixB) {
            // TODO(P0): Add implementation
            if (matrixA->GetRowCount() != matrixB->GetRowCount() || matrixA->GetColumnCount() != matrixB->GetColumnCount())
            {
                return std::unique_ptr<RowMatrix<T>>(nullptr);
            }

            RowMatrix<T>* C = new RowMatrix<T>(matrixA->GetRowCount(), matrixA->GetColumnCount());
            for (int i = 0; i < (*C).GetRowCount(); i++)
            {
                for (int j = 0; j < (*C).GetColumnCount(); j++)
                {
                    T val = matrixA->GetElement(i, j) + matrixB->GetElement(i, j);
                    (*C).SetElement(i, j, val);
                }
            }
            return std::unique_ptr<RowMatrix<T>>(C);
        }

        /**
         * Compute the matrix multiplication (`matrixA` * `matrixB` and return the result.
         * Return `nullptr` if dimensions mismatch for input matrices.
         * @param matrixA Input matrix
         * @param matrixB Input matrix
         * @return The result of matrix multiplication
         */
        static std::unique_ptr<RowMatrix<T>> Multiply(const RowMatrix<T>* matrixA, const RowMatrix<T>* matrixB) {
            // TODO(P0): Add implementation
            if (matrixA->GetColumnCount() != matrixB->GetRowCount())
            {
                return std::unique_ptr<RowMatrix<T>>(nullptr);
            }
            RowMatrix<T>* C = new RowMatrix<T>(matrixA->GetRowCount(), matrixB->GetColumnCount());

            //matrixC 设为0矩阵
            for (int i = 0; i < (*C).GetRowCount(); i++)
            {
                for (int j = 0; j < (*C).GetColumnCount(); j++)
                {
                    (*C).SetElement(i, j, 0);
                }
            }
            for (int i = 0; i < matrixA->GetRowCount(); i++)
            {
                for (int j = 0; j < matrixB->GetColumnCount(); j++)
                {
                    for (int m = 0; m < matrixA->GetColumnCount(); m++)
                    {
                        //c[i][j] = c[i][j] + a[i][m] * b[m][j];
                        T val = (*C).GetElement(i, j) + matrixA->GetElement(i, m) * matrixB->GetElement(m, j);
                        (*C).SetElement(i, j, val);
                    }
                }
            }
            return std::unique_ptr<RowMatrix<T>>(C);
        }

        /**
         * Simplified General Matrix Multiply operation. Compute (`matrixA` * `matrixB` + `matrixC`).
         * Return `nullptr` if dimensions mismatch for input matrices.
         * @param matrixA Input matrix
         * @param matrixB Input matrix
         * @param matrixC Input matrix
         * @return The result of general matrix multiply
         */
        static std::unique_ptr<RowMatrix<T>> GEMM(const RowMatrix<T>* matrixA, const RowMatrix<T>* matrixB,
            const RowMatrix<T>* matrixC) {
            // TODO(P0): Add implementation
            if (matrixA->GetColumnCount() != matrixB->GetRowCount() || matrixA->GetRowCount() != matrixC->GetRowCount() || matrixB->GetColumnCount() != matrixC->GetColumnCount())
            {
                return std::unique_ptr<RowMatrix<T>>(nullptr);
            }

            //R得是个unique_ptr
            std::unique_ptr<RowMatrix<T>>R(new RowMatrix<T>(matrixC->GetRowCount(), matrixC->GetColumnCount()));
            RowMatrixOperations<T> Opera;
            R = Opera.Multiply(matrixA, matrixB);
            return Opera.Add((R.get()), matrixC);      
        }
    };
}  // namespace scudb

