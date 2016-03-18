#pragma once
#ifndef __MYUTILS_H__
#define __MYUTILS_H__
/////////////////////////////////
#include <cassert>
#include <valarray>
#include <vector>
#include <algorithm>
#include <iostream>
#include <random>
#include <chrono>
////////////////////////////////////////
namespace info {
	///////////////////////////////////////////////
#ifdef TOTO
	size_t convert_int_to_binary_string(const int n, std::wstring &sRet) {
		assert(n >= 0);
		sRet.clear();
		int nx = n;
		if (nx == 0) {
			sRet = L"0";
		}
		else {
			while (nx > 0) {
				int r = nx % 2;
				if (r != 0) {
					sRet = L"1" + sRet;
				}
				else {
					sRet = L"0" + sRet;
				}
				nx = nx / 2;
			}// nx
		}
		return (sRet.size());
	}// convert_int_to_binary_string
#endif
#ifdef TOTO
	size_t convert_int_to_binary_string(const int n, std::string &sRet) {
		assert(n >= 0);
		sRet.clear();
		int nx = n;
		if (nx == 0) {
			sRet = "0";
		}
		else {
			while (nx > 0) {
				int r = nx % 2;
				if (r != 0) {
					sRet = "1" + sRet;
				}
				else {
					sRet = "0" + sRet;
				}
				nx = nx / 2;
			}// nx
		}
		return (sRet.size());
	}// convert_int_to_binary_string
#endif // TOTO
	///////////////////////////////////////////////
	template <typename T, typename U = int>
	bool make_discrete(const std::valarray<T> &data, size_t &nClasses, 
		std::valarray<T> &limits, std::valarray<U> &vals) {
		assert(nClasses > 0);
		const size_t n = data.size();
		if (n < 3) {
			return false;
		}
		if ((nClasses % 2) == 0) {
			++nClasses;
		}
		assert(n >= nClasses);
		const double vMin = (double)data.min();
		const double vMax = (double)data.max();
		if (vMin >= vMax) {
			return false;
		}
		std::vector<T> temp(n);
		for (size_t i = 0; i < n; ++i) {
			temp[i] = data[i];
		}// i
		std::sort(temp.begin(), temp.end());
		const size_t n2 = (size_t)(n / 2);
		double vMed = (double)temp[n2];
		if ((n % 2) == 0) {
			vMed = (double)((vMed + temp[(n2 - 1)]) / 2);
		}
		assert(vMed >= vMin);
		assert(vMed <= vMax);
		const size_t nc = (size_t)(2 * n2 + 1);
		const double dxMin = (double)((2 * (vMed - vMin)) / nc);
		const double dxMax = (double)((2 * (vMax - vMed)) / nc);
		double xMin = vMin;
		double xMax = vMax;
		size_t i = 0;
		size_t j = nClasses;
		limits.resize(nClasses + 1);
		while (i < j) {
			limits[i++] = (T)xMin;
			limits[j--] = (T)xMax;
			xMin = xMin + dxMin;
			xMax = xMax - dxMax;
		}// while (i < j)
		vals.resize(n);
		const size_t nb = limits.size();
		for (size_t i = 0; i < n; ++i) {
			const T val = data[i];
			size_t ipos = 0;
			if (val <= vMin) {
				ipos = 0;
			}
			else if (val >= vMax) {
				ipos = (size_t)(nClasses - 1);
			}
			else {
				ipos = 0;
				for (size_t j = 0; j < nb; ++j) {
					if (val <= limits[j]) {
						break;
					}
					++ipos;
				}//j
			}
			if (ipos >= nClasses) {
				ipos = nClasses - 1;
			}
			vals[i] = (U)ipos;
		}// i
		return (true);
	}// make_discrete
	template <typename T, typename U = int>
	bool make_discrete(const std::vector<T> &data, size_t &nClasses,
		std::vector<T> &limits, std::vector<U> &vals) {
		assert(nClasses > 0);
		const size_t n = data.size();
		if (n < 3) {
			return false;
		}
		if ((nClasses % 2) == 0) {
			++nClasses;
		}
		assert(n >= nClasses);
		std::vector<T> temp(n);
		for (size_t i = 0; i < n; ++i) {
			temp[i] = data[i];
		}// i
		std::sort(temp.begin(), temp.end());
		const double vMin = (double)temp[0];
		const double vMax = (double)temp[n - 1];
		if (vMin >= vMax) {
			return false;
		}
		const size_t n2 = (size_t)(n / 2);
		double vMed = (double)temp[n2];
		if ((n % 2) == 0) {
			vMed = (double)((vMed + temp[(n2 - 1)]) / 2);
		}
		assert(vMed >= vMin);
		assert(vMed <= vMax);
		const size_t nc = (size_t)(2 * n2 + 1);
		const double dxMin = (double)((2 * (vMed - vMin)) / nc);
		const double dxMax = (double)((2 * (vMax - vMed)) / nc);
		double xMin = vMin;
		double xMax = vMax;
		size_t i = 0;
		size_t j = nClasses;
		limits.resize(nClasses + 1);
		while (i < j) {
			limits[i++] = (T)xMin;
			limits[j--] = (T)xMax;
			xMin = xMin + dxMin;
			xMax = xMax - dxMax;
		}// while (i < j)
		vals.resize(n);
		const size_t nb = limits.size();
		for (size_t i = 0; i < n; ++i) {
			const T val = data[i];
			size_t ipos = 0;
			if (val <= vMin) {
				ipos = 0;
			}
			else if (val >= vMax) {
				ipos = (size_t)(nClasses - 1);
			}
			else {
				ipos = 0;
				for (size_t j = 0; j < nb; ++j) {
					if (val <= limits[j]) {
						break;
					}
					++ipos;
				}//j
			}
			if (ipos >= nClasses) {
				ipos = nClasses - 1;
			}
			vals[i] = (U)ipos;
		}// i
		return (true);
	}// make_discrete
	///////////////////////////////////////////
	template <typename T>
	void shuffle_vector(std::vector<T> &v) {
		std::random_device rd;
		std::mt19937 g(rd());
		std::shuffle(v.begin(), v.end(), g);
	}// shuffle_vector
	/////////////////////////////////////////
	template <class INTTYPE = int>
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
	template <class REALTYPE = double>
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
	template <class REALTYPE = double>
	void gener_normal_data(const size_t n, REALTYPE a, REALTYPE b,
		REALTYPE moy, REALTYPE dev, std::valarray<REALTYPE> &v) {
		assert(n > 0);
		assert(a < b);
		assert((moy >= a) && (moy <= b));
		assert(dev > 0);
		unsigned seed = (unsigned)std::chrono::system_clock::now().time_since_epoch().count();
		std::default_random_engine generator(seed);
		std::normal_distribution<REALTYPE> distribution(moy, dev);
		v.resize(n);
		for (size_t i = 0; i < n; ++i) {
			REALTYPE x = distribution(generator);
			if (x < a) {
				x = a;
			}
			if (x > b) {
				x = b;
			}
			v[i] = x;
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


