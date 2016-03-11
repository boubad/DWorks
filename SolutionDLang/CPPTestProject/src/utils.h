#pragma once
#ifndef __UTILS_H__
#define __UTILS_H__
/////////////////////////////////
#include <cassert>
#include <valarray>
#include <iostream>
#include <random>
#include <chrono>
////////////////////////////////////////
namespace info {
	/////////////////////////////////////////
	enum class DataMode {modeRow,modeCol};
	/////////////////////////////////////////////
	template <class INTTYPE>
	void gener_int_data(const size_t n, INTTYPE a, INTTYPE b, std::valarray<INTTYPE> &v) {
		assert(n > 0);
		assert(a < b);
		unsigned seed = (unsigned)std::chrono::system_clock::now().time_since_epoch().count();
		std::default_random_engine generator(seed);
		std::uniform_int_distribution<INTTYPE> distribution(a, b);
		v.resize(n);
		for (size_t i = 0; i < n; ++i) {
			v[i] = distribution(generator);
		}// i
	}// gener_real_data
	//////////////////////////////////////////
	template <class REALTYPE>
	void gener_real_data(const size_t n, REALTYPE a, REALTYPE b, std::valarray<REALTYPE> &v) {
		assert(n > 0);
		assert(a < b);
		unsigned seed = (unsigned)std::chrono::system_clock::now().time_since_epoch().count();
		std::default_random_engine generator(seed);
		std::uniform_real_distribution<REALTYPE> distribution(a, b);
		v.resize(n);
		for (size_t i = 0; i < n; ++i) {
			v[i] = distribution(generator);
		}// i
	}// gener_real_data
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


