#pragma once
#ifndef __ELEMCRIT_H__
#define __ELEMCRIT_H__
///////////////////////////
#include "indivs.h"
#include "indivset.h"
///////////////////////////////////
namespace info {
	//////////////////////////////////
	template <typename Z>
	class ElemCrit {
	public:
		typedef Z DistanceType;
		typedef ElemCrit<DistanceType> ElemCritType;
	private:
		size_t _first;
		size_t _second;
		DistanceMode _order;
		DistanceType _dist;
	public:
		ElemCrit() :_first(0), _second(0), _order(DistanceMode::modeInvalid), _dist(0) {}
		ElemCrit(const size_t i1, const size_t i2, const DistanceMode o, const DistanceType d) :
			_first(i1), _second(i2), _order(o), _dist(d) {
			if (this->_second < this->_first) {
				this->_first = i2;
				this->_second = i1;
			}
			assert(this->is_valid());
		}
		ElemCrit(const size_t i1, const size_t i2) :
			_first(i1), _second(i2), _order(DistanceMode::modeInvalid), _dist(0) {
			if (this->_second < this->_first) {
				this->_first = i2;
				this->_second = i1;
			}
		}
		ElemCrit(const ElemCritType &other) :_first(other._first), _second(other._second),
			_order(other._order), _dist(other._dist) {}
		ElemCritType & operator=(const ElemCritType &other) {
			if (this != &other) {
				this->_first = other._first;
				this->_second = other._second;
				this->_order = other._order;
				this->_dist = other._dist;
				assert(this->is_valid());
			}
			return (*this);
		}
		~ElemCrit() {}
	public:
		bool is_valid(void) const {
			return ((this->_order != DistanceMode::modeInvalid) &&
				(this->_dist >= 0) && (this->_first <= this->_second));
		}
		size_t first(void) const {
			return (this->_first);
		}
		size_t second(void) const {
			return (this->_second);
		}
		DistanceMode order(void) const {
			return (this->_order);
		}
		void order(const DistanceMode o) {
			this->_order = o;
		}
		DistanceType criteria(void) const {
			return (this->_dist);
		}
		void criteria(const DistanceType d) {
			this->_dist = d;
		}
	public:
		bool operator<(const ElemCritType &other) const {
			assert(this->is_valid());
			assert(other.is_valid());
			if (this->_first < other._first) {
				return (true);
			}
			else if (this->_first > other._first) {
				return (false);
			}
			return (this->_second < other._second);
		}
		bool operator==(const ElemCritType &other) const {
			assert(this->is_valid());
			assert(other.is_valid());
			return ((this->_first == other._first) && (this->_second == other._second));
		}
	};// class ElemCrit<Z>
	  ///////////////////////////////////////////
	template <typename Z> class ElemCrits {
	public:
		typedef Z DistanceType;
		//
		typedef ElemCrit<DistanceType> ElemCritType;
		typedef std::vector<ElemCritType> ElemCritTypeVector;
		typedef ElemCrits<DistanceType> ElemCritsType;
	private:
		DistanceType _crit;
		ElemCritTypeVector  _data;
	public:
		ElemCrits() :_crit(0) {}
		ElemCrits(const ElemCritsType &other) :_crit(other._crit), _data(other._data) {}
		ElemCritsType & operator=(const ElemCritsType &other) {
			if (this != &other) {
				this->_crit = other._crit;
				this->_data = other._data;
			}
			return (*this);
		}
		~ElemCrits() {}
	public:
		bool operator<(const ElemCritsType &other) const {
			return (this->criteria() < other.criteria());
		}
		bool operator==(const ElemCritsType &other) const {
			return (this->criteria() == other.criteria());
		}
	public:
		void reset(void) {
			this->_crit = 0;
			this->_data.clear();
		}// reset
		size_t size(void) const {
			return (this->_data.size());
		}
		const ElemCritType &at(const size_t i) const {
			assert(i < this->_data.size());
			return (this->_data[i]);
		}// at
		DistanceType criteria(void) const {
			return (this->_crit);
		}// criteria
		bool add(const ElemCritType &c) {
			if (!c.is_valid()) {
				return (false);
			}
			ElemCritTypeVector &vv = this->_data;
			if (vv.empty()) {
				this->_crit = c.criteria();
				ElemCritType cc(c);
				vv.push_back(cc);
				return (true);
			}
			const DistanceType curCrit = c.criteria();
			const DistanceType bestCrit = this->criteria();
			if (curCrit > bestCrit) {
				return (false);
			}
			else if (curCrit < bestCrit) {
				this->_crit = curCrit;
				this->_data.clear();
				ElemCritType cc(c);
				this->_data.push_back(cc);
				return (true);
			}
			const size_t i1 = c.first();
			const size_t i2 = c.second();
			auto iend = vv.end();
			for (auto it = vv.begin(); it != iend; ++it) {
				const ElemCritType &x = *it;
				const size_t x1 = x.first();
				const size_t x2 = x.second();
				if ((i1 == x1) || (i1 == x2) || (i2 == x1) || (i2 == x2)) {
					return (false);
				}
			}// it
			ElemCritType cc(c);
			this->_data.push_back(cc);
			return (true);
		}// add
	};// class ElemCrits<Z>
	////////////////////////////////////////
	template <typename T = int, typename U = int, typename Z = long, class S = std::wstring>
	class MatArrange {
	public:
		typedef T DataType;
		typedef U IndexType;
		typedef Z DistanceType;
		typedef S StringType;
		//
		typedef ElemCrit<DistanceType> ElemCritType;
		typedef ElemCrits<DistanceType> ElemCritsType;
		typedef DistanceFunc<DataType, DistanceType> DistanceFuncType;
		typedef Indiv<DataType, IndexType, StringType> IndivType;
		typedef std::shared_ptr<IndivType> IndivTypePtr;
		typedef IndivSet<DataType, IndexType, StringType> IndivSetType;
		typedef std::shared_ptr<IndivSetType> IndivSetTypePtr;
		typedef std::vector<IndivSetTypePtr> IndivSetTypePtrVector;
		typedef Indivs<DataType, IndexType, DistanceType, StringType> IndivsType;
		//
		typedef MatArrange<DataType, IndexType, DistanceType, StringType> MatArrangeType;
	private:
		DistanceFuncType *_pfunc;
		IndivSetTypePtrVector _vec;
	public:
		MatArrange() :_pfunc(nullptr) {}
		MatArrange(const IndivsType *pIndivs) :_pfunc(nullptr) {
			assert(pIndivs != nullptr);
			assert(pIndivs->is_valid());
			this->_pfunc = pIndivs->distance_func();
			const size_t n = pIndivs->indivs_count();
			assert(n > 2);
			IndivSetTypePtrVector &vv = this->_vec;
			vv.resize(n);
			for (size_t i = 0; i < n; ++i) {
				IndivTypePtr o = pIndivs->indiv_at(i);
				vv[i] = std::make_shared<IndivSetType>(o);
			}// i
		}// MatArrange
		MatArrange(const MatArrangeType &other) :_pfunc(other._pfunc), _vec(other._vec) {}
		MatArrangeType & operator=(const MatArrangeType &other) {
			if (this != &other) {
				this->_pfunc = other._pfunc;
				this->_vec = other._vec;
			}
			return (*this);
		}
		virtual ~MatArrange() {}
	public:
		bool is_valid(void) const {
			return (!(this->_vec.empty()));
		}
		size_t size(void) const {
			return (this->_vec.size());
		}
		const IndivSetType *cluster_at(const size_t i) const {
			assert(i < this->_vec.size());
			const IndivSetTypePtr &c = this->_vec[i];
			const IndivSetType *p = c.get();
			assert(p != nullptr);
			return (p);
		}
		void arrange(const size_t nClasses = 1) {
			assert(nClasses > 0);
			assert(nClasses <= this->_vec.size());
			while (this->_vec.size() > nClasses) {
				if (!this->update_candidates()) {
					break;
				}
			}// while
		}// arrange
	protected:
		bool update_candidates(void) {
			IndivSetTypePtrVector &oVec = this->_vec;
			const size_t n = oVec.size();
			if (n < 2) {
				return (false);
			}
			ElemCritsType oCrits;
			this->find_best_step(oCrits);
			const size_t nn = oCrits.size();
			if (nn < 1) {
				return (false);
			}
			std::set<size_t> oSet;
			for (size_t i = 0; i < nn; ++i) {
				const ElemCritType &c = oCrits.at(i);
				const size_t i1 = c.first();
				const size_t i2 = c.second();
				const DistanceMode order = c.order();
				IndivSetTypePtr o = oVec[i1];
				IndivSetType *p = o.get();
				assert(p != nullptr);
				IndivSetTypePtr ox = oVec[i2];
				const IndivSetType *px = ox.get();
				assert(px != nullptr);
				p->merge(*px, order);
				oSet.insert(i2);
			}// i
			IndivSetTypePtrVector xtemp;
			for (size_t i = 0; i < n; ++i) {
				if (oSet.find(i) == oSet.end()) {
					xtemp.push_back(oVec[i]);
				}
			}//i
			this->_vec = xtemp;
			return (true);
		}//update_candidates
		void  find_best_step(ElemCritsType  &oCrits) const {
			oCrits.reset();
			const size_t n = this->_vec.size();
			for (size_t i = 0; i < n; ++i) {
				for (size_t j = (size_t)(i + 1); j < n; ++j) {
					ElemCritType c;
					this->distance(i, j, c);
					oCrits.add(c);
				}// j
			}// i
		}// st_find_best_step
		DistanceType distance(size_t ipos, size_t jpos, ElemCritType &oCrit) const {
			const IndivSetTypePtrVector &oVec = this->_vec;
			const size_t n = oVec.size();
			assert(ipos < n);
			assert(jpos < n);
			assert(ipos != jpos);
			//
			const IndivSetType *p1 = (oVec[ipos]).get();
			assert(p1 != nullptr);
			const IndivSetType *p2 = (oVec[jpos]).get();
			assert(p2 != nullptr);
			//
			DistanceType dummy;
			DistanceMode mode;
			DistanceType dist = p1->find_min_distance(*p2, mode, dummy, const_cast<const DistanceFuncType *>(this->_pfunc));
			oCrit = ElemCritType(ipos, jpos, mode, dist);
			return (dist);
		}// distance
	};// class MatArrange<T,U,Z>
	///////////////////////////////////
}// namespace info
///////////////////////////////
#endif // !__ELEMCRIT_H__
/////////////////////
//eof: elemcrit.h
