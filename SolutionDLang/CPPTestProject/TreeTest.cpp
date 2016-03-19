#include "stdafx.h"
#include "CppUnitTest.h"
///////////////////////////////////
#include "infotestdata.h"
///////////////////
#include <treeelem.h>
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
	typedef TreeElem<DataType, IndexType, DistanceType, StringType> TreeElemType;
	typedef std::shared_ptr<TreeElemType> TreeElemTypePtr;
	typedef std::vector<TreeElemTypePtr> TreeElemTypePtrVector;
	typedef Tree<DataType, IndexType, DistanceType, StringType> TreeType;
	///////////////////////////////////////////
	TEST_CLASS(UnitTestTree)
	{
	public:
		 //
		TEST_METHOD(TestTreeAggrgate)
		{
			StringTypeVector rowNames;
			StringTypeVector colNames;
			size_t nCols = 0;
			size_t nRows = 0;
			std::valarray<double> data;
			InfoTestData::get(nRows, nCols, data, &rowNames, &colNames);
			Assert::IsTrue(nCols > 1);
			Assert::IsTrue(nRows > 5);
			const size_t nTotal = (size_t)(nCols * nRows);
			Assert::AreEqual(nTotal, data.size());
			MatData<double> oMat(nRows, nCols, &data, &rowNames, &colNames);
			std::valarray<double> oTransfData;
			bool b = oMat.normalize_data(oTransfData);
			Assert::IsTrue(b);
			MatData<double> xMat(nRows, nCols, &oTransfData, &rowNames, &colNames);
			//
			Indivs<double, int, double> oIndivs(&xMat);
			std::wstringstream os;
			//////////////////////////////////
			Tree<double, int, double> oTree(&oIndivs);
			const size_t nClasses = 5;
			oTree.initialize();
			oTree.aggregate(nClasses);
			const size_t nn = oTree.size();
			Assert::AreEqual(nClasses, nn);
			for (size_t i = 0; i < nn; ++i) {
				auto pElem = oTree.element_at(i);
				Assert::IsNotNull(pElem);
				IndivSet<double, int> oCluster;
				pElem->convert_to_clusters(oCluster);
				os << L"Cluster " << oCluster.index() << std::endl << oCluster << std::endl;
			}// ii
			 /////////////////////
			std::wstring sd = os.str();
			Logger::WriteMessage(sd.c_str());
		}// estClusterizeIndivsDouble
	};
}