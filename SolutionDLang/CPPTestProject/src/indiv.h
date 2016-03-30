#pragma once
#ifndef __INDIV_H__
#define __INDIV_H__
///////////////////////////
#include <cassert>
#include <valarray>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <memory>
//////////////////////////////////
#include "distance.h"
////////////////////////////////
namespace info {
	template <typename T = int, typename U = int, class S = std::wstring> class Indiv {
	public:
		typedef T DataType;
		typedef U IndexType;
		typedef S StringType;
		//
		typedef std::valarray<DataType> DataTypeArray;
		typedef std::vector<DataType> DataTypeVector;
		typedef Indiv<DataType, IndexType, StringType> IndivType;
		typedef std::shared_ptr<IndivType> IndivTypePtr;
	private:
		IndexType _index;
		StringType _id;
		DataTypeArray _data;
	public:
		Indiv() :_index(0) {}
		Indiv(const IndexType ii) :_index(ii) {}
		Indiv(const IndexType ii, const StringType &sid) :_index(ii), _id(sid) {}
		Indiv(const IndexType ii, const DataTypeArray &oAr) :_index(ii), _data(oAr) {}
		Indiv(const IndexType ii, const DataTypeVector &oVec) :_index(ii) {
			const size_t n = oVec.size();
			DataTypeArray & w = this->_data;
			w.resize(n);
			for (size_t i = 0; i < n; ++i) {
				w[i] = oVec[i];
			}// i
		}
		Indiv(const IndexType ii, const StringType &sid, const DataTypeArray &oAr) :
			_index(ii), _id(sid), _data(oAr) {}
		Indiv(const IndexType ii, const StringType &sid, const DataTypeVector &oVec) :
			_index(ii), _id(sid) {
			const size_t n = oVec.size();
			DataTypeArray & w = this->_data;
			w.resize(n);
			for (size_t i = 0; i < n; ++i) {
				w[i] = oVec[i];
			}// i
		}
		Indiv(const IndivType &other) :_index(other._index), _id(other._id), _data(other._data) {}
		IndivType & operator=(const IndivType &other) {
			if (this != &other) {
				this->_index = other._index;
				this->_id = other._id;
				this->_data = other._data;
			}
			return (*this);
		}
		virtual ~Indiv() {}
	public:
		bool operator==(const IndivType &other) const {
			return (this->trace() == other.trace());
		}
		bool operator<(const IndivType &other) const {
			return (this->trace() < other.trace());
		}
	public:
		bool is_valid(void) const {
			return ((this->_index >= 0) && (this->_data.size() > 0));
		}
		IndexType index(void) const {
			return (this->_index);
		}
		void index(const IndexType aIndex) {
			assert(aIndex >= 0);
			this->_index = aIndex;
		}
		const StringType & id(void) const {
			return (this->_id);
		}
		void id(const StringType &s) {
			this->_id = s;
		}
		const DataTypeArray & value(void) const {
			return (this->_data);
		}
		void value(const DataTypeArray &oAr) {
			this->_data = oAr;
		}
		void value(DataTypeVector &oVec) const {
			const DataTypeArray &oAr = this->_data;
			const size_t n = oAr.size();
			oVec.resize(n);
			for (size_t i = 0; i < n; ++i) {
				oVec[i] = oAr[i];
			}
		}
		void value(const DataTypeVector &oVec) {
			DataTypeArray &oAr = this->_data;
			const size_t n = oVec.size();
			oAr.resize(n);
			for (size_t i = 0; i < n; ++i) {
				oAr[i] = oVec[i];
			}
		}
		size_t size(void) const {
			return (this->_data.size());
		}
		double trace(void) const {
			DataTypeArray t = this->_data;
			DataTypeArray tt = t * t;
			return (double)tt.sum();
		}// trace
		void toString(std::string &s) const {
			std::stringstream os;
			this->write_to(os);
			s = os.str();
		}// toString
		void toString(std::wstring &s) const {
			std::wstringstream os;
			this->write_to(os);
			s = os.str();
		}// toString
		template <typename Z>
		static void distance(const DataTypeArray &vv1, const DataTypeArray &vv2, Z &result,
			const DistanceFunc<T, Z> *pFunc = nullptr)  {
			if (pFunc == nullptr) {
				EuclideDistanceFunc<T, Z> f;
				result = f(vv1, vv2);
			}
			else {
				result = (*pFunc)(vv1, vv2);
			}
		}// distance
		template <typename Z>
		static void distance(const DataTypeVector &vv1, const DataTypeVector &vv2, Z &result,
			const DistanceFunc<T, Z> *pFunc = nullptr)  {
			if (pFunc == nullptr) {
				EuclideDistanceFunc<T, Z> f;
				result = f(vv1, vv2);
			}
			else {
				result = (*pFunc)(vv1, vv2);
			}
		}// distance
		template <typename Z>
		void distance(const DataTypeArray &vv1, Z &result,
			const DistanceFunc<T, Z> *pFunc = nullptr) const {
			if (pFunc == nullptr) {
				EuclideDistanceFunc<T, Z> f;
				result = f(this->value(), vv1);
			}
			else {
				result = (*pFunc)(this->value(), vv1);
			}
		}// distance
		template <typename Z>
		void distance(const DataTypeVector &vv1, Z &result,
			const DistanceFunc<T, Z> *pFunc = nullptr) const {
			if (pFunc == nullptr) {
				EuclideDistanceFunc<T, Z> f;
				result = f(this->value(), vv1);
			}
			else {
				result = (*pFunc)(this->value(), vv1);
			}
		}// distance
		template <typename Z>
		void distance(const IndivType &other, Z &result,
			const DistanceFunc<T, Z> *pFunc = nullptr) const {
			if (pFunc == nullptr) {
				EuclideDistanceFunc<T, Z> f;
				result = f(this->value(), other.value());
			}
			else {
				result = (*pFunc)(this->value(), other.value());
			}
		}// distance
		template <typename Z>
		void distance(const IndivTypePtr &other, Z &result,
			const DistanceFunc<T, Z> *pFunc = nullptr) const {
			const IndivType *p = other.get();
			if (p == nullptr) {
				result = 0;
			}
			else {
				this->distance(*p, result, pFunc);
			}
		}// distance
	public:
		virtual std::wostream & write_to(std::wostream &os) const {
			os << L"{" << this->_index << L",\t" << this->id() << L",\t[";
			const size_t n = this->_data.size();
			for (size_t i = 0; i < n; ++i) {
				if (i > 0) {
					os << L", ";
				}
				os << this->_data[i];
			}// i
			os << L"] }";
			return (os);
		}// write_to
	}; // class Indiv<T,U,S>
	   /////////////////////////
}// namespace info
///////////////////////////////////
template <typename T, typename U, class S>
std::ostream & operator<<(std::ostream &os, const info::Indiv<T, U, S> &d) {
	return (d.write_to(os));
}
template <typename T, typename U, class S>
std::wostream & operator<<(std::wostream &os, const info::Indiv<T, U, S> &d) {
	return (d.write_to(os));
}
///////////////////////////////////
#endif // !__INDIV_H__
///////////////////////
//eof: indiv.h
