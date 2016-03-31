#pragma once
#ifndef __GENDEFS_H__
#define __GENDEFS_H__
////////////////////////////////////
#include <cassert>
#include <type_traits>
#include <memory>
#include <string>
#include <iostream>
#include <sstream>
#include <valarray>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <deque>
#include <random>
#include <chrono>
#include <cmath>
#include <algorithm>
//////////////////////////////////////////
namespace info {
	//////////////////////////////////////////
#if defined(INFO_STRING_TYPE)
	typedef char CharType;
	typedef std::string StringType;
	typedef std::ostream OStreamType;
	typedef std::stringstream StringStreamType;
#define STRING_TAB ("\t")
#define STRING_COMMA (", ")
#define START_ARRAY ("[")
#define END_ARRAY ("]")
#define START_OBJECT ("{")
#define END_OBJECT ("}")
#define STRING_ZERO ("0")
#define STRING_ONE  ("1")
#define CHAR_ZERO ('0')
#define CHAR_ONE ('1')
#define LEFT_PAR ("(")
#define RIGHT_PAR (")")
#else
	typedef wchar_t CharType;
	typedef std::wstring StringType;
	typedef std::wostream OStreamType;
	typedef std::wstringstream StringStreamType;
#define STRING_TAB (L"\t")
#define STRING_COMMA (L", ")
#define START_ARRAY (L"[")
#define END_ARRAY (L"]")
#define START_OBJECT (L"{")
#define END_OBJECT (L"}")
#define STRING_ZERO (L"0")
#define STRING_ONE  (L"1")
#define CHAR_ZERO (L'0')
#define CHAR_ONE (L'1')
#define LEFT_PAR (L"(")
#define RIGHT_PAR (L")")
#endif //INFO_STRING_TYPE
	///////////////////////////////////////////////
}// namespace info
////////////////////////////////////////
#endif // !__GENDEFS_H__
