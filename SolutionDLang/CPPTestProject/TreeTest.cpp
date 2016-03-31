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
			IndivsType oIndivs(&xMat);
			std::wstringstream os;
			//////////////////////////////////
			const size_t nClasses = 5;
			TreeType oTree(&oIndivs);
			oTree.initialize();
			oTree.aggregate(nClasses);
			const size_t nn = oTree.size();
			Assert::AreEqual(nClasses, nn);
			for (size_t i = 0; i < nn; ++i) {
				auto pElem = oTree.element_at(i);
				//Assert::IsNotNull(pElem);
				ClusterType oCluster;
				pElem->convert_to_clusters(oCluster);
				os << L"Cluster " << oCluster.index() << std::endl << oCluster << std::endl;
			}// ii
			 /////////////////////
			std::wstring sd = os.str();
			Logger::WriteMessage(sd.c_str());
			////////////////////////////////
			std::wstringstream os2;
			os2 << std::endl << "DOT FILE" << std::endl;
			oTree.writeDot(os2);
			std::wstring sd2 = os2.str();
			Logger::WriteMessage(sd2.c_str());
		}// estClusterizeIndivsDouble
	};
}