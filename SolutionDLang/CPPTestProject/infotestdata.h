#pragma once
///////////////////////////////
#ifndef __INFOTESTDATA_H__
#define __INFOTESTDATA_H__
/////////////////////////////////
#include <cassert>
#include <string>
#include <valarray>
#include <vector>
//////////////////////////////////////
class InfoTestData
{
public:
	InfoTestData();
	~InfoTestData();
public:
	static size_t get_nb_rows(void) {
		return (st_socmortal_rows);
	}
	static size_t get_nb_cols(void) {
		return  (st_socmortal_cols);
	}
	static const int *get_data() {
		return (&(st_socmortal_data[0]));
	}
public:
	template <typename T>
	static void get(size_t &nRows, size_t &nCols, std::valarray<T> &data,
		std::vector<std::wstring> *pIndsNames = nullptr,
		std::vector<std::wstring> *pVarsNames = nullptr ) {
		const size_t nc = InfoTestData::get_nb_cols();
		assert(nc > 2);
		const size_t nr = InfoTestData::get_nb_rows();
		assert(nr > 3);
		const int *pData = InfoTestData::get_data();
		assert(pData != nullptr);
		data.resize(nr * nc);
		nRows = nr;
		nCols = nc;
		for (size_t i = 0; i < nr; ++i) {
			for (size_t j = 0; j < nc; ++j) {
				const size_t pos = (size_t)(i * nc + j);
				data[pos] = (T)pData[pos];
			}// j
		}// i
		if (pIndsNames != nullptr) {
			pIndsNames->resize(nr);
			for (size_t i = 0; i < nr; ++i) {
				(*pIndsNames)[i] = st_socmortal_inds[i];
			}// i
		}
		if (pVarsNames != nullptr) {
			pVarsNames->resize(nc);
			for (size_t i = 0; i < nc; ++i) {
				(*pVarsNames)[i] = st_socmortal_vars[i];
			}// i
		}
	}//get_data
private:
	static size_t st_socmortal_cols;
	static size_t st_socmortal_rows;
	static int st_socmortal_data[];
	static const wchar_t *st_socmortal_vars[];
	static const wchar_t *st_socmortal_inds[];
};
///////////////////////////////
#endif //__INFOTESTDATA_H__


