#pragma once
#ifndef __UTILS_H__
#define __UTILS_H__
/////////////////////////////////
#include <valarray>
#include <iostream>
////////////////////////////////////////
namespace info {
	/////////////////////////////////////////
	enum class DataMode {modeRow,modeCol};
	//////////////////////////////////////
	template <typename T>
	std::ostream & write_valarray(std::ostream &os, const std::valarray<T> &oAr) {
		const size_t n = oAr.size();
		os << "[ ";
		for (size_t i = 0; i < n; ++i) {
			if (i > 0) {
				os << " ,";
			}
			os << oAr[i];
		}// i
		os << " ]";
		return (os);
	}// write_valarray
	template <typename T>
	std::wostream & write_valarray(std::wostream &os, const std::valarray<T> &oAr) {
		const size_t n = oAr.size();
		os << L"[ ";
		for (size_t i = 0; i < n; ++i) {
			if (i > 0) {
				os << L" ,";
			}
			os << oAr[i];
		}// i
		os << L" ]";
		return (os);
	}// write_valarray
	//////////////////////////////////////////
}// namespace info
/////////////////////////////////////
#endif // !__UTILS_H__


