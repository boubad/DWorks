#pragma once
#ifndef __MATDATA_H__
#define __MATDATA_H__
////////////////////////////////
#include <cassert>
#include <valarray>
#include <string>
#include <iostream>
#include <sstream>
///////////////////////////////////
#include "utils.h"
///////////////////////////////////
namespace info {
	////////////////////////////
	template <typename T = int> class MatData {
	private:
		size_t _rows;
		size_t _cols;
		const std::valarray<T> *_pdata;
	public:
		MatData() :_rows(0), _cols(0), _pdata(nullptr){}
		MatData(const size_t r, const size_t c, const std::valarray<T> *pData) :
			_rows(r), _cols(c), _pdata(pData) {
			assert(this->_rows > 0);
			assert(this->_cols > 0);
			assert(this->_pdata != nullptr);
			assert(this->_pdata->size() >= (size_t)(this->_rows * this->_cols));
		}
		MatData(const MatData<T> &other):_rows(other._rows),_cols(other._cols),_pdata(other._pdata){}
		MatData<T> & operator=(const MatData<T> &other) {
			if (this != &other) {
				this->_rows = other._rows;
				this->_cols = other._cols;
				this->_pdata = other._pdata;
			}
			return (*this);
		}
		virtual ~MatData(){}
	public:
		bool is_valid(void) const {
			return ((this->_rows > 0) && (this->_cols > 0) && (this->_pdata != nullptr) &&
				(this->_pdata->size() >= (size_t)(this->_cols * this->_rows)));
		}
		size_t rows(void) const {
			return (this->_rows);
		}
		size_t cols(void) const {
			return (this->_cols);
		}
		void row_at(const size_t irow, std::valarray<T> &v) const {
			assert(this->is_valid());
			assert(irow < this->_rows);
			v = (*(this->_pdata))[std::slice(irow * this->_cols, this->_cols, 1)];
		}// row_at
		void col_at(const size_t icol, std::valarray<T> &v) const {
			assert(this->is_valid());
			assert(icol < this->_cols);
			v = (*(this->_pdata))[std::slice(icol, this->_rows, this->_cols)];
		}// row_at
		size_t get_data_count(const DataMode mode) const {
			return ((mode == DataMode::modeRow) ? this->rows() : this->cols());
		}
		void data_at(const size_t ipos, const DataMode mode, std::valarray<T> &v) const {
			assert(ipos < this->get_data_count(mode));
			if (mode == DataMode::modeRow) {
				this->row_at(ipos, v);
			}
			else {
				this->col_at(ipos, v);
			}
		}// data_at
	public:
		std::ostream & write_to(std::ostream &os) const {
			const size_t nCols = this->_cols;
			const size_t nRows = this->_rows;
			const std::valarray<T> *pData = this->_pdata;
			os << nRows << "\t," << nCols << std::endl;
			if (pData != nullptr) {
				assert(this->is_valid());
				for (size_t i = 0; i < nRows; ++i) {
					for (size_t j = 0; j < nCols; ++j) {
						if (j > 0) {
							os << " ,";
						}
						os << pData[i * nCols + j];
					}// j
					os << std::endl;
				}// i
			}// pData
			return os;
		}// write_to
		std::wostream & write_to(std::wostream &os) const {
			const size_t nCols = this->_cols;
			const size_t nRows = this->_rows;
			const std::valarray<T> *pData = this->_pdata;
			os << nRows << L"\t," << nCols << std::endl;
			if (pData != nullptr) {
				assert(this->is_valid());
				for (size_t i = 0; i < nRows; ++i) {
					for (size_t j = 0; j < nCols; ++j) {
						if (j > 0) {
							os << L" ,";
						}
						os << (*pData)[i * nCols + j];
					}// j
					os << std::endl;
				}// i
			}// pData
			return os;
		}// write_to
		void toString(std::string &s) const {
			std::stringstream os;
			this->write_to(os);
			s = os.str();
		}
		void toString(std::wstring &s) const {
			std::wstringstream os;
			this->write_to(os);
			s = os.str();
		}
	};// class MatData<T>
	//////////////////////////////
}// namespace info
///////////////////////////////////
template <class OUTPUT, typename T>
OUTPUT & operator<<(OUTPUT &os, const info::MatData<T> &d) {
	return (d.write_to(os));
}
/////////////////////////////////
#endif // !__MATDATA_H__
