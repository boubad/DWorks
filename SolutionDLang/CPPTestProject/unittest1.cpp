#include "stdafx.h"
#include "CppUnitTest.h"
///////////////////////////////////
#include <cstdlib>
////////////////////////////////////
#include <utils.h>
#include <indiv.h>
#include <matdata.h>
#include <cluster.h>
#include <clusterize.h>
//////////////////////////////////////
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace info;
////////////////////////////////////
namespace CPPTestProject
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		static void gener_data(const size_t n, std::valarray<int> &data) {
			Assert::IsTrue(n > 0);
			data.resize(n);
			for (size_t i = 0; i < n; ++i) {
				data[i] = ::rand() & 100;
			}// i
		}// gener_data
	public:
		
		TEST_METHOD(TestIndiv)
		{
			int aIndex = 100;
			int aIndex2 = 150;
			size_t n = 30;
			std::valarray<int> data;
			std::valarray<int> data2;
			gener_data(n, data);
			gener_data(n, data2);
			Indiv<int, int> oInd1(aIndex, data);
			Indiv<int, int> oInd2(aIndex2, data2);
			std::wstring s, s2;
			oInd1.toString(s);
			oInd2.toString(s2);
			Logger::WriteMessage(s.c_str());
			Logger::WriteMessage(s2.c_str());
			int dist = 0;
			oInd1.distance(oInd2, dist);
			std::wstringstream os;
			os << L"distance: " << dist;
			std::wstring sd = os.str();
			Logger::WriteMessage(sd.c_str());
		}// TestIndiv
		TEST_METHOD(TestMatData)
		{
			size_t n = 30;
			size_t nCols = 3;
			size_t nRows = 10;
			std::valarray<int> data;
			gener_data(n, data);
			MatData<int> oMat(nRows, nCols, &data);
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
		TEST_METHOD(TestCluster)
		{
			int aIndex = 100;
			size_t n = 5;
			Cluster<int, int> c(aIndex);
			size_t nb = 10;
			for (size_t i = 0; i < nb; ++i) {
				std::valarray<int> v;
				gener_data(n, v);
				c.add((int)i, v);
			}// i
			c.update_center();
			std::wstringstream os;
			os << c;
			std::wstring sd = os.str();
			Logger::WriteMessage(sd.c_str());
		}// TestCluster
		TEST_METHOD(TestClusterManager)
		{
			size_t nCols = 3;
			size_t nRows = 20;
			std::valarray<int> data;
			gener_data(nRows * nCols, data);
			MatData<int> oMat(nRows, nCols, &data);
			ClusterManager<int, int, long> oMan(&oMat);
		}//TestClusterManager
		
	};
}