#include"p0_start.h"
#include<iostream>
using namespace std;
using namespace scudb;
int main()
{
	int row1 = 0, col1 = 0, row2 = 0, col2 = 0;
	//测试out_of_range
	cout << "——测试out_of_range——" << endl;
	cout << "source.size()=1,RowMatrix(2,2)" << endl;
	RowMatrix<int> R(2, 2);
	vector<int> v(1,1);
	try
	{
		R.FillFrom(v);
	}
	catch (out_of_range& e)
	{
		cout << e.what() << endl;;
	}
	cout << endl;

	//测试矩阵相加
	cout << "——Add——" << endl;
	cout << "rows of Matrix1：";
	cin >> row1;
	cout << "columns of Matrix1：";
	cin >> col1;
	cout << "Matrix1:" << endl;
	int *arr1 = new int[row1 * col1];
	for (int i = 0; i < row1*col1; i++)
	{
		cin >> arr1[i];
	}
	cout << "rows of Matrix2：";
	cin >> row2;
	cout << "columns of Matrix2：";
	cin >> col2;
	cout << "Matrix2:" << endl;
	int* arr2 = new int[row2 * col2];
	for (int i = 0; i < row2 * col2; i++)
	{
		cin >> arr2[i];
	}
	RowMatrixOperations<int> oper;
	RowMatrix<int> R1(row1, col1);
	RowMatrix<int> R11(row2, col2);
	vector<int> v1(arr1, arr1 + row1*col1);
	vector<int> v11(arr2, arr2 + row2 * col2);
	try
	{
		R1.FillFrom(v1);
		R11.FillFrom(v11);
	}
	catch (out_of_range& e)
	{
		cout << e.what() << endl;;
	}
	R1.SetElement(0, 0, 3);
	unique_ptr <RowMatrix<int>> p1(new RowMatrix<int>(R1.GetRowCount(), R11.GetColumnCount()));//智能指针p指向一个矩阵
	p1 = oper.Add(&R1, &R11);
	cout << "Result:" << endl;
	for (int i = 0; i < (*p1).GetRowCount(); i++)
	{
		for (int j = 0; j < (*p1).GetColumnCount(); j++)
		{
			cout << (*p1).GetElement(i, j) << " ";
		}
		cout << endl;
	}
	cout << endl;

	//测试矩阵相乘
	cout << "——Multiply——" << endl;
	row1 = 0, row2 = 0, col1 = 0, col2 = 0;
	cout << "rows of Matrix1：";
	cin >> row1;
	cout << "columns of Matrix2：";
	cin >> col1;
	int* arrM1 = new int[row1 * col1];
	cout << "Matrix1：" << endl;
	for (int i = 0; i < row1 * col1; i++)
	{
		cin >> arrM1[i];
	}
	cout << "rows of Matrix2：";
	cin >> row2;
	cout << "columns of Matrix2：";
	cin >> col2;
	int* arrM2 = new int[row2 * col2];
	cout << "Matrix2：" << endl;
	for (int i = 0; i < row2 * col2; i++)
	{
		cin >> arrM2[i];
	}
	RowMatrix<int> R2(row1, col1);
	RowMatrix<int> R3(row2, col2);
	vector<int> v2(arrM1, arrM1 + row1*col1);
	vector<int> v3(arrM2, arrM2 + row2*col2);
	try
	{
		R2.FillFrom(v2);
		R3.FillFrom(v3);
	}
	catch (out_of_range& e)
	{
		cout << e.what() << endl;
	}
	unique_ptr <RowMatrix<int>> p2(new RowMatrix<int>(R2.GetRowCount(), R3.GetColumnCount()));//智能指针p指向一个矩阵
	p2 = oper.Multiply(&R2, &R3);
	cout << "Result:" << endl;
	for (int i = 0; i < (*p2).GetRowCount(); i++)
	{
		for (int j = 0; j < (*p2).GetColumnCount(); j++)
		{
			cout << (*p2).GetElement(i, j) << " ";
		}
		cout << endl;
	}
	cout << endl;

	//测试矩阵先乘后加
	cout << "——GEMM——" << endl;
	row1 = 0, row2 = 0, col1 = 0, col2 = 0;
	int row3=0, col3=0;
	cout << "rows of Matrix1:";
	cin >> row1;
	cout << "columns of Matrix1:";
	cin >> col1;
	int* arrG1 = new int[row1 * col1];
	cout << "Matrix1:" << endl;
	for (int i = 0; i < row1 * col1; i++)
	{
		cin >> arrG1[i];
	}
	cout << "rows of Matrix2:";
	cin >> row2;
	cout << "columns of Matrix2:";
	cin >> col2;
	int* arrG2 = new int[row2 * col2];
	cout << "Matrix2:" << endl;
	for (int i = 0; i < row2 * col2; i++)
	{
		cin >> arrG2[i];
	}
	cout << "rows of Matrix3:";
	cin >> row3;
	cout << "columns of Matrix3:";
	cin >> col3;
	int* arrG3 = new int[row3 * col3];
	cout << "Matrix3:" << endl;
	for (int i = 0; i < row3 * col3; i++)
	{
		cin >> arrG3[i];
	}
	RowMatrix<int> R4(row1, col1);
	RowMatrix<int> R5(row2, col2);
	RowMatrix<int> R6(row3, col3);
	vector<int> v4(arrG1, arrG1 + row1 * col1);
	vector<int> v5(arrG2, arrG2 + row2 * col2);
	vector<int> v6(arrG3, arrG3 + row3 * col3);
	try
	{
		R4.FillFrom(v4);
		R5.FillFrom(v5);
		R6.FillFrom(v6);
	}
	catch (out_of_range& e)
	{
		cout << e.what() << endl;
	}
	unique_ptr <RowMatrix<int>> p3(new RowMatrix<int>(R6.GetRowCount(), R6.GetColumnCount()));//智能指针p指向一个矩阵
	p3 = oper.GEMM(&R4, &R5, &R6);
	cout << "Result:" << endl;
	for (int i = 0; i < (*p3).GetRowCount(); i++)
	{
		for (int j = 0; j < (*p3).GetColumnCount(); j++)
		{
			cout << (*p3).GetElement(i, j) << " ";
		}
		cout << endl;
	}
	return 0;
}