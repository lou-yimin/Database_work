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
        {//���캯��
            this->cols_ = cols;
            this->rows_ = rows;
            linear_ = new T[rows * cols];
        }

        //row��col�Ǿ�������������������±��1��ʼ
        /** The number of rows in the matrix */
        int rows_;
        /** The number of columns in the matrix */
        int cols_;

        /**
         * TODO(P0): Allocate the array in the constructor.                ���캯���з��������
         * TODO(P0): Deallocate the array in the destructor.               �����������ͷ�
         * A flattened array containing the elements of the matrix.
         */
        T* linear_;     //_________________________________________________________________

    public:
        /** @return The number of rows in the matrix */
        virtual int GetRowCount() const = 0; //���麯��

        /** @return The number of columns in the matrix */
        virtual int GetColumnCount() const = 0;  //���麯��

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
        virtual T GetElement(int i, int j) const = 0;  //���麯��

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
        virtual void SetElement(int i, int j, T val) = 0;  //���麯�� 

        /**
         * Fill the elements of the matrix from `source`.
         *
         * Throw OUT_OF_RANGE in the event that `source`
         * does not contain the required number of elements.
         *
         * @param source The source container
         * @throws OUT_OF_RANGE if `source` is incorrect size
         */
        virtual void FillFrom(const std::vector<T>& source) = 0;//source����һ������ֱ��ʵ���� 

        /**
         * Destroy a matrix instance.
         * TODO(P0): Add implementation
         */
         //��������
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
            //��ʼ����ά����
            data_ = new T* [rows];
            for (int i = 0; i < rows; i++)
            {
                data_[i] = new T[cols];
            }
            //ʹ����Щָ��ָ�������������ӦԪ�أ�T *linear_;T **data_;
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
         * Throw OUT_OF_RANGE if either index is out of range.���±�Խ���׳��쳣
         *
         * @param i The row index
         * @param j The column index
         * @return The (i,j)th matrix element
         * @throws OUT_OF_RANGE if either index is out of range
         */
        T GetElement(int i, int j) const override {
            if (i<0 || i>GetRowCount() - 1 || j<0 || j>GetColumnCount())
            {
                //try��catchӦ�ö���main��
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
         * Fill the elements of the matrix from `source`.��source��data
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
        ~RowMatrix() override = default;//Ĭ���������������Զ���������

    private:
        /**
         * A 2D array containing the elements of the matrix in row-major format.
         *
         * TODO(P0):
         * - Allocate the array of row pointers in the constructor.
         * - Use these pointers to point to corresponding elements of the `linear` array.
         * ʹ����Щָ��ָ�������������ӦԪ��
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
         * Return `nullptr` if dimensions mismatch for input matrices.ά�Ȳ�ƥ�䣬����nullptr
         * @param matrixA Input matrix
         * @param matrixB Input matrix
         * @return The result of matrix addition
         * ��ϣ���C��������B
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

            //matrixC ��Ϊ0����
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

            //R���Ǹ�unique_ptr
            std::unique_ptr<RowMatrix<T>>R(new RowMatrix<T>(matrixC->GetRowCount(), matrixC->GetColumnCount()));
            RowMatrixOperations<T> Opera;
            R = Opera.Multiply(matrixA, matrixB);
            return Opera.Add((R.get()), matrixC);      
        }
    };
}  // namespace scudb

