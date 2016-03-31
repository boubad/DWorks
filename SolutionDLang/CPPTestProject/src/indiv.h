#pragma once
#ifndef __INDIV_H__
#define __INDIV_H__
///////////////////////////
#include "utils.h"
#include "distance.h"
////////////////////////////////
namespace info {
	template <typename T = int, typename U = int> class Indiv {
		static_assert(std::is_integral<U>::value, "index typename must be integral type");
	public:
		typedef T DataType;
		typedef U IndexType;
		//
		typedef std::valarray<DataType> DataTypeArray;
		typedef std::vector<DataType> DataTypeVector;
		typedef Indiv<DataType, IndexType> IndivType;
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
			return ((this->index() == other.index()) ||
				(this->id() == other.id()));
		}
		bool operator<(const IndivType &other) const {
			if (this->index() < other.index()) {
				return (true);
			}
			else if (this->index() > other.index()) {
				return (false);
			}
			return (this->id() < other.id());
		}
	public:
		virtual bool is_valid(void) const {
			return ((this->index() >= 0) && (this->size() > 0));
		}
		virtual const DataTypeArray & value(void) const {
			return (this->_data);
		}
		virtual void value(const DataTypeArray &oAr) {
			this->_data = oAr;
		}
		virtual IndexType index(void) const {
			return (this->_index);
		}
		virtual void index(const IndexType aIndex) {
			this->_index = aIndex;
		}
		virtual const StringType & id(void) const {
			return (this->_id);
		}
		virtual void id(const StringType &s) {
			this->_id = s;
		}
		//
		size_t size(void) const {
			return (this->value().size());
		}
		void value(DataTypeVector &oVec) const {
			const DataTypeArray &oAr = this->value();
			const size_t n = oAr.size();
			oVec.resize(n);
			for (size_t i = 0; i < n; ++i) {
				oVec[i] = oAr[i];
			}
		}
		void value(const DataTypeVector &oVec) {
			const size_t n = oVec.size();
			DataTypeArray dx(n);
			for (size_t i = 0; i < n; ++i) {
				dx[i] = oVec[i];
			}
			this->value(dx);
		}
		double norm(void) const {
			const DataTypeArray &t = this->value();
			double res = 0;
			const size_t n = t.size();
			for (size_t i = 0; i < n; ++i) {
				double tx = t[i];
				res += tx * tx;
			}
			if (n > 0) {
				res /= n;
			}
			return (std::sqrt(res));
		}// trace
		void toString(StringType &s) const {
			StringStreamType os;
			this->write_to(os);
			s = os.str();
		}// toString
		template <typename Z>
		void distance(const DataTypeArray &vv1, Z &result,
			const DistanceFunc<T, Z> *pFunc = nullptr) const {
			info_distance(this->value(), vv1, result, pFunc);
		}// distance
		template <typename Z>
		void distance(const DataTypeVector &vv1, Z &result,
			const DistanceFunc<T, Z> *pFunc = nullptr) const {
			info_distance(this->value(), vv1, result, pFunc);
		}// distance
		template <typename Z>
		void distance(const IndivType &other, Z &result,
			const DistanceFunc<T, Z> *pFunc = nullptr) const {
			info_distance(this->value(), other.value(), result, pFunc);
		}// distance
		template <typename Z>
		void distance(const IndivTypePtr &other, Z &result,
			const DistanceFunc<T, Z> *pFunc = nullptr) const {
			const IndivType *p = other.get();
			assert(p != nullptr);
			this->distance(*p, result, pFunc);
		}// distance
	public:
		virtual OStreamType & write_to(OStreamType &os) const {
			os << START_OBJECT << this->index() << STRING_COMMA << this->id() << STRING_COMMA;
			info_write_array(os, this->value());
			os << END_OBJECT;
			return (os);
		}// write_to
	}; // class Indiv<T,U>
	   /////////////////////////
}// namespace info
///////////////////////////////////
template <typename T, typename U>
info::OStreamType & operator<<(info::OStreamType &os, const info::Indiv<T, U> &d) {
	return (d.write_to(os));
}
///////////////////////////////////
#endif // !__INDIV_H__
///////////////////////
//eof: indiv.h
