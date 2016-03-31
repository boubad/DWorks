#include "stdafx.h"
#include "CppUnitTest.h"
///////////////////////////////////
#include "infotestdata.h"
///////////////////
#include <indivs.h>
#include <matelem.h>
#include <anacompo.h>
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
	typedef MatData<DataType> MatDataType;
	typedef Indiv<DataType, IndexType> IndivType;
	typedef std::shared_ptr<IndivType> IndivTypePtr;
	typedef std::vector<IndivTypePtr> IndivTypePtrVector;
	typedef IndivSet<DataType, IndexType> ClusterType;
	typedef std::shared_ptr<ClusterType> ClusterTypePtr;
	typedef std::vector<ClusterTypePtr> ClusterTypePtrVector;
	typedef std::set<IndexType> IndexTypeSet;
	typedef std::map<IndexType, IndexTypeSet> IndexTypeMap;
	//
	typedef Indivs<DataType, IndexType, DistanceType> IndivsType;
	typedef MatElem<DistanceType> MatElemType;
	typedef MatElemSort<IndexType, DistanceType> MatElemSortType;
	typedef IndexedMatData<DataType, IndexType> IndexedMatDataType;
	typedef IntraEigenSolver<double> IntraEigenSolverType;
	///////////////////////////////////////////
	TEST_CLASS(UnitTestMatElem)
	{
	public:
		//
		TEST_METHOD(TestComputeStats)
		{
			StringTypeVector rowNames;
			StringTypeVector colNames;
			size_t nCols = 0;
			size_t nRows = 0;
			std::valarray<double> data;
			InfoTestData::get_conso(nRows, nCols, data, &rowNames, &colNames);
			Assert::IsTrue(nCols > 1);
			Assert::IsTrue(nRows > 5);
			const size_t nTotal = (size_t)(nCols * nRows);
			Assert::AreEqual(nTotal, data.size());
			//
			{
				std::valarray<double> oMeans, oStds, oCorr;
				bool bRet = IntraEigenSolver<double>::compute_stats(nRows, nCols, data, oMeans, oStds, oCorr);
				Assert::IsTrue(bRet);
			}
			{
				size_t nFacts = 0;
				std::valarray<double> oMeans, oStds, oCorr, oFreq, oVals, oVecs, oVars, oInds;
				bool bRet = IntraEigenSolver<double>::compute_anacompo(nRows, nCols,
					data, oMeans, oStds, oCorr, nFacts, oFreq, oVals, oVecs, oVars, oInds);
				Assert::IsTrue(bRet);
			}
		}// TestCompiuteStats
		//
		TEST_METHOD(TestAnaCompoMat)
		{
			EuclideDistanceFunc<DataType, DistanceType> oBaseDist;
			StringTypeVector rowNames;
			StringTypeVector colNames;
			size_t nCols = 0;
			size_t nRows = 0;
			DataType maxVal = 1000;
			DataType minVal = 0;
			std::valarray<double> data;
			InfoTestData::get(nRows, nCols, data, &rowNames, &colNames);
			Assert::IsTrue(nCols > 1);
			Assert::IsTrue(nRows > 5);
			const size_t nTotal = (size_t)(nCols * nRows);
			Assert::AreEqual(nTotal, data.size());
			//
			size_t nFacts = 0;
			std::valarray<double> oVars, oInds, oFreq;
			bool bRet = IntraEigenSolverType::compute_anacompo(nRows, nCols, data, nFacts, oFreq, oVars, oInds);
			Assert::IsTrue(bRet);
			Assert::IsTrue(nFacts > 0);
			Assert::IsTrue(nFacts <= nCols);
			Assert::IsTrue(oFreq.size() >= nFacts);
			Assert::IsTrue(oVars.size() >= (size_t)(nCols * nFacts));
			Assert::IsTrue(oInds.size() >= (size_t)(nRows * nFacts));
			//
			std::valarray<double> tt = oFreq * oFreq;
			double somme = tt.sum();
			Assert::IsTrue(somme > 0.0);
			for (size_t i = 0; i < nFacts; ++i) {
				double x = oFreq[i];
				double xx = tt[i] / somme;
				oFreq[i] = xx;
			}
			WeightedDistanceFunc<DataType, DistanceType, double> fDist(oBaseDist, oFreq);
			//
			std::vector<IndexType> rowindex, colindex;
			//
			{
				MatData<double> oMat(nRows, nFacts, &oInds);
				std::valarray<DataType> oTransfData;
				bool b = oMat.recode_data(oTransfData, maxVal, minVal);
				Assert::IsTrue(b);
				MatDataType xMat(nRows, nFacts, &oTransfData);
				IndivsType oIndivs(&xMat, DataMode::modeRow, &fDist);
				std::vector<DistanceType> distances;
				oIndivs.compute_distances(distances);
				const size_t n = oIndivs.indivs_count();
				MatElemSortType oSort(n, &distances);
				std::vector<IndexType> index(n);
				for (size_t i = 0; i < n; ++i) {
					index[i] = i;
				}
				DistanceType crit1 = oSort.criteria(index);
				oSort.arrange(index);
				rowindex = index;
				DistanceType crit2 = oSort.criteria(index);
				std::wstringstream os;
				////////////////////////////////////////
				os << L"Start crit: " << crit1 << L"\t\tEnd crit: " << crit2 << std::endl;
				for (auto jt = index.begin(); jt != index.end(); ++jt) {
					if (jt != index.begin()) {
						os << L", ";
					}
					IndexType id = *jt;
					const IndivType *p = oIndivs.indiv(id);
					Assert::IsNotNull(p);
					os << p->index();
				}// jt
				 /////////////////////
				std::wstring sd = os.str();
				Logger::WriteMessage(sd.c_str());
			}// indivs
			//
			{
				MatData<double> oMat(nCols, nFacts, &oVars);
				std::valarray<DataType> oTransfData;
				bool b = oMat.recode_data(oTransfData, maxVal, minVal);
				Assert::IsTrue(b);
				MatDataType xMat(nCols, nFacts, &oTransfData);
				IndivsType oIndivs(&xMat, DataMode::modeRow, &fDist);
				std::vector<DistanceType> distances;
				oIndivs.compute_distances(distances);
				const size_t n = oIndivs.indivs_count();
				MatElemSortType oSort(n, &distances);
				std::vector<IndexType> index(n);
				for (size_t i = 0; i < n; ++i) {
					index[i] = i;
				}
				DistanceType crit1 = oSort.criteria(index);
				oSort.arrange(index);
				colindex = index;
				DistanceType crit2 = oSort.criteria(index);
				std::wstringstream os;
				////////////////////////////////////////
				os << L"Start crit: " << crit1 << L"\t\tEnd crit: " << crit2 << std::endl;
				for (auto jt = index.begin(); jt != index.end(); ++jt) {
					if (jt != index.begin()) {
						os << L", ";
					}
					IndexType id = *jt;
					const IndivType *p = oIndivs.indiv(id);
					Assert::IsNotNull(p);
					os << p->index();
				}// jt
				 /////////////////////
				std::wstring sd = os.str();
				Logger::WriteMessage(sd.c_str());
			}// vars
			//
			////////////////////////////////////
			MatData<double> kMat(nRows, nCols, &data, &rowNames, &colNames);
			IndexedMatData<double> rMat(&kMat);
			rMat.colindex(colindex);
			rMat.rowindex(rowindex);
			{
				std::wstringstream os;
				os << std::endl << std::endl;
				os << rMat;
				std::wstring sd = os.str();
				Logger::WriteMessage(sd.c_str());
			}
			////////////////////////////////
		}// estClusterizeIndivsDouble
	};
}