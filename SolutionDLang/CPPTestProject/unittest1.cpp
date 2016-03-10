#include "stdafx.h"
#include "CppUnitTest.h"
///////////////////////////////////
#include <cstdlib>
////////////////////////////////////
#include <indiv.h>
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
			size_t n = 20;
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

	};
}