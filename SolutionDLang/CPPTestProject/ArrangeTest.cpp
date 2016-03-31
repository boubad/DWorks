#include "stdafx.h"
#include "CppUnitTest.h"
///////////////////////////////////
#include "infotestdata.h"
///////////////////
#include <treeelem.h>
#include <arrangedesc.h>
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
	typedef TreeElem<DataType, IndexType, DistanceType> TreeElemType;
	typedef std::shared_ptr<TreeElemType> TreeElemTypePtr;
	typedef std::vector<TreeElemTypePtr> TreeElemTypePtrVector;
	typedef Tree<DataType, IndexType, DistanceType> TreeType;
	typedef std::vector<DistanceType> DistanceTypeVector;
	typedef std::vector<IndexType> IndexTypeVector;
	typedef std::vector<IndexTypeVector> IndexTypeArray;
	typedef ArrangeDesc<DistanceType, IndexType> ArrangeDescType;
	///////////////////////////////////////////
	TEST_CLASS(UnitTestTree)
	{
	public:
		//
		TEST_METHOD(TestArrangeCols)
		{
			StringTypeVector rowNames;
			StringTypeVector colNames;
			size_t nCols = 0;
			size_t nRows = 0;
			DataType maxVal = 255;
			DataType minVal = 0;
			std::valarray<DataType> data;
			InfoTestData::get(nRows, nCols, data, &rowNames, &colNames);
			Assert::IsTrue(nCols > 1);
			Assert::IsTrue(nRows > 5);
			const size_t nTotal = (size_t)(nCols * nRows);
			Assert::AreEqual(nTotal, data.size());
			MatDataType oMat(nRows, nCols, &data, &rowNames, &colNames);
			std::valarray<DataType> oTransfData;
			{
				std::valarray<double> t;
				oMat.normalize_data(t);
				MatData<double> zMat(nRows, nCols, &t);
				bool b = zMat.recode_data(oTransfData, maxVal, minVal);
				Assert::IsTrue(b);
			}
			MatDataType xMat(nRows, nCols, &oTransfData, &rowNames, &colNames);
			//
			//IndivsType oIndivs(&xMat,DataMode::modeCol);
			IndivsType oIndivs(&xMat);
			const size_t nbIndivs = oIndivs.indivs_count();
			std::vector<DistanceType> dist;
			oIndivs.compute_distances(dist);
			ArrangeDescType oArrange(nbIndivs, dist);
			IndexTypeArray oAr;
			//DistanceType crit = oArrange.best_permutations(oAr);
			std::wstringstream os;
			////////////////////////////////////////
			IndexTypeVector vec;
			DistanceType dd = 0;
			oArrange.arrange(vec, dd);
			os << L"CRITERIA: " << dd << std::endl;
			for (auto jt = vec.begin(); jt != vec.end(); ++jt) {
				if (jt != vec.begin()) {
					os << L", ";
				}
				IndexType id = *jt;
				const IndivType *p = oIndivs.indiv(id);
				Assert::IsNotNull(p);
				os << p->id();
			}// jt
			//////////////////////////////////
#ifdef TOTO
			for (auto it = oAr.begin(); it != oAr.end(); ++it) {
				auto vec = *it;
				os << L"[";
				for (auto jt = vec.begin(); jt != vec.end(); ++jt) {
					if (jt != vec.begin()) {
						os << L", ";
					}
					IndexType id = *jt;
					const IndivType *p = oIndivs.indiv(id);
					Assert::IsNotNull(p);
					os << p->id();
				}// jt
				os << L" ]" << std::endl;
			}// it
#endif
			 /////////////////////
			std::wstring sd = os.str();
			Logger::WriteMessage(sd.c_str());
			////////////////////////////////
		}// estClusterizeIndivsDouble
	};
}