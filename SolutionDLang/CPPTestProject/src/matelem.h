#pragma once
#ifndef __MATEMEM_H__
#define  __MATEMEM_H__
///////////////////////////////
#include <cassert>
#include <vector>
#include <deque>
#include <set>
#include <map>
////////////////////////////////////
#include "indivs.h"
////////////////////////////////////
namespace info {
	////////////////////////////////////////////
	enum class MatElemOrderType { orderInvalid, orderFirstFirst, orderFirstLast, orderLastFirst, orderLastLast };
	////////////////////////////////////////////
	template <typename Z>
	class ElemCrit {
	private:
		size_t _first;
		size_t _second;
		MatElemOrderType _order;
		Z _dist;
	public:
		ElemCrit() :first(0), _second(0), _order(MatElemOrderType::orderInvalid), _dist(0) {}
		ElemCrit(const size_t i1, const size_t i2, const MatElemOrderType o, const Z d) :
			_first(i1), _second(i2), _order(o), _dist(d) {
			if (this->second < this->first) {
				this->_first = i2;
				this->_second = i1;
			}
			assert(this->is_valid());
		}
		ElemCrit(const ElemCrit<Z> &other) :_first(other._first), _second(other._second),
			_order(other._order), _dist(other._dist) {}
		ElemCrit<Z> & operator=(const ElemCrit<Z> &other) {
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
			return ((this->_order != MatElemOrderType::orderInvalid) &&
				(this->_dist >= 0) && (this->_first <= this->_second));
		}
		size_t first(void) const {
			return (this->_first);
		}
		size_t second(void) const {
			return (this->_second);
		}
		MatElemOrderType order(void) const {
			return (this->_order);
		}
		Z criteria(void) const {
			return (this->_dist);
		}
	public:
		bool operator<(const ElemCrit<Z> &other) const {
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
		bool operator==(const ElemCrit<Z> &other) const {
			assert(this->is_valid());
			assert(other.is_valid());
			return ((this->_first == other._first) && (this->_second == other._second));
		}
	};// class ElemCrit<Z>
	///////////////////////////////////////////
	template <typename Z> class ElemCrits {
	public:
		typedef ElemCrit<Z> ElemCritType;
		typedef std::vector<ElemCritType> ElemCritTypeVector;
		typedef ElemCrits<Z> ElemCritsType;
	private:
		ElemCritTypeVector  _data;
	public:
		ElemCrits() {}
		ElemCrits(const ElemCritsType &other) :_data(other._data) {}
		ElemCritsType & operator=(const ElemCritsType &other) {
			if (this != &other) {
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
			this->_data.clear();
		}// reset
		size_t size(void) const {
			return (this->_data.size());
		}
		const ElemCritType &at(const size_t i) const {
			assert(i < this->_data.size());
			return (this->_data[i]);
		}// at
		Z criteria(void) const {
			const ElemCritTypeVector &vv = this->_data;
			Z dMin = 0;
			for (auto it = vv.begin(); it != vv.end(); ++it) {
				const Z d = (*it).citeria();
				if (it == vv.begin()) {
					dMin = d;
				}
				else if (d < dMin) {
					dMin = d;
				}
			}// it
			return (dMin);
		}// criteria
		bool add(const ElemCritType &c) {
			if (!c.is_valid()) {
				return (false);
			}
			ElemCritTypeVector &vv = this->_data;
			if (vv.empty()) {
				ElemCritType cc(c);
				vv.push_back(cc);
				return (true);
			}
			const Z dCur = c.criteria();
			const size_t iCur = c.first();
			const size_t jCur = c.second();
			Z dMin;
			bool bFirst = true;
			for (auto it = vv.begin(); it != vv.end(); ++it) {
				const ElemCritType &x = *it;
				const Z dd = x.criteria();
				if (dCur > dd) {
					return (false);
				}
				const size_t i1 = x.first();
				const size_t i2 = x.second();
				if ((i1 == iCur) || (i2 == iCur) || (i1 == jCur) || (i2 == jCur)) {
					return (false);
				}
				if (bFirst) {
					bFirst = false;
					dMin = dd;
				}
				else if (dd < dMin) {
					dMin = dd;
				}
			}// it
			if (dCur < dMin) {
				vv.clear();
			}
			ElemCritType cc(c);
			vv.push_back(cc);
			return (true);
		}// add
	};// class ElemCrits<Z>
	////////////////////////////////////////
	template <typename U = int> class MatElem {
	public:
		typedef U IndexType;
		typedef std::deque<IndexType> IndexTypeDeque;
		typedef MatElem<IndexType> MatElemType;
		typedef std::shared_ptr<MatElemType> MatElemTypePtr;
		typedef std::vector<MatElemTypePtr> MatElemTypePtrVector;
		//
		typedef std::map<size_t, MatElemOrderType> MatOrderTypeMap;
		typedef std::map<size_t, MatOrderTypeMap> SizeTMapType;
	private:
		IndexTypeDeque _data;
	public:
		MatElem() {}
		MatElem(const IndexType aIndex) {
			this->_data.push_back(aIndex);
		}
		MatElem(const MatElemType &other) :_data(other._data) {}
		MatElemType & operator=(const MatElemType &other) {
			if (this != &other) {
				this->_data = other._data;
			}
			return *this;
		}
		virtual ~MatElem() {}
	public:
		bool is_valid(void) const {
			return (!this->_data.empty());
		}
		size_t size(void) const {
			return (this->_data.size());
		}
		const IndexTypeDeque & data(void) const {
			return (this->_data);
		}
		IndexType front(void) const {
			assert(this->is_valid());
			return (this->_data.front());
		}
		IndexType back(void) const {
			assert(this->is_valid());
			return (this->_data.back());
		}
		void add(const MatElemType &other, const MatElemOrderType order) {
			assert(this->is_valid());
			assert(other.is_valid());
			IndexTypeDeque &vv = this->_data;
			switch (order) {
			case MatElemOrderType::orderFirstFirst:
			{
				for (auto it = xx.begin(); it != xx.end(); ++it) {
					vv.push_front(*it);
				}// it
			}
			break;
			case MatElemOrderType::orderFirstLast:
			{
				const IndexTypeDeque &xx = other._data;
				const size_t n = xx.size();
				for (size_t i = (size_t)(n - 1); i >= 0; --i) {
					vv.push_front(xx[i]) :
				}// i
			}
			break;
			case MatElemOrderType::orderLastFirst:
			{
				for (auto it = xx.begin(); it != xx.end(); ++it) {
					vv.push_back(*it);
				}// it
			}
			break;
			case MatElemOrderType::orderLastLast:
			{
				const IndexTypeDeque &xx = other._data;
				const size_t n = xx.size();
				for (size_t i = (size_t)(n - 1); i >= 0; --i) {
					vv.push_back(xx[i]) :
				}// i
			}
			break;
			default:
				break;
			}// order
		}// add
	public:
		template <typename T, typename UX, typename Z, class S>
		static bool st_firststep(const Indivs<T, UX, Z, S> &oInds,
			MatElemTypePtrVector &oVec, MatElemType &oCur) {
			size_t n = oInds.indivs_count();
			oVec.resize(n);
			for (size_t i = 0; i < n; ++i) {
				oVec[i] = std::make_shared<MatElemType>((IndexType)i);
			}// i
			bool bDone = false;
			SizeTMapType oMap;
			while (!bDone) {
				st_find_best_step(oVec, oInds, oMap);
				if (oMap.size() < 2) {
					bDone = true;
					break;
				}
				MatElemTypePtrVector otemp;
				std::set<size_t> oSet;
				for (auto it = oMap.begin(); it != oMap; end(); ++it) {
					size_t i = (*it).first;
					oSet.insert(i);
					auto m = (*it).second;
					MatElemTypePtr c = oVec[i];
					MatElemType *p = c.get();
					assert(p != nullptr);
					for (auto jt = m.begin(); jt != m.end(); ++jt) {
						size_t j = (*jt).first;
						oSet.insert(j);
						MatElemOrderType order = (*jt).second;
						MatElemType *px = (oVec[j]).get();
						assert(px != nullptr);
						p->add(*px, order);
						otemp.push_back(c);
					}// jt
				}// it
				const size_t nn = oVec.size();
				for (size_t i = 0; i < nn; ++i) {
					if (oSet.find(i) != oSet.end()) {
						otemp.push_back(oVec[i]);
					}
				}// i
				oVec = otemp;
			}// not done
			if (oMap.empty()) {
				return (false);
			}
			std::set<size_t> xSet;
			auto it = oMap.begin();
			size_t ii = (*it).first;
			xSet.insert(ii);
			auto mm = (*it).second;
			MatElemTypePtr cc = oVec[ii];
			MatElemType *pp = cc.get();
			assert(pp != nullptr)
				auto jt = m.begin();
			size_t jj = (*jt).first;
			oSet.insert(jj);
			MatElemOrderType order = (*jt).second;
			MatElemType *ppx = (oVec[jj]).get();
			assert(ppx != nullptr);
			pp->add(*ppx, order);
			oCur = *pp;
			const size_t nnx = oVec.size();
			MatElemTypePtrVector xtemp;
			for (size_t i = 0; i < nn; ++i) {
				if (xSet.find(i) != xSet.end()) {
					xtemp.push_back(oVec[i]);
				}
			}// i
			oVec = xtemp;
		}// st_first_step
		template <typename T, typename UX, typename Z, class S>
		static void  st_find_best_step(const  MatElemTypePtrVector &oVec,
			const Indivs<T, UX, Z, S> &oInds,
			ElemCrits<Z> &oCrits) {
			typedef ElemCrit<Z> ElemCritType;
			oCrits.reset();
			const size_t n = oVec.size();
			for (size_t i = 0; i < n; ++i) {
				const MatElemTypePtr &oFirst = oVec[i];
				const MatElemType *p1 = oFirst.get();
				assert(p1 != nullptr);
				for (size_t j = i + 1; j < n; ++j) {
					const MatElemTypePtr &oSecond = oVec[j];
					const MatElemType *p2 = oSecond.get();
					assert(p2 != nullptr);
					ElemCritType c;
					p1->distance(*p2, oInds, c);
					oCrits.add(c);
				}// j
			}// i
		}// st_find_best_step
		template <typename T, typename UX, typename Z, class S>
		static void  st_find_best_step(const  MatElemTypePtrVector &oVec,
			const Indivs<T, UX, Z, S> &oInds,
			const MatElemType &oElem,
			ElemCrits<Z> &oCrits) {
			typedef ElemCrit<Z> ElemCritType;
			oCrits.reset();
			const size_t n = oVec.size();
			for (size_t i = 0; i < n; ++i) {
				const MatElemTypePtr &oFirst = oVec[i];
				const MatElemType *p1 = oFirst.get();
				assert(p1 != nullptr);
				ElemCritType c;
				oElem.distance(*p1, oInds, c);
				oCrits.add(c);
			}// i
		}// st_find_best_step
		template <typename T, typename UX, typename Z, class S>
		Z distance(const MatElemType &other, const Indivs<T, UX, Z, S> &oInds, ElemCrit<Z>t &oCrit) const {
			typedef ElemCrit<Z> ElemCritType;
			//
			assert(oInds.is_valid());
			assert(this->is_valid());
			assert(other.is_valid());
			//
			const size_t i1 = this->front();
			const size_t j1 = other.front();
			assert(i1 != j1);
			if (this->size() == 1) {
				if (other.size() == 1) {
					MatElemOrderType order = MatElemOrderType::orderFirstFirst;
					const Z d = oInds.distance_index(i1, j1);
					oCrit = ElemCritType(i1, j1, order, d);
					return (d);
				}
				else {
					const size_t j2 = other.back();
					assert(j2 != i1);
					assert(j2 < oInds.indivs_count());
					const Z d1 = oInds.distance_index(i1, j1);
					const Z d2 = oInds.distance_index(i1, j2);
					if (d1 < d2) {
						MatElemOrderType order = MatElemOrderType::orderFirstFirst;
						oCrit = ElemCritType(i1, j1, order, d1);
						return (d1);
					}
					else {
						MatElemOrderType order = MatElemOrderType::orderFirstLast;
						oCrit = ElemCritType(i1, j2, order, d2);
						return (d2);
					}
				}
			}
			else {
				const size_t i2 = this->back();
				assert(i1 != i2);
				assert(i1 < oInds.indivs_count());
				assert(i2 < oInds.indivs_count());
				if (other.size() == 1) {
					const Z d1 = oInds.distance_index(i1, j1);
					const Z d2 = oInds.distance_index(i2, j1);
					if (d1 < d2) {
						MatElemOrderType order = MatElemOrderType::orderFirstFirst;
						oCrit = ElemCritType(i1, j1, order, d1);
						return (d1);
					}
					else {
						MatElemOrderType order = MatElemOrderType::orderLastFirst;
						oCrit = ElemCritType(i2, j1, order, d1);
						return (d2);
					}
				}
				else {
					const size_t j2 = other.back();
					assert(j1 != j2);
					assert(i1 != j2);
					assert(i2 != j2);
					size iRet = i1, jRet = j1;
					order = MatElemOrderType::orderFirstFirst;
					const Z d1 = oInds.distance_index(i1, j1);
					Z dRet = d1;
					const Z d2 = oInds.distance_index(i1, j2);
					if (d2 < d1) {
						dRet = d2;
						iRet = i1;
						jRet = j2;
						order = MatElemOrderType::orderFirstLast;
					}
					const Z d3 = oInds.distance_index(i2, j1);
					if (d3 < dRet) {
						dRet = d3;
						iRet = i2;
						jRet = j1;
						order = MatElemOrderType::orderLastFirst;
					}
					const Z d4 = oInds.distance(i2, j2);
					if (d4 < dRet) {
						dRet = d4;
						iRet = i2;
						jRet = j2;
						order = MatElemOrderType::orderLastLast;
					}
					oCrit = ElemCritType(iRet, jRet, order, dRet);
					return dRet;
				}
			}
		}// distance
	}; // class MatElem
	/////////////////////////////////////////
}// namespace info
///////////////////////////////////
#endif // !__MATEMEM_H__
///////////////////////////////
//eof: matelem.h
