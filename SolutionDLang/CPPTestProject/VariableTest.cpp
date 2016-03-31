#include "stdafx.h"
#include "CppUnitTest.h"
///////////////////////////////////
#include "infotestdata.h"
///////////////////
#include <variable.h>
#include <indivs.h>
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
	///////////////////////////////////////////
	TEST_CLASS(UnitTestVariable)
	{
	public:
		//
		TEST_METHOD(TestNumVariableDesc)
		{
			StringType sName(L"TestVariable");
			StringType lName(L"TestVariable Long name");
			StringType desc(L"TestVariable description");
			const size_t nbModalites = 8;
			NumVariableDesc oDesc(sName, lName, desc);
			//
			Assert::AreEqual(sName, oDesc.shortName());
			Assert::AreEqual(lName, oDesc.longName());
			Assert::AreEqual(desc, oDesc.description());
			//
			VariableType oType = VariableType::numType;
			VariableType cType = oDesc.variableType();
			Assert::IsTrue(oDesc.is_valid());
			//
			const size_t n = 10;
			std::vector<int> data;
			for (size_t i = 0; i < n; ++i) {
				int nx = ::rand() % 8;
				data.push_back(nx);
			}// i
			std::vector<bool> dest;
			bool bRet = oDesc.to_bool_array(data, dest);
			Assert::IsTrue(bRet);
		}//TestOrdinalVariableDesc
		//
		TEST_METHOD(TestOrdinalVariableDesc)
		{
			StringType sName(L"TestVariable");
			StringType lName(L"TestVariable Long name");
			StringType desc(L"TestVariable description");
			const size_t nbModalites = 8;
			OrdinalVariableDesc oDesc(nbModalites, sName,lName, desc);
			//
			Assert::AreEqual(sName, oDesc.shortName());
			Assert::AreEqual(lName, oDesc.longName());
			Assert::AreEqual(desc, oDesc.description());
			//
			VariableType oType = VariableType::ordinalType;
			VariableType cType = oDesc.variableType();
			Assert::IsTrue(oDesc.is_valid());
			//
			Assert::AreEqual(nbModalites, oDesc.modalites_count());
			const size_t n = 10;
			std::vector<int> data;
			for (size_t i = 0; i < n; ++i) {
				int nx = ::rand() % 8;
				data.push_back(nx);
			}// i
			std::vector<bool> dest;
			bool bRet = oDesc.to_bool_array(data, dest);
			Assert::IsTrue(bRet);
		}//TestOrdinalVariableDesc
		 //
		//
		TEST_METHOD(TestNominalVariableDesc)
		{
			StringType sName(L"TestVariable");
			StringType lName(L"TestVariable Long name");
			StringType desc(L"TestVariable description");
			std::vector<StringType> ovec;
			std::set<StringType> oSet;
			oSet.insert(L"zero");
			oSet.insert(L"one");
			oSet.insert(L"two");
			oSet.insert(L"three");
			for (auto it = oSet.begin(); it != oSet.end(); ++it) {
				ovec.push_back(*it);
			}
			NominalVariableDesc oDesc(sName, oSet, lName, desc);
			//
			Assert::AreEqual(sName, oDesc.shortName());
			Assert::AreEqual(lName, oDesc.longName());
			Assert::AreEqual(desc, oDesc.description());
			//
			VariableType oType = VariableType::nominalType;
			VariableType cType = oDesc.variableType();
			//Assert::AreEqual(oType, cType);
			Assert::IsTrue(oDesc.is_valid());
			//
			oDesc.add_modalite(L"four");
			oDesc.add_modalite(L"five");
			oDesc.add_modalite(L"six");
			oDesc.add_modalite(L"seven");
			//
			std::map<StringType, int> oMap;
			oDesc.get_modalites(oMap);
			std::map<int, StringType> rMap;
			for (auto it = oMap.begin(); it != oMap.end(); ++it) {
				StringType val = (*it).first;
				int key = (*it).second;
				rMap[key] = val;
			}// it
			const size_t nn = oMap.size();
			const size_t n = 10;
			std::vector<StringType> data;
			for (size_t i = 0; i < n; ++i) {
				int nx = ::rand() % 8;
				if (rMap.find(nx) != rMap.end()) {
					StringType s = rMap[nx];
					data.push_back(s);
				}
			}// i
			std::vector<bool> dest;
			bool bRet = oDesc.to_bool_array(data, dest);
			Assert::IsTrue(bRet);
		}//TestNominalVariableDesc
		//
		TEST_METHOD(TestBooleanVariableDesc)
		{
			StringType sName(L"TestVariable");
			StringType lName(L"TestVariable Long name");
			StringType desc(L"TestVariable description");
			BooleanVariableDesc oDesc(sName,lName,desc);
			//
			Assert::AreEqual(sName, oDesc.shortName());
			Assert::AreEqual(lName, oDesc.longName());
			Assert::AreEqual(desc, oDesc.description());
			//
			VariableType oType = VariableType::booleanType;
			VariableType cType = oDesc.variableType();
			Assert::IsTrue(oType == cType);
			Assert::IsTrue(oDesc.is_valid());
			//
			const size_t n = 10;
			std::vector<bool> data;
			for (size_t i = 0; i < n; ++i) {
				if ((::rand() % 2) == 1) {
					data.push_back(true);
				}
				else {
					data.push_back(false);
				}
			}// i
			std::vector<bool> dest;
			bool bRet = oDesc.to_bool_array(data, dest);
			Assert::AreEqual(n, dest.size());
			for (size_t i = 0; i < n; ++i) {
				bool b1 = data[i];
				bool b2 = dest[i];
				Assert::AreEqual(b1, b2);
			}// i
		}//TestBooleanVariableDesc
		
	};// class UnitTestVariable
}
