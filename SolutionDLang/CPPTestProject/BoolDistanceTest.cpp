#include "stdafx.h"
#include "CppUnitTest.h"
///////////////////////////////////
#include "infotestdata.h"
///////////////////
#include <indivs.h>
#include <booldistance.h>
#include <variable.h>
//////////////////////////////////////
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace info;
////////////////////////////////////
namespace CPPTestProject
{
	typedef double DataType;
	typedef int IndexType;
	typedef double DistanceType;
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
	///////////////////////////////////////////
	TEST_CLASS(UnitTestBoolDistance)
	{
	public:
		//
		TEST_METHOD(TestBoolDistance)
		{
			StringTypeVector rowNames;
			StringTypeVector colNames;
			size_t nCols = 0;
			size_t nRows = 0;
			std::valarray<DataType> data;
			InfoTestData::get_conso(nRows, nCols, data, &rowNames, &colNames);
			Assert::IsTrue(nCols > 1);
			Assert::IsTrue(nRows > 5);
			const size_t nTotal = (size_t)(nCols * nRows);
			Assert::AreEqual(nTotal, data.size());
			MatDataType oMat(nRows, nCols, &data, &rowNames, &colNames);
			std::valarray<int> oClasses;
			size_t nClasses = 7;
			bool bRet = oMat.discretize_data(oClasses,nClasses);
			Assert::IsTrue(bRet);
			MatData<int> wMat(nRows, nCols, &oClasses, &rowNames, &colNames);
			Indivs<int> oInds(&wMat);
			size_t nbIndivs = oInds.indivs_count();
			Assert::IsTrue(nbIndivs > 1);
			OrdinalVariableDesc<StringType> oVar(nClasses, L"TestVar");
			//
			const Indiv<int> *pInd1 = (oInds.indiv_at(0)).get();
			Assert::IsNotNull(pInd1);
			std::valarray<int> cur1 = pInd1->value();
			const size_t n1 = cur1.size();
			Assert::IsTrue(n1 > 0);
			std::vector<bool> oBoolData1;
			bRet = oVar.to_bool_array(cur1, oBoolData1);
			Assert::IsTrue(bRet);
			const Indiv<int> *pInd2 = (oInds.indiv_at(1)).get();
			Assert::IsNotNull(pInd2);
			std::valarray<int> cur2 = pInd2->value();
			const size_t n2 = cur1.size();
			Assert::IsTrue(n2 > 0);
			std::vector<bool> oBoolData2;
			bRet = oVar.to_bool_array(cur2, oBoolData2);
			Assert::IsTrue(bRet);
			Assert::AreEqual(oBoolData1.size(), oBoolData2.size());
			std::vector< IndiceDistanceType> indices;
			IndiceDistanceFunc<DistanceType>::get_all_indices(indices);
			
			std::wstringstream os;
			////////////////////////
			for (auto it = indices.begin(); it != indices.end(); ++it) {
				const IndiceDistanceType c = *it;
				IndiceDistanceFunc<DistanceType> oFunc(c);
				DistanceType dz = oFunc(oBoolData1, oBoolData2);
				int ic = static_cast<int>(c);
				if (dz < 0.0) {
					os << ic << L"\t\t" << dz << L"\tHOOPS..." <<std::endl;
				}
				else {
					os << ic << L"\t\t" << dz << std::endl;
				}
				
			}// it
			////////////////////////////
			std::wstring sd = os.str();
			Logger::WriteMessage(sd.c_str());
			//
		}// TestBoolDistance
	};
}
