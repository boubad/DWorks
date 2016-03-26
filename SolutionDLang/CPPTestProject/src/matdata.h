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
	template <typename T = int, class S = std::wstring> class MatData {
	public:
		typedef T DataType;
		typedef S StringType;
		typedef std::vector<StringType> StringTypeVector;
		typedef std::valarray<T> DataTypeArray;
		typedef MatData<DataType, StringType> MatDataType;
	private:
		size_t _rows;
		size_t _cols;
		const DataTypeArray *_pdata;
		StringTypeVector _rowids;
		StringTypeVector _colids;
	public:
		MatData() :_rows(0), _cols(0), _pdata(nullptr) {}
		MatData(const size_t r, const size_t c, const DataTypeArray *pData,
			const StringTypeVector *pRowNames = nullptr,
			const StringTypeVector *pColNames = nullptr) :
			_rows(r), _cols(c), _pdata(pData), _rowids(r), _colids(c) {
			assert(this->_rows > 0);
			assert(this->_cols > 0);
			assert(this->_pdata != nullptr);
			assert(this->_pdata->size() >= (size_t)(this->_rows * this->_cols));
			if (pRowNames != nullptr) {
				assert(pRowNames->size() == r);
				this->_rowids = *pRowNames;
			}
			if (pColNames != nullptr) {
				assert(pColNames->size() == c);
				this->_colids = *pColNames;
			}
		}
		MatData(const MatDataType &other) :_rows(other._rows), _cols(other._cols), _pdata(other._pdata),
			_rowids(other._rowids), _colids(other._ids) {}
		MatDataType & operator=(const MatDataType &other) {
			if (this != &other) {
				this->_rows = other._rows;
				this->_cols = other._cols;
				this->_pdata = other._pdata;
				this->_rowids = other._rowids;
				this->_colids = other._colids;
			}
			return (*this);
		}
		virtual ~MatData() {}
	public:
		DataType operator()(const size_t irow, const size_t icol) const {
			return (this->value_at(irow, icol));
		}
	public:
		virtual bool is_valid(void) const {
			return ((this->rows() > 0) && (this->cols() > 0) && (this->_pdata != nullptr) &&
				(this->_pdata->size() >= (size_t)(this->cols() * this->rows())) &&
				(this->_rowids.size() == this->_rows) && (this->_colids.size() == this->_cols));
		}
		virtual size_t rows(void) const {
			return (this->_rows);
		}
		virtual size_t cols(void) const {
			return (this->_cols);
		}
	public:
		virtual const StringType &row_name(const size_t i) const {
			return ((this->_rowids)[i]);
		}
		virtual const StringType &col_name(const size_t i) const {
			return ((this->_colids)[i]);
		}
		virtual DataType value_at(const size_t irow, const size_t icol) const {
			return ((*(this->_pdata))[irow * this->cols() + icol]);
		}
		virtual void row_at(const size_t irow, DataTypeArray &v) const {
			v = (*(this->_pdata))[std::slice(irow * this->cols(), this->cols(), 1)];
		}// row_at
		virtual void col_at(const size_t icol, DataTypeArray &v) const {
			v = (*(this->_pdata))[std::slice(icol, this->rows(), this->cols())];
		}// row_at
	public:
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
		bool discretize_data(std::valarray<int> &vRet, const size_t nc = 7) const {
			assert(nc > 2);
			assert(this->is_valid());
			const size_t nRows = this->rows();
			if (nRows < nc) {
				return (false);
			}
			const size_t nCols = this->cols();
			const size_t nTotal = (size_t)(nCols * nRows);
			vRet.resize(nTotal);
			size_t nClasses = nc;
			for (size_t icol = 0; icol < nCols; ++icol) {
				DataTypeArray v;
				this->col_at(icol, v);
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
		template <typename X>
		bool normalize_data(std::valarray<X> &vRet) const {
			assert(this->is_valid());
			const size_t nRows = this->rows();
			if (nRows < 3) {
				return (false);
			}
			const size_t nCols = this->cols();
			const size_t nTotal = (size_t)(nCols * nRows);
			vRet.resize(nTotal);
			for (size_t icol = 0; icol < nCols; ++icol) {
				DataTypeArray v;
				std::valarray<double> dv(nRows);
				this->col_at(icol, v);
				for (size_t i = 0; i < nRows; ++i) {
					dv[i] = v[i];
				}
				const double moy = (double)(dv.sum() / nRows);
				std::valarray<double> t = dv - moy;
				std::valarray<double> tt = t * t;
				const double ecart2 = (tt.sum()) / (nRows - 1);
				if (ecart2 <= 0.0) {
					return false;
				}
				const double ecart = std::sqrt(ecart2);
				std::valarray<double> xt = t / ecart;
				for (size_t irow = 0; irow < nRows; ++irow) {
					const size_t pos = (size_t)(irow * nCols + icol);
					vRet[pos] = (X)xt[irow];
				}// irow
			}// icol
			return (true);
		}// normalize_data
		template <typename X>
		bool recode_data(std::valarray<X> &vRet, const X xMax = (X)1000, const X xMin = (X)0) const {
			assert(xMin < xMax);
			assert(this->is_valid());
			const size_t nRows = this->rows();
			if (nRows < 3) {
				return (false);
			}
			const size_t nCols = this->cols();
			const size_t nTotal = (size_t)(nCols * nRows);
			vRet.resize(nTotal);
			for (size_t icol = 0; icol < nCols; ++icol) {
				DataTypeArray v;
				this->col_at(icol, v);
				const DataType vMin = v.min();
				const DataType vMax = v.max();
				if (vMin >= vMax) {
					return (false);
				}
				const double delta = (double)(xMax - xMin) / (double)(vMax - vMin);
				std::valarray<double> dv(nRows);
				for (size_t i = 0; i < nRows; ++i) {
					double xx = delta * (v[i] - (double)vMin) + (double)xMin;
					const size_t pos = (size_t)(i * nCols + icol);
					vRet[pos] = (X)xx;
				}
			}// icol
			return (true);
		}// recode_data
	public:
		const StringType & data_id_at(const size_t ipos, const DataMode mode) {
			assert(ipos < this->get_data_count(mode));
			if (mode == DataMode::modeRow) {
				return (this->row_name(ipos));
			}
			else if (mode == DataMode::modeCol) {
				return (this->col_name(ipos));
			}
			static StringType t;
			return t;
		}
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
		virtual std::wostream & write_to(std::wostream &os) const {
			const size_t nRows = this->rows();
			const size_t nCols = this->cols();
			for (size_t i = 0; i < nCols; ++i) {
				os << L"\t" << this->col_name(i);
			}// i
			os << std::endl;
			for (size_t i = 0; i < nRows; ++i) {
				os << this->row_name(i);
				for (size_t j = 0; j < nCols; ++j) {
					os << L"\t" << this->value_at(i, j);
				}// j
				os << std::endl;
			}// i
			return os;
		}// write_to
	};// class MatData<T,S>
	//////////////////////////////
	template <typename T = int, typename U = int, class S = std::wstring> class IndexedMatData : public MatData<T, S> {
	public:
		typedef T DataType;
		typedef U IndexType;
		typedef S StringType;
		typedef MatData<DataType, StringType> MatDataType;
		typedef std::vector<IndexType> IndexTypeVector;
		typedef IndexedMatData<DataType, IndexType, StringType> IndexedMatDataType;
	private:
		const MatDataType *_pmat;
		IndexTypeVector _rowindex;
		IndexTypeVector _colindex;
	public:
		IndexedMatData() :_pmat(nullptr) {}
		IndexedMatData(const MatDataType *pMat) :_pmat(pMat) {
			assert(this->_pmat != nullptr);
			assert(this->_pmat->is_valid());
			const size_t c = this->_pmat->cols();
			const size_t r = this->_pmat->rows();
			IndexTypeVector &vr = this->_rowindex;
			vr.resize(r);
			for (size_t i = 0; i < r; ++i) {
				vr[i] = (U)i;
			}
			IndexTypeVector &vc = this->_colindex;
			vc.resize(c);
			for (size_t i = 0; i < c; ++i) {
				vc[i] = (U)i;
			}
		}
		IndexedMatData(const IndexedMatDataType &other) :MatDataType(other), _pmat(other._pmat),
			_rowindex(other._rowindex), _colindex(other._colindex) {}
		IndexedMatDataType & operator=(const IndexedMatDataType &other) {
			if (this != &other) {
				MatDataType::operator=(other);
				this->_pmat = other._pmat;
				this->_rowindex = other._rowindex;
				this->_colindex = other._colindex;
			}
			return (*this);
		}
		virtual ~IndexedMatData() {}
	public:
		virtual bool is_valid(void) const {
			return ((this->_pmat != nullptr) && (this->_pmat->is_valid()) &&
				(this->_rowindex.size() == this->_pmat->rows()) &&
				(this->_colindex.size() == this->_pmat->cols()));
		}// is_valid
		virtual size_t rows(void) const {
			assert(this->_pmat != nullptr);
			return (this->_pmat->rows());
		}
		virtual size_t cols(void) const {
			assert(this->_pmat != nullptr);
			return (this->_pmat->cols());
		}
		virtual const StringType &row_name(const size_t i) const {
			assert(this->is_valid());
			assert(i < this->_rowindex.size());
			return this->_pmat->row_name(this->_rowindex[i]);
		}
		virtual const StringType &col_name(const size_t i) const {
			assert(this->is_valid());
			assert(i < this->_colindex.size());
			return this->_pmat->col_name(this->_colindex[i]);
		}
		virtual DataType value_at(const size_t irow, const size_t icol) const {
			assert(this->is_valid());
			assert(irow < this->_rowindex.size());
			assert(icol < this->_colindex.size());
			return this->_pmat->value_at(this->_rowindex[irow], this->_colindex[icol]);
		}
		virtual void row_at(const size_t irow, DataTypeArray &v) const {
			assert(this->is_valid());
			assert(irow < this->_rowindex.size());
			return this->_pmat->row_at(this->_rowindex[irow], v);
		}// row_at
		virtual void col_at(const size_t icol, DataTypeArray &v) const {
			assert(this->is_valid());
			assert(icol < this->_colindex.size());
			return this->_pmat->col_at(this->_colindex[icol], v);
		}// row_at
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
			for (auto it = vx.begin(); it != vx.end(); ++it) {
				if (it != vx.begin()) {
					os << L", ";
				}
				os << (*it);
			}
			os << L" ]" << std::endl;
			return MatDataType::write_to(os);
		}// write_to
	};// class IndexedMatData<T,U,S>
	////////////////////////////////////
}// namespace info
/////////////////////////////////////////
template <typename T, class S>
std::ostream & operator<<(std::ostream &os, info::MatData<T, S> &d) {
	return d.write_to(os);
}
template <typename T, class S>
std::wostream & operator<<(std::wostream &os, info::MatData<T, S> &d) {
	return d.write_to(os);
}
///////////////////////////////////
template <typename T, typename U, class S>
std::ostream & operator<<(std::ostream &os, info::IndexedMatData<T, U, S> &d) {
	return d.write_to(os);
}
template <typename T, typename U, class S>
std::wostream & operator<<(std::wostream &os, info::IndexedMatData<T, U, S> &d) {
	return d.write_to(os);
}
///////////////////////////////////
#endif // !__MATDATA_H__
