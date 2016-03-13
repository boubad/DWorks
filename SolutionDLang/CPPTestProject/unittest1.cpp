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
#include <indivs.h>
//////////////////////////////////////
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace info;
////////////////////////////////////
namespace CPPTestProject
{		
	typedef int DataType;
	typedef int IndexType;
	typedef long DistanceType;
	typedef std::wstring StringType;
	//
	typedef std::valarray<DataType> DataTypeArray;
	typedef DistanceFunc<DataType, DistanceType> DistanceFuncType;
	typedef MatData<DataType> MatDataType;
	typedef Indiv<DataType, IndexType, StringType> IndivType;
	typedef std::shared_ptr<IndivType> IndivTypePtr;
	typedef std::vector<IndivTypePtr> IndivTypePtrVector;
	typedef IndivSet<DataType, IndexType, StringType> ClusterType;
	typedef std::shared_ptr<ClusterType> ClusterTypePtr;
	typedef std::vector<ClusterTypePtr> ClusterTypePtrVector;
	typedef Indivs<DataType, IndexType, DistanceType, StringType> IndivsType;

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
			IndexType aIndex = 100;
			size_t n = 5;
			ClusterType c(aIndex);
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
			size_t nCols = 5;
			size_t nRows = 100;
			std::valarray<int> data;
			gener_int_data(nRows * nCols, 0, 20, data);
			MatData<int> oMat(nRows, nCols, &data);
			ClusterManager<int, int, long> oMan(&oMat);
			oMan.clusterize();
			std::wstringstream os;
			os << oMan;
			std::wstring sd = os.str();
			Logger::WriteMessage(sd.c_str());
		}//TestClusterManager
		TEST_METHOD(TestIndivs)
		{
			double vmin = 0.0;
			double vmax = 20.0;
			double moy = 10.0;
			double ecart = 3.0;
			size_t nbIters = 200;
			size_t nbClusters = 7;
			size_t nCols = 5;
			size_t nRows = 100;
			size_t ntotal = (size_t)(nCols * nRows);
			std::valarray<double> dv;
			gener_normal_data(ntotal, vmin, vmax, moy, ecart, dv);
			std::valarray<DataType> data(ntotal);
			for (size_t i = 0; i < ntotal; ++i) {
				data[i] = (DataType)dv[i];
			}
			MatDataType oMat(nRows, nCols, &data);
			IndivsType oMan(&oMat);
			bool b = oMan.check_data();
			Assert::IsTrue(b);
			ClusterTypePtrVector clusters;
			size_t nc = oMan.clusterize(clusters,nbClusters,nbIters);
			std::wstringstream os;
			os << L"COUNT: " << nc << std::endl;
			for (auto it = clusters.begin(); it != clusters.end(); ++it) {
				ClusterTypePtr o = *it;
				ClusterType *p = o.get();
				Assert::IsNotNull(p);
				os << *p << std::endl;
			}// i
			std::wstring sd = os.str();
			Logger::WriteMessage(sd.c_str());
		}//TestIndivs
	};
}