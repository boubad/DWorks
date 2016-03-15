#pragma once
#ifndef __MATDATA_H__
#define __MATDATA_H__
////////////////////////////////
#include <cassert>
#include <valarray>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <random>
#include <chrono>
///////////////////////////////////
#include "utils.h"
///////////////////////////////////
namespace info {
	//////////////////////////////////////////
	enum class DataMode { noMode, modeRow, modeCol };
	////////////////////////////
	template <typename T = int> class MatData {
	public:
		typedef T DataType;
		typedef std::valarray<T> DataTypeArray;
		typedef MatData<DataType> MatDataType;
	private:
		size_t _rows;
		size_t _cols;
		const DataTypeArray *_pdata;
	public:
		MatData() :_rows(0), _cols(0), _pdata(nullptr) {}
		MatData(const size_t r, const size_t c, const DataTypeArray *pData) :
			_rows(r), _cols(c), _pdata(pData) {
			assert(this->_rows > 0);
			assert(this->_cols > 0);
			assert(this->_pdata != nullptr);
			assert(this->_pdata->size() >= (size_t)(this->_rows * this->_cols));
		}
		MatData(const MatDataType &other) :_rows(other._rows), _cols(other._cols), _pdata(other._pdata) {}
		MatDataType & operator=(const MatDataType &other) {
			if (this != &other) {
				this->_rows = other._rows;
				this->_cols = other._cols;
				this->_pdata = other._pdata;
			}
			return (*this);
		}
		virtual ~MatData() {}
	public:
		DataType operator()(const size_t irow, const size_t icol) const {
			return (this->value_at(irow, icol));
		}
	public:
		bool is_valid(void) const {
			return ((this->rows() > 0) && (this->cols() > 0) && (this->_pdata != nullptr) &&
				(this->_pdata->size() >= (size_t)(this->cols() * this->rows())));
		}
		size_t rows(void) const {
			return (this->_rows);
		}
		size_t cols(void) const {
			return (this->_cols);
		}
	public:
		virtual DataType value_at(const size_t irow, const size_t icol) const {
			assert(this->is_valid());
			assert(irow < this->_rows);
			assert(icol < this->_cols);
			return ((*(this->_pdata))[irow * this->_cols + icol]);
		}
		virtual void row_at(const size_t irow, DataTypeArray &v) const {
			assert(this->is_valid());
			assert(irow < this->_rows);
			v = (*(this->_pdata))[std::slice(irow * this->_cols, this->_cols, 1)];
		}// row_at
		virtual void col_at(const size_t icol, DataTypeArray &v) const {
			assert(this->is_valid());
			assert(icol < this->_cols);
			v = (*(this->_pdata))[std::slice(icol, this->_rows, this->_cols)];
		}// row_at
		void get_rows_min_max(DataTypeArray &vmin, DataTypeArray &vmax) const {
			const size_t n = this->cols();
			vmin.resize(n);
			vmax.resize(n);
			for (size_t i = 0; i < n; ++i) {
				DataTypeArray v;
				this->col_at(i, v);
				vmin[i] = v.min();
				vmax[i] = v.max();
			}// i
		}//get_rows_min_max
		void get_cols_min_max(DataTypeArray &vmin, DataTypeArray &vmax) const {
			const size_t n = this->rows();
			vmin.resize(n);
			vmax.resize(n);
			for (size_t i = 0; i < n; ++i) {
				DataTypeArray v;
				this->row_at(i, v);
				vmin[i] = v.min();
				vmax[i] = v.max();
			}// i
		}//get_cols_min_max
		bool discretize_data(std::valarray<int> &vRet) const {
			assert(this->is_valid());
			const size_t nRows = this->_rows;
			if (nRows < 8) {
				return (false);
			}
			const size_t nCols = this->_cols;
			const size_t nTotal = (size_t)(nCols * nRows);
			vRet.resize(nTotal);
			const size_t nClasses = 8;
			for (size_t icol = 0; icol < nCols; ++icol) {
				DataTypeArray v;
				this->col_at(i,v);
				std::valarray<int> index;
				std::valarray<DataType> limits;
				bool bRet = make_discrete(v, nClasses, limits, index);
				assert(bRet);
				assert(index.size() == v.size());
				assert(v.size() == nRows);
				for (size_t irow = 0; irow < nRows; ++irow) {
					const size_t pos = (size_t)(irow * nCols + icol);
					vRet[pos] = index[irow];
				}// irow
			}// icol
			return (true);
		}// discretize
	public:
		size_t get_data_count(const DataMode mode) const {
			if (mode == DataMode::modeRow) {
				return (this->rows());
			}
			else if (mode == DataMode::modeCol) {
				return (this->cols());
			}
			else {
				return (0);
			}
		}
		void data_at(const size_t ipos, const DataMode mode, DataTypeArray &v) const {
			assert(ipos < this->get_data_count(mode));
			if (mode == DataMode::modeRow) {
				this->row_at(ipos, v);
			}
			else {
				this->col_at(ipos, v);
			}
		}// data_at
		void get_data_min_max(const DataMode mode, DataTypeArray &vmin, DataTypeArray &vmax) const {
			if (mode == DataMode::modeRow) {
				this->get_rows_min_max(vmin, vmax);
			}
			else if (mode == DataMode::modeCol) {
				this->get_cols_min_max(vmin, vmax);
			}
		}
		void get_random_data(const DataMode mode, DataTypeArray &v) const {
			unsigned seed = (unsigned)std::chrono::system_clock::now().time_since_epoch().count();
			std::default_random_engine generator(seed);
			DataTypeArray vmin, vmax;
			this->get_data_min_max(mode, vmin, vmax);
			const size_t n = vmin.size();
			v.resize(n);
			for (size_t i = 0; i < n; ++i) {
				const T a = vmin[i];
				const T b = vmax[i];
				if (a >= b) {
					v[i] = a;
				}
				else {
					std::uniform_real_distribution<double> distribution((double)a, (double)b);
					v[i] = (T)distribution(generator);
				}
			}// i
		}// get_random_data
	public:
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
	public:
		virtual std::ostream & write_to(std::ostream &os) const {
			const size_t nRows = this->_rows;
			os << nRows << "\t," << this->cols() << std::endl;
			for (size_t i = 0; i < nRows; ++i) {
				DataTypeArray v;
				this->row_at(i, v);
				const size_t n = v.size();
				for (size_t j = 0; j < n; ++j) {
					if (j > 0) {
						os << " ,";
					}
					os << v[j];
				}// j
				os << std::endl;
			}// i
			return os;
		}// write_to
		virtual std::wostream & write_to(std::wostream &os) const {
			const size_t nRows = this->_rows;
			os << nRows << L"\t," << this->cols() << std::endl;
			for (size_t i = 0; i < nRows; ++i) {
				DataTypeArray v;
				this->row_at(i, v);
				const size_t n = v.size();
				for (size_t j = 0; j < n; ++j) {
					if (j > 0) {
						os << L" ,";
					}
					os << v[j];
				}// j
				os << std::endl;
			}// i
			return os;
		}// write_to

	};// class MatData<T>
	//////////////////////////////
	template <typename T = int, typename U = int> class IndexedMatData : public MatData<T> {
	public:
		typedef T DataType;
		typedef U IndexType;
		typedef MatData<DataType> MatDataType;
		typedef std::vector<IndexType> IndexTypeVector;
		typedef IndexedMatData<DataType, IndexType> IndexedMatDataType;
	private:
		IndexTypeVector _rowindex;
		IndexTypeVector _colindex;
	public:
		IndexedMatData() {}
		IndexedMatData(const size_t r, const size_t c, const DataTypeArray *pData) :MataDataType(r, c, pData),
			_rowindex(r), _colindex(c) {
			IndexTypeVector &vr = this->_rowindex;
			assert(vr.size() == r);
			for (size_t i = 0; i < r; ++i) {
				vr[i] = (U)i;
			}
			IndexTypeVector &vc = this->_colindex;
			assert(vc.size() == c);
			for (size_t i = 0; i < c; ++i) {
				vc[i] = (U)i;
			}
		}
		IndexedMatData(const IndexedMatDataType &other) :MatDataType(other),
			_rowindex(other._rowindex), _colindex(other._colindex) {}
		IndexedMatDataType & operator=(const IndexedMatDataType &other) {
			if (this != &other) {
				MatDataType::operator=(other);
				this->_rowindex = other._rowindex;
				this->_colindex = other._colindex;
			}
			return (*this);
		}
		virtual ~IndexedMatData() {}
	public:
		void shuffle_rows(void) {
			shuffle_vector(this->_rowindex);
		}
		const IndexTypeVector & rowindex(void) const {
			return (this->_rowindex);
		}
		void rowindex(const IndexTypeVector &v) {
			const size_t n = this->rows();
			assert(v.size() >= n);
			IndexTypeVector &vv = this->_rowindex;
			vv.resize(n);
			for (size_t i = 0; i < n; ++i) {
				const IndexType val = v[i];
				assert((val >= 0) && (val < (IndexType)n));
				vv[i] = val;
			}// i
		}
		void reset_rowindex(void) {
			IndexTypeVector &vr = this->_rowindex;
			const size_t n = vr.size();
			for (size_t i = 0; i < n; ++i) {
				vr[i] = (U)i;
			}
		}
		void shuffle_cols(void) {
			shuffle_vector(this->_colindex);
		}
		const IndexTypeVector & colindex(void) const {
			return (this->_colindex);
		}
		void colindex(const IndexTypeVector &v) {
			const size_t n = this->cols();
			assert(v.size() >= n);
			IndexTypeVector &vv = this->_colindex;
			vv.resize(n);
			for (size_t i = 0; i < n; ++i) {
				const IndexType val = v[i];
				assert((val >= 0) && (val < (IndexType)n));
				vv[i] = val;
			}// i
		}
		void reset_colindex(void) {
			IndexTypeVector &vr = this->_colindex;
			const size_t n = vr.size();
			for (size_t i = 0; i < n; ++i) {
				vr[i] = (U)i;
			}
		}
	public:
		void shuffle_data(const DataMode mode) {
			if (mode == DataMode::modeRow) {
				this->shuffle_rows();
			}
			else if (mode == DataMode::modeCol) {
				this->shuffle_cols();
			}
		}
		const IndexTypeVector & get_data_index(const DataMode mode) const {
			return ((mode == DataMode::modeRow) ? this->rowindex() : this->colindex());
		}
		void  set_data_index(const DataMode mode, const IndexTypeVector &ind) {
			if (mode == DataMode::modeRow) {
				this->rowindex(ind)
			}
			else if (mode == DataMode::modeCol) {
				this->colindex(ind);
			}
		}
	public:
		virtual DataType value_at(const size_t irow, const size_t icol) const {
			assert(irow < this->_rowindex.size());
			assert(icol < this->_colindex.size());
			return (MatDataType::value_at(this->_rowindex[irow], this->_colindex[icol]));
		}
		virtual void row_at(const size_t irow, DataTypeArray &v) const {
			assert(irow < this->_rowindex.size());
			return (MatDataType::row_at(this->_rowindex[irow], v));
		}// row_at
		virtual void col_at(const size_t icol, DataTypeArray &v) const {
			assert(icol < this->_colindex.size());
			return (MatDataType::col_at(this->_colindex[icol], v));
		}// row_at
	public:
		virtual std::ostream & write_to(std::ostream &os) const {
			os << "row indexes: [";
			auto vv = this->rowindex();
			for (auto it = vv.begin(); it != vv.end(); ++it) {
				if (it != vv.begin()) {
					os << ", ";
				}
				os << (*it);
			}
			os << " ]" << std::endl;
			os << "col indexes: [";
			auto vx = this->colindex();
			for (auto it = vr.begin(); it != rv.end(); ++it) {
				if (it != vr.begin()) {
					os << ", ";
				}
				os << (*it);
			}
			os << " ]" << std::endl;
			return MatDataType::write_to(os);
		}// write_to
		virtual std::wostream & write_to(std::wostream &os) const {
			os << L"row indexes: [";
			auto vv = this->rowindex();
			for (auto it = vv.begin(); it != vv.end(); ++it) {
				if (it != vv.begin()) {
					os << L", ";
				}
				os << (*it);
			}
			os << L" ]" << std::endl;
			os << L"col indexes: [";
			auto vx = this->colindex();
			for (auto it = vr.begin(); it != rv.end(); ++it) {
				if (it != vr.begin()) {
					os << L", ";
				}
				os << (*it);
			}
			os << L" ]" << std::endl;
			return MatDataType::write_to(os);
		}// write_to
	};// class IndexedMatData<T,U>
	////////////////////////////////////
}// namespace info
///////////////////////////////////
template <class OUTPUT, typename T>
OUTPUT & operator<<(OUTPUT &os, const info::MatData<T> &d) {
	return (d.write_to(os));
}
/////////////////////////////////
#endif // !__MATDATA_H__
