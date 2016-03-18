#include "stdafx.h"
#include "CppUnitTest.h"
///////////////////////////////////
#include "infotestdata.h"
///////////////////
#include <clusterizeindivs.h>
//////////////////////////////////////
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace info;
////////////////////////////////////
namespace CPPTestProject
{
	typedef int DataType;
	typedef int IndexType;
	typedef long DistanceType;
	typedef size_t ResultType;
	typedef std::wstring StringType;
	//
	typedef std::vector<StringType> StringTypeVector;
	typedef std::valarray<DataType> DataTypeArray;
	typedef DistanceFunc<DataType, DistanceType> DistanceFuncType;
	typedef MatData<DataType, StringType> MatDataType;
	typedef Indiv<DataType, IndexType, StringType> IndivType;
	typedef std::shared_ptr<IndivType> IndivTypePtr;
	typedef std::vector<IndivTypePtr> IndivTypePtrVector;
	typedef IndivSet<DataType, IndexType, StringType> ClusterType;
	typedef std::shared_ptr<ClusterType> ClusterTypePtr;
	typedef std::vector<ClusterTypePtr> ClusterTypePtrVector;
	typedef std::set<IndexType> IndexTypeSet;
	typedef std::map<IndexType, IndexTypeSet> IndexTypeMap;
	//
	typedef Indivs<DataType, IndexType, DistanceType, StringType> IndivsType;
	typedef ClusterizeIndivs<DataType, IndexType, DistanceType, StringType> ClusterizeIndivsType;
	typedef ResultItemEntry<IndexType, ResultType> ResultItemEntryType;
	typedef std::vector<ResultItemEntryType> ResultItemEntryTypeVector;
	///////////////////////////////////////////
	TEST_CLASS(UnitTestClusterizeIndivs)
	{
	public:
		TEST_METHOD(TestClusterizeIndivs)
		{
			StringTypeVector rowNames;
			StringTypeVector colNames;
			size_t nCols = 0;
			size_t nRows = 0;
			DataTypeArray data;
			InfoTestData::get(nRows, nCols, data, &rowNames, &colNames);
			Assert::IsTrue(nCols > 2);
			Assert::IsTrue(nRows > 3);
			const size_t nTotal = (size_t)(nCols * nRows);
			Assert::AreEqual(nTotal, data.size());
			MatData<DataType> oMat(nRows, nCols, &data, &rowNames, &colNames);
			DataTypeArray oTransfData;
			bool b = oMat.recode_data(oTransfData);
			Assert::IsTrue(b);
			MatData<DataType> xMat(nRows, nCols, &oTransfData, &rowNames, &colNames);
			//
			IndivsType oIndivs(&xMat);
			//
			ClusterizeIndivsType oClusters(&oIndivs);
			//
			size_t nbClusters = 5;
			IndexTypeMap oMap;
			oClusters.clusterize(oMap, nbClusters);
			std::wstringstream os;
			for (auto it = oMap.begin(); it != oMap.end(); ++it) {
				IndexType aInd = (*it).first;
				const IndexTypeSet &oSet = (*it).second;
				os << L"CLUSTER: " << aInd << std::endl;
				for (auto jt = oSet.begin(); jt != oSet.end(); ++jt) {
					IndexType pos = *jt;
					const IndivType *pInd = oIndivs.find_indiv_by_index(pos);
					Assert::IsNotNull(pInd);
					os << L"\t" << *pInd << std::endl;
				}// jt
			}// it
			 /////////////////////
			std::wstring sd = os.str();
			Logger::WriteMessage(sd.c_str());
		}// TestClusterizeIndivs
		TEST_METHOD(TestClusterizeIndivsDouble)
		{
			StringTypeVector rowNames;
			StringTypeVector colNames;
			size_t nCols = 0;
			size_t nRows = 0;
			std::valarray<double> data;
			InfoTestData::get(nRows, nCols, data, &rowNames, &colNames);
			Assert::IsTrue(nCols > 2);
			Assert::IsTrue(nRows > 3);
			const size_t nTotal = (size_t)(nCols * nRows);
			Assert::AreEqual(nTotal, data.size());
			MatData<double> oMat(nRows, nCols, &data, &rowNames, &colNames);
			std::valarray<double> oTransfData;
			bool b = oMat.normalize_data(oTransfData);
			Assert::IsTrue(b);
			MatData<double> xMat(nRows, nCols, &oTransfData, &rowNames, &colNames);
			//
			Indivs<double, int, double> oIndivs(&xMat);
			//
			ClusterizeIndivs<double, int, double> oClusters(&oIndivs);
			//
			for (int k = 0; k < 10; ++k) {
				size_t nbClusters = 5;
				IndexTypeMap oMap;
				oClusters.clusterize(oMap, nbClusters);
				std::wstringstream os;
				for (auto it = oMap.begin(); it != oMap.end(); ++it) {
					IndexType aInd = (*it).first;
					const IndexTypeSet &oSet = (*it).second;
					os << L"CLUSTER: " << aInd << std::endl;
					for (auto jt = oSet.begin(); jt != oSet.end(); ++jt) {
						IndexType pos = *jt;
						const Indiv<double> *pInd = oIndivs.find_indiv_by_index(pos);
						Assert::IsNotNull(pInd);
						os << L"\t" << *pInd << std::endl;
					}// jt
				}// it
				 /////////////////////
				std::wstring sd = os.str();
				Logger::WriteMessage(sd.c_str());
			}// k
		}// estClusterizeIndivsDouble
		//
		TEST_METHOD(TestMultiClusterize)
		{
			StringTypeVector rowNames;
			StringTypeVector colNames;
			size_t nCols = 0;
			size_t nRows = 0;
			std::valarray<double> data;
			InfoTestData::get(nRows, nCols, data, &rowNames, &colNames);
			Assert::IsTrue(nCols > 2);
			Assert::IsTrue(nRows > 3);
			const size_t nTotal = (size_t)(nCols * nRows);
			Assert::AreEqual(nTotal, data.size());
			MatData<double> oMat(nRows, nCols, &data, &rowNames, &colNames);
			std::valarray<double> oTransfData;
			bool b = oMat.normalize_data(oTransfData);
			Assert::IsTrue(b);
			MatData<double> xMat(nRows, nCols, &oTransfData, &rowNames, &colNames);
			//
			Indivs<double, int, double> oIndivs(&xMat);
			//
			ClusterizeIndivs<double, int, double> oClusters(&oIndivs);
			//
			ResultItem<IndexType, size_t> oResult;
			size_t nbClusters = 5;
			size_t nCount = 100;
			size_t limit = 50;
			std::wstringstream os;
			oClusters.multi_clusterize(oResult, nCount, nbClusters);
			ResultItemEntryTypeVector oVec;
			oResult.get(oVec, limit);
			const size_t n = oVec.size();
			for (size_t i = 0; i < n; ++i) {
				auto p = oVec[i];
				const IndexType index1 = p.firstIndex();
				const IndexType index2 = p.secondIndex();
				const ResultType r = p.value();
				const Indiv<double> *pInd1 = oIndivs.indiv(index1);
				Assert::IsNotNull(pInd1);
				const Indiv<double> *pInd2 = oIndivs.indiv(index2);
				Assert::IsNotNull(pInd2);
				os << pInd1->id() << L" --> \t" << pInd2->id() << L"\t" << r << std::endl;
			}// i
			 /////////////////////
			std::wstring sd = os.str();
			Logger::WriteMessage(sd.c_str());
		}// estClusterizeIndivsDouble
	};
}