#include "stdafx.h"
#include "CppUnitTest.h"
///////////////////////////////////
#include "infotestdata.h"
///////////////////
#include <matdata.h>
//////////////////////////////////////
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace info;
////////////////////////////////////
namespace CPPTestProject
{
	typedef int DataType;
	typedef std::wstring StringType;
	typedef std::valarray<DataType> DataTypeArray;
	typedef MatData<DataType,StringType> MatDataType;
	typedef std::vector<StringType> StringTypeVector;
	///////////////////////////////////////////
	TEST_CLASS(UnitTestMatData)
	{
	public:
		TEST_METHOD(TestMatDataConstructor)
		{
			StringTypeVector rowNames;
			StringTypeVector colNames;
			size_t nCols = 0;
			size_t nRows = 0;
			DataTypeArray data;
			InfoTestData::get(nRows, nCols, data,&rowNames,&colNames);
			Assert::IsTrue(nCols > 2);
			Assert::IsTrue(nRows > 3);
			const size_t nTotal = (size_t)(nCols * nRows);
			Assert::AreEqual(nTotal, data.size());
			MatData<DataType> oMat(nRows, nCols, &data, &rowNames, &colNames);
			std::wstring s;
			oMat.toString(s);
			Logger::WriteMessage(s.c_str());
			std::wstringstream os;
			os << L"INDIVS" << std::endl;
			const size_t nr = oMat.rows();
			const size_t nc = oMat.cols();
			for (size_t i = 0; i < nr; ++i) {
				std::valarray<int> v;
				oMat.row_at(i, v);
				os << i << L"\t";
				write_valarray(os, v);
				os << std::endl;
			}// i
			os << L"COLS" << std::endl;
			for (size_t i = 0; i < nc; ++i) {
				std::valarray<int> v;
				oMat.col_at(i, v);
				os << i << L"\t";
				write_valarray(os, v);
				os << std::endl;
			}// i
			 ////////////////////////////
			os << L"MODE ROW" << std::endl;
			DataMode mode = DataMode::modeRow;
			size_t nn = oMat.get_data_count(mode);
			for (size_t i = 0; i < nn; ++i) {
				std::valarray<int> v;
				oMat.data_at(i, mode, v);
				os << i << L"\t";
				write_valarray(os, v);
				os << std::endl;
			}// i
			os << L"MODE COL" << std::endl;
			mode = DataMode::modeCol;
			nn = oMat.get_data_count(mode);
			for (size_t i = 0; i < nn; ++i) {
				std::valarray<int> v;
				oMat.data_at(i, mode, v);
				os << i << L"\t";
				write_valarray(os, v);
				os << std::endl;
			}// i
			 /////////////////////
			std::wstring sd = os.str();
			Logger::WriteMessage(sd.c_str());
		}// TestMatData
		///////////////////////////////
		TEST_METHOD(TestMatDataDiscretize)
		{
			size_t nCols = 0;
			size_t nRows = 0;
			DataTypeArray data;
			InfoTestData::get(nRows, nCols, data);
			Assert::IsTrue(nCols > 2);
			Assert::IsTrue(nRows > 3);
			const size_t nTotal = (size_t)(nCols * nRows);
			Assert::AreEqual(nTotal, data.size());
			MatData<DataType> oMat(nRows, nCols, &data);
			std::valarray<int> xdata;
			bool bx = oMat.discretize_data(xdata);
			Assert::IsTrue(bx);
			Assert::AreEqual(data.size(), xdata.size());
			MatData<int> xMat(nRows, nCols, &xdata);
			std::wstring s;
			xMat.toString(s);
			Logger::WriteMessage(s.c_str());
		}// TestMatDataDiscretize
		 ///////////////////////////////
		TEST_METHOD(TestMatDataNormalize)
		{
			size_t nCols = 0;
			size_t nRows = 0;
			DataTypeArray data;
			InfoTestData::get(nRows, nCols, data);
			Assert::IsTrue(nCols > 2);
			Assert::IsTrue(nRows > 3);
			const size_t nTotal = (size_t)(nCols * nRows);
			Assert::AreEqual(nTotal, data.size());
			MatData<DataType> oMat(nRows, nCols, &data);
			std::valarray<double> xdata;
			bool bx = oMat.normalize_data(xdata);
			Assert::IsTrue(bx);
			Assert::AreEqual(data.size(), xdata.size());
			MatData<double> xMat(nRows, nCols, &xdata);
			std::wstring s;
			xMat.toString(s);
			Logger::WriteMessage(s.c_str());
		}// TestMatDataNormalize
		TEST_METHOD(TestMatDataRecode)
		{
			size_t nCols = 0;
			size_t nRows = 0;
			DataTypeArray data;
			InfoTestData::get(nRows, nCols, data);
			Assert::IsTrue(nCols > 2);
			Assert::IsTrue(nRows > 3);
			const size_t nTotal = (size_t)(nCols * nRows);
			Assert::AreEqual(nTotal, data.size());
			MatData<DataType> oMat(nRows, nCols, &data);
			std::valarray<int> xdata;
			bool bx = oMat.recode_data(xdata);
			Assert::IsTrue(bx);
			Assert::AreEqual(data.size(), xdata.size());
			MatData<int> xMat(nRows, nCols, &xdata);
			std::wstring s;
			xMat.toString(s);
			Logger::WriteMessage(s.c_str());
			Logger::WriteMessage(L"\n");
			//
			std::valarray<double> ydata;
			bx = oMat.normalize_data(ydata);
			Assert::IsTrue(bx);
			Assert::AreEqual(data.size(), ydata.size());
			MatData<double> xxMat(nRows, nCols, &ydata);
			std::valarray<int> xxdata;
			bx = xxMat.recode_data(xxdata);
			Assert::IsTrue(bx);
			Assert::AreEqual(data.size(), xxdata.size());
			MatData<int> yxMat(nRows, nCols, &xxdata);
			std::wstring ss;
			yxMat.toString(ss);
			Logger::WriteMessage(ss.c_str());
		}// TestMatDataRecode
		 ///////////////////////////////
	};
}