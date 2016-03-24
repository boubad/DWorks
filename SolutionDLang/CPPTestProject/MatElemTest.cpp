#include "stdafx.h"
#include "CppUnitTest.h"
///////////////////////////////////
#include "infotestdata.h"
///////////////////
#include <matelem.h>
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
	typedef MatElem<DistanceType> MatElemType;
	
	///////////////////////////////////////////
	TEST_CLASS(UnitTestMatElem)
	{
	public:
		//
		TEST_METHOD(TestArrangeMat)
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
			std::vector<size_t> index;
			bool bRet = MatElemType::arrange(oIndivs, index);
			Assert::IsTrue(bRet);
			std::wstringstream os;
			////////////////////////////////////////
			for (auto jt = index.begin(); jt != index.end(); ++jt) {
				if (jt != index.begin()) {
					os << L", ";
				}
				IndexType id = *jt;
				const IndivType *p = oIndivs.indiv(id);
				Assert::IsNotNull(p);
				os << p->id();
			}// jt
			 /////////////////////
			std::wstring sd = os.str();
			Logger::WriteMessage(sd.c_str());
			////////////////////////////////
		}// estClusterizeIndivsDouble
	};
}