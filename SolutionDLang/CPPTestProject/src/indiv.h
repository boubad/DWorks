#pragma once
#ifndef __INDIV_H__
#define __INDIV_H__
///////////////////////////
#include <cassert>
#include <valarray>
#include <string>
#include <iostream>
#include <sstream>
//////////////////////////////////
#include "distance.h"
////////////////////////////////
namespace info {
	template <typename T = int, typename U = int> class Indiv {
	public:
		typedef T DataType;
		typedef U IndexType;
		//
		typedef std::valarray<DataType> DataTypeArray;
		typedef Indiv<DataType, IndexType> IndivType;
	private:
		IndexType _index;
		DataTypeArray _data;
	public:
		Indiv() :_index(0) {}
		Indiv(const IndexType ii) :_index(ii) {}
		Indiv(const IndexType ii, const DataTypeArray &oAr) :_index(ii), _data(oAr) {}
		Indiv(const IndivType &other) :_index(other._index), _data(other._data) {}
		IndivType & operator=(const IndivType &other) {
			if (this != &other) {
				this->_index = other._index;
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
			assert(oIndex >= 0);
			this->_index = aIndex;
		}
		const DataTypeArray & value(void) const {
			return (this->_data);
		}
		void value(const DataTypeArray &oAr) {
			this->_data = oAr;
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
		void distance(const DataTypeArray &vv1, const DataTypeArray &vv2, Z &result,
			const DistanceFunc<T, Z> *pFunc = nullptr) const {
			if (pFunc == nullptr) {
				ManhattanDistanceFunc<T, Z> f;
				res = f(vv1, vv2);
			}
			else {
				result = (*pFunc)(vv1, vv2);
			}
			
		}// distance
		template <typename Z>
		void distance(const IndivType &other, Z &result,
			const DistanceFunc<T, Z> *pFunc = nullptr) const {
			if (pFunc == nullptr) {
				ManhattanDistanceFunc<T, Z> f;
				result = f(this->_data, other._data);
			}
			else {
				result = (*pFunc)(this->_data, other._data);
			}
			
		}// distance
	public:
		std::ostream & write_to(std::ostream &os) const {
			os << "{" << this->_index << " ,[";
			const size_t n = this->_data.size();
			for (size_t i = 0; i < n; ++i) {
				if (i > 0) {
					os << ", ";
				}
				os << this->_data[i];
			}// i
			os << "]";
			return os;
		}// write_to
		std::wostream & write_to(std::wostream &os) const {
			os << L"{" << this->_index << L" ,[";
			const size_t n = this->_data.size();
			for (size_t i = 0; i < n; ++i) {
				if (i > 0) {
					os << L", ";
				}
				os << this->_data[i];
			}// i
			os << L"] }";
			return os;
		}// write_to
	}; // class Indiv<T,U>
	   /////////////////////////
}// namespace info
#endif // !__INDIV_H__
///////////////////////
//eof: indiv.h
