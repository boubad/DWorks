#pragma once
#ifndef __INDIVSET_H__
#define __INDIVSET_H__
/////////////////////////////////
#include <memory>
#include <set>
#include <vector>
///////////////////////////////////
#include "indiv.h"
/////////////////////////////////////
namespace info {
	///////////////////////////////////////
	template <typename T = int, typename U = int, class S = std::wstring> class IndivSet : public Indiv<T, U, S> {
	public:
		typedef T DataType;
		typedef U IndexType;
		typedef S StringType;
		typedef std::valarray<T> DataTypeArray;
		typedef Indiv<T, U, S> IndivType;
		typedef std::shared_ptr<IndivType> IndivTypePtr;
		typedef std::vector<IndivTypePtr> IndivTypePtrVector;
		typedef IndivSet<T, U, S> IndivSetType;
	private:
		IndivTypePtrVector _points;
		std::vector<double> _sum;
	public:
		IndivSet() {}
		IndivSet(const IndexType aIndex) :IndivType(aIndex) {}
		IndivSet(const IndexType aIndex, const StringType &sid) :IndivType(aIndex, sid) {}
		IndivSet(const IndexType aIndex, const DataTypeArray &oCenter) :
			IndivType(aIndex, oCenter) {}
		IndivSet(const IndexType aIndex, const StringType &sid, const DataTypeArray &oCenter) :
			IndivType(aIndex, sid, oCenter) {}
		IndivSet(const IndivSetType &other) :IndivType(other),
			_points(other._points), _sum(other._sum) {}
		IndivSetType & operator=(const IndivSetType &other) {
			if (this != &other) {
				IndivType::operator=(other);
				this->_points = other._points;
				this->_sum = other._sum;
			}
			return (*this);
		}
		virtual ~IndivSet() {}
	public:
		bool empty(void) const {
			return (this->_points.empty());
		}
	public:
		const IndivTypePtrVector & members(void) const {
			return (this->_points);
		}
		void reset(void) {
			this->_points.clear();
		}// reset
		void update_center(void) {
			std::vector <double>  &sum = this->_sum;
			const size_t n = sum.size();
			const size_t nTotal = this->_points.size();
			if ((n > 0) && (nTotal > 0)) {
				DataTypeArray val(n);
				for (size_t i = 0; i < n; ++i) {
					val[i] = (T)(sum[i] / nTotal);
				}// i
				this->value(val);
			}// n
		}// update_centers
		bool contains(const IndexType aIndex)  const {
			bool bFound = false;
			auto vv = this->_points;
			auto vend = vv.end();
			for (auto it = vv.begin(); it != vend; ++it) {
				IndivTypePtrVector o = *it;
				const IndivType *p = o.get();
				assert(p != nullptr);
				if (p->index() == aIndex) {
					bFound = true;
					break;
				}
			}// it
			return (bFound);
		}// contains
		bool contains(const StringType &sid)  const {
			if (sid.empty()) {
				return (false);
			}
			bool bFound = false;
			auto vv = this->_points;
			auto vend = vv.end();
			for (auto it = vv.begin(); it != vend; ++it) {
				IndivTypePtrVector o = *it;
				const IndivType *p = o.get();
				assert(p != nullptr);
				if (p->id() == sid) {
					bFound = true;
					break;
				}
			}// it
			return (bFound);
		}// contains
		bool contains(const IndivType *pIndiv)  const {
			if (pIndiv == nullptr) {
				return (false);
			}
			bool bFound = false;
			auto vv = this->_points;
			auto vend = vv.end();
			const StringType &sid = pIndiv->id();
			const IndexType aIndex = pIndiv->index();
			for (auto it = vv.begin(); it != vend; ++it) {
				IndivTypePtrVector o = *it;
				const IndivType *p = o.get();
				assert(p != nullptr);
				if (p->index() == aIndex) {
					bFound = true;
					break;
				}
				if ((!sid.empty()) && (p->id() == sid)) {
					bFound = true;
					break;
				}
			}// it
			return (bFound);
		}// contains
		bool contains(const IndivType &oIndiv)  const {
			return (this->contains(&oIndiv));
		}// contains
		bool contains(const IndivTypePtr &oIndiv)  const {
			return (this->contains(oIndiv.get()));
		}// contains
		//
		bool add(const IndivTypePtr &oIndiv, bool bUpdate = false) {
			if (this->contains(oIndiv)) {
				return (false);
			}
			const IndivType *pIndiv = oIndiv.get();
			assert(pIndiv != nullptr);
			if (!pIndiv->is_valid()) {
				return (false);
			}
			const DataTypeArray &v = pIndiv->value();
			const size_t n = v.size();
			IndivTypePtrVector &vec = this->_points;
			std::vector<double> &sum = this->_sum;
			if (vec.empty()) {
				sum.resize(n);
				for (size_t i = 0; i < n; ++i) {
					sum[i] = (double)v[i];
				}
			}
			else {
				const size_t nn = sum.size();
				assert(n >= nn);
				for (size_t i = 0; i < nn; ++i) {
					sum[i] += (double)v[i];
				}// i
			}
			IndivTypePtr oo(oIndiv);
			vec.push_back(oo);
			if (bUpdate) {
				this->update_center();
			}
			return true;
		}// add
		bool add(const IndivType &oIndiv, bool bUpdate = false) {
			if (this->contains(oIndiv)) {
				return false;
			}
			IndivTypePtr oo = std::make_shared<IndivType>(oIndiv);
			return this->add(oo, bUpdate);
		}// add
		bool add(const IndivType *pIndiv, bool bUpdate = false) {
			if (pIndiv == nullptr) {
				return (false);
			}
			if (!pIndiv->is_valid()) {
				return (false)
			}
			if (this->contains(pIndiv)) {
				return false;
			}
			return this->add(*pIndiv, bUpdate);
		}// add
		bool add(const IndexType aIndex, const DataTypeArray &v, bool bUpdate = false) {
			if (this->contains(aIndex)) {
				return (false);
			}
			IndivTypePtr oo = std::make_shared<IndivType>(aIndex, v);
			return this->add(oo, bUpdate);
		}// add
		bool add(const IndexType aIndex, const StringType &sid, const DataTypeArray &v, bool bUpdate = false) {
			if (this->contains(aIndex)) {
				return (false);
			}
			IndivTypePtr oo = std::make_shared<IndivType>(aIndex, sid, v);
			return this->add(oo, bUpdate);
		}// add
	public:
		virtual std::ostream & write_to(std::ostream &os) const {
			os << "{" << std::endl;
			os << "\tindex: " << this->index() << std::endl;
			os << "\tid: " << std::endl;
			os << "\ttrace: " << this->trace() << std::endl;
			os << "\tcenter: ";
			IndivType::write_to(os);
			os << std::endl;
			os << "\tmembers: ";
			auto vv = this->_points;
			for (auto it = vv.begin(); it != vv.end(); ++it) {
				os << "\t\t";
				IndivTypePtr o = *it;
				const IndivType *p = o.get();
				assert(p != nullptr);
				p->write_to(os);
				os << std::endl;
			}// it
			os << "}";
			return os;
		}// write_to
		virtual std::wostream & write_to(std::wostream &os) const {
			os << L"{" << std::endl;
			os << L"\tindex: " << this->index() << std::endl;
			os << L"\tid: " << std::endl;
			os << L"\ttrace: " << this->trace() << std::endl;
			os << L"\tcenter: ";
			IndivType::write_to(os);
			os << std::endl;
			os << L"\tmembers: ";
			auto vv = this->_points;
			for (auto it = vv.begin(); it != vv.end(); ++it) {
				os << L"\t\t";
				IndivTypePtr o = *it;
				const IndivType *p = o.get();
				assert(p != nullptr);
				p->write_to(os);
				os << std::endl;
			}// it
			os << L"}";
			return os;
		}// write_to
	}; // class IndivSet<T,U,S>
	//////////////////////////////////////////
}// namespace info
/////////////////////////////////
#endif // !__INDIVSET_H__
