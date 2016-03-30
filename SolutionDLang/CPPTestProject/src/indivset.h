#pragma once
#ifndef __INDIVSET_H__
#define __INDIVSET_H__
/////////////////////////////////
#include <set>
#include <deque>
///////////////////////////////////
#include "indiv.h"
/////////////////////////////////////////
enum class DistanceMode { modeInvalid, modeCenter, modeUpUp, modeUpDown, modeDownUp, modeDownDown }; // enum DistanceMode
/////////////////////////////////////
namespace info {
	///////////////////////////////////////
	template <typename T = int, typename U = int, class S = std::wstring> class IndivSet : public Indiv<T, U, S> {
	public:
		typedef T DataType;
		typedef U IndexType;
		typedef S StringType;
		typedef std::valarray<T> DataTypeArray;
		typedef std::vector<T> DataTypeVector;
		typedef Indiv<DataType, IndexType, StringType> IndivType;
		typedef std::shared_ptr<IndivType> IndivTypePtr;
		typedef std::deque<IndivTypePtr> IndivTypePtrVector;
		typedef IndivSet<DataType, IndexType, StringType> IndivSetType;
		typedef std::shared_ptr<IndivSetType> IndivSetTypePtr;
		typedef std::vector<IndivSetTypePtr> IndivSetTypePtrVector;
	private:
		IndivTypePtrVector _points;
		std::vector<double> _sum;
	public:
		IndivSet() {}
		IndivSet(const IndexType aIndex) :IndivType(aIndex) {}
		IndivSet(const IndexType aIndex, const StringType &sid) :IndivType(aIndex, sid) {}
		IndivSet(const IndexType aIndex, const DataTypeArray &oCenter) :
			IndivType(aIndex, oCenter) {}
		IndivSet(const IndexType aIndex, const DataTypeVector &oCenter) :
			IndivType(aIndex, oCenter) {}
		IndivSet(const IndexType aIndex, const StringType &sid, const DataTypeArray &oCenter) :
			IndivType(aIndex, sid, oCenter) {}
		IndivSet(const IndexType aIndex, const StringType &sid, const DataTypeVector &oCenter) :
			IndivType(aIndex, sid, oCenter) {}
		IndivSet(const IndivTypePtr &oInd) {
			IndivTypePtr oo(oInd);
			const IndivType *pInd = oo.get();
			assert(pInd != nullptr);
			assert(pInd->is_valid());
			this->index(pInd->index());
			this->id(pInd->id());
			DataTypeArray val = pInd->value();
			this->value(val);
			this->_points.push_back(oo);
			const size_t n = val.size();
			this->_sum.resize(n);
			for (size_t i = 0; i < n; ++i) {
				this->_sum[i] = (double)val[i];
			}
		}
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
		void merge(const IndivSetType &other, const DistanceMode mode) {
			assert(!(this->empty()));
			assert(!(other.empty()));
			IndivTypePtrVector &dest = this->_points;
			const IndivTypePtrVector &src = other._points;
			const size_t n = src.size();
			switch (mode) {
			case DistanceMode::modeUpUp:
			{
				for (size_t i = 0; i < n; ++i) {
					dest.push_front(src[i]);
				}// i
			}
			break;
			case DistanceMode::modeUpDown:
			{
				for (size_t i = (size_t)(n - 1); i >= 0; i--) {
					dest.push_front(src[i]);
					if (i == 0) {
						break;
					}
				}
			}
			break;
			case DistanceMode::modeDownUp:
			{
				for (size_t i = 0; i < n; ++i) {
					dest.push_back(src[i]);
				}// i
			}
			break;
			case DistanceMode::modeDownDown:
			{
				for (size_t i = (size_t)(n - 1); i >= 0; i--) {
					dest.push_back(src[i]);
					if (i == 0) {
						break;
					}
				}
			}
			break;
			default:
				// should not fall here!
				assert(false);
				break;
			}// mode
			const size_t nTotal = dest.size();
			assert(nTotal > 1);
			IndivType *p = (dest[0]).get();
			assert(p != nullptr);
			DataTypeArray oAr = p->value();
			const size_t nx = oAr.size();
			assert(nx > 0);
			std::vector<double> &somme = this->_sum;
			somme.resize(nx);
			for (size_t i = 0; i < nx; ++i) {
				somme[i] = (double)oAr[i];
			}
			for (size_t i = 1; i < nTotal; ++i) {
				IndivTypePtr o = dest[i];
				IndivType *px = o.get();
				assert(px != nullptr);
				DataTypeArray aa = px->value();
				assert(aa.size() == nx);
				for (size_t j = 0; j < nx; ++j) {
					somme[j] += aa[j];
				}// j
			}// i
			DataTypeArray oCenter(nx);
			for (size_t i = 0; i < nx; ++i) {
				oCenter[i] = (DataType)(somme[i] / nTotal);
			}// i
			this->value(oCenter);
		}// merge
		template <typename Z>
		Z find_min_distance(const IndivSetType &other, DistanceMode &mode, Z & /*dummy*/,
			const DistanceFunc<DataType, Z> *pFunc = nullptr) const {
			assert(!(this->empty()));
			assert(!(other.empty()));
			//
			mode = DistanceMode::modeUpUp;
			Z d = 0;
			Z dMin = this->compute_distance(other, DistanceMode::modeUpUp, pFunc);
			const size_t n1 = this->members_size();
			const size_t n2 = other.members_size();
			if ((n1 < 2) && (n2 < 2)) {
				return (dMin);
			}
			else if ((n1 < 2) && (n2 > 1)) {
				d = this->compute_distance(other, DistanceMode::modeUpDown, pFunc);
				if (d < dMin) {
					mode = DistanceMode::modeUpDown;
					return (d);
				}
			}
			else  if ((n1 > 1) && (n2 < 2)) {
				d = this->compute_distance(other, DistanceMode::modeDownUp, pFunc);
				if (d < dMin) {
					mode = DistanceMode::modeDownUp;
					return (d);
				}
			}
			d = this->compute_distance(other, DistanceMode::modeUpDown, pFunc);
			if (d < dMin) {
				dMin = d;
				mode = DistanceMode::modeUpDown;
			}
			d = this->compute_distance(other, DistanceMode::modeDownUp, pFunc);
			if (d < dMin) {
				dMin = d;
				mode = DistanceMode::modeDownUp;
			}
			d = this->compute_distance(other, DistanceMode::modeDownDown, pFunc);
			if (d < dMin) {
				dMin = d;
				mode = DistanceMode::modeDownDown;
			}
			return (dMin);
		}// find_min_distance
		template <typename Z>
		Z compute_distance(const IndivSetType &other,
			const DistanceMode mode = DistanceMode::modeCenter,
			const DistanceFunc<DataType, Z> *pFunc = nullptr) const {
			Z res = 0;
			assert(mode != DistanceMode::modeInvalid);
			switch (mode) {
			case DistanceMode::modeCenter:
			{
				assert(this->is_valid());
				assert(other.is_valid());
				IndivType::distance(this->value(), other.value(), res, pFunc);
			}
			break;
			case DistanceMode::modeUpUp:
			{
				assert(!(this->empty()));
				assert(!(other.empty()));
				IndivTypePtr o1 = this->_points.front();
				const IndivType *p1 = o1.get();
				assert(p1 != nullptr);
				IndivTypePtr o2 = other._points.front();
				const IndivType *p2 = o2.get();
				assert(p2 != nullptr);
				IndivType::distance(p1->value(), p2->value(), res, pFunc);
			}
			break;
			case DistanceMode::modeUpDown:
			{
				assert(!(this->empty()));
				assert(!(other.empty()));
				IndivTypePtr o1 = this->_points.front();
				const IndivType *p1 = o1.get();
				assert(p1 != nullptr);
				IndivTypePtr o2 = other._points.back();
				const IndivType *p2 = o2.get();
				assert(p2 != nullptr);
				IndivType::distance(p1->value(), p2->value(), res, pFunc);
			}
			break;
			case DistanceMode::modeDownUp:
			{
				assert(!(this->empty()));
				assert(!(other.empty()));
				IndivTypePtr o1 = this->_points.back();
				const IndivType *p1 = o1.get();
				assert(p1 != nullptr);
				IndivTypePtr o2 = other._points.front();
				const IndivType *p2 = o2.get();
				assert(p2 != nullptr);
				IndivType::distance(p1->value(), p2->value(), res, pFunc);
			}
			break;
			case DistanceMode::modeDownDown:
			{
				assert(!(this->empty()));
				assert(!(other.empty()));
				IndivTypePtr o1 = this->_points.back();
				const IndivType *p1 = o1.get();
				assert(p1 != nullptr);
				IndivTypePtr o2 = other._points.back();
				const IndivType *p2 = o2.get();
				assert(p2 != nullptr);
				IndivType::distance(p1->value(), p2->value(), res, pFunc);
			}
			break;
			default:
				// should not fall here!
				assert(false);
				break;
			}// mode
			return (res);
		}// compute_distance
		template <typename Z>
		Z compute_distance(const IndivSetTypePtr &o,
			const DistanceMode mode = DistanceMode::modeCenter,
			const DistanceFunc<DataType, Z> *pFunc = nullptr) const {
			const IndivSetType *p = o.get();
			assert(p != nullptr);
			return this->compute_distance(*p, mode, pFunc);
		}// compute_distance
	public:
		template <typename Z>
		void intra_inertia(Z &dist) const {
			const IndivTypePtrVector &v1 = this->_points;
			const size_t n = v1.size();
			const DataTypeArray &oAr0 = this->value();
			double ss = 0.0;
			for (size_t i = 0; i < n; ++i) {
				IndivTypePtr oInd1 = v1[i];
				const IndivType *p1 = oInd1.get();
				assert(p1 != nullptr);
				const DataTypeArray &oAr1 = p1->value();
				DataTypeArray t = oAr1 - oAr0;
				DataTypeArray tt = t * t;
				ss += tt.sum();
			}// i
			if (n > 0) {
				ss = (ss / n);
			}
			dist = (Z)ss;
		}// intra_inertia
		template <typename Z>
		void intra_variance(Z &dist) const {
			const IndivTypePtrVector &v1 = this->_points;
			const size_t n = v1.size();
			size_t count = 0;
			double ss = 0.0;
			for (size_t i = 0; i < n; ++i) {
				IndivTypePtr oInd1 = v1[i];
				const IndivType *p1 = oInd1.get();
				assert(p1 != nullptr);
				const DataTypeArray &oAr1 = p1->value();
				for (size_t j = i + 1; j < n; ++j) {
					IndivTypePtr oInd2 = v1[j];
					const IndivType *p2 = oInd2.get();
					assert(p2 != nullptr);
					const DataTypeArray &oAr2 = p2->value();
					DataTypeArray t = oAr1 - oAr2;
					DataTypeArray tt = t * t;
					ss += tt.sum();
					++count;
				}// j
			}// i
			if (count > 1) {
				ss = ss / count;
			}
			dist = (Z)ss;
		}// intra_variance
		bool same_set(const IndivSetType &other) const {
			const IndivTypePtrVector &v1 = this->_points;
			const IndivTypePtrVector &v2 = other._points;
			if (v1.size() != v2.size()) {
				return (false);
			}
			for (auto it = v1.begin(); it != v1.end(); ++it) {
				IndivTypePtr oInd1 = *it;
				const IndivType *p1 = oInd1.get();
				assert(p1 != nullptr);
				bool bFound = false;
				IndexType aIndex = p1->index();
				for (auto jt = v2.begin(); jt != v2.end(); ++jt) {
					IndivTypePtr oInd2 = *jt;
					const IndivType *p2 = oInd2.get();
					assert(p2 != nullptr);
					if (p2->index() == aIndex) {
						bFound = true;
						break;
					}
				}// jt
				if (!bFound) {
					return (false);
				}
			}// it
			return (true);
		}// same_set
		const IndivTypePtrVector & members(void) const {
			return (this->_points);
		}
		const size_t members_size(void) const {
			return (this->_points.size());
		}
		const IndivType *member_at(const size_t i) const {
			assert(i < this->_points.size());
			const IndivTypePtr &c = this->_points[i];
			const IndivType *p = c.get();
			assert(p != nullptr);
			return p;
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
				IndivTypePtr o = *it;
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
				IndivTypePtr o = *it;
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
		bool add(const IndexType aIndex, const DataTypeVector &v, bool bUpdate = false) {
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
		bool add(const IndexType aIndex, const StringType &sid, const DataTypeVector &v, bool bUpdate = false) {
			if (this->contains(aIndex)) {
				return (false);
			}
			IndivTypePtr oo = std::make_shared<IndivType>(aIndex, sid, v);
			return this->add(oo, bUpdate);
		}// add
	public:
		virtual std::wostream & write_to(std::wostream &os) const {
			double var = 0;
			double tx = 0;
			this->intra_inertia(tx);
			this->intra_variance(var);
			os << L"{" << std::endl;
			os << L"\tindex: " << this->index() << std::endl;
			os << L"\tid: " << std::endl;
			os << L"\tintra-inertia: " << tx << std::endl;
			os << L"\ttrace: " << this->trace() << std::endl;
			os << L"\tintra-variance: " << var << std::endl;
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
//////////////////////////////////////
template <typename T, typename U, class S>
std::wostream & operator<<(std::wostream &os, const info::IndivSet<T, U, S> &d) {
	return d.write_to(os);
}
/////////////////////////////////
#endif // !__INDIVSET_H__
