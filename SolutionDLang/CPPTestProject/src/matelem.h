#pragma once
#ifndef __MATEMEM_H__
#define  __MATEMEM_H__
///////////////////////////////
#include "gendefs.h"
////////////////////////////////////
namespace info {
	////////////////////////////////////////////
	enum class MatElemOrderType { orderInvalid, orderFirstFirst, orderFirstLast, orderLastFirst, orderLastLast };
	////////////////////////////////////////////
	template <typename Z>
	class ElemCrit {
	public:
		typedef Z DistanceType;
		typedef ElemCrit<DistanceType> ElemCritType;
	private:
		size_t _first;
		size_t _second;
		MatElemOrderType _order;
		DistanceType _dist;
	public:
		ElemCrit() :_first(0), _second(0), _order(MatElemOrderType::orderInvalid), _dist(0) {}
		ElemCrit(const size_t i1, const size_t i2, const MatElemOrderType o, const DistanceType d) :
			_first(i1), _second(i2), _order(o), _dist(d) {
			if (this->_second < this->_first) {
				this->_first = i2;
				this->_second = i1;
			}
			assert(this->is_valid());
		}
		ElemCrit(const size_t i1, const size_t i2) :
			_first(i1), _second(i2), _order(MatElemOrderType::orderInvalid), _dist(0) {
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
		void order(const MatElemOrderType o) {
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
	template <typename U = size_t> class MatElem {
	public:
		typedef U IndexType;
		//
		typedef std::deque<IndexType> IndexTypeDeque;
		typedef MatElem<IndexType> MatElemType;
		typedef std::shared_ptr<MatElemType> MatElemTypePtr;
		typedef std::vector<MatElemTypePtr> MatElemTypePtrVector;
		//
		typedef std::map<IndexType, MatElemOrderType> MatOrderTypeMap;
		typedef std::map<IndexType, MatOrderTypeMap> SizeTMapType;
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
			assert(order != MatElemOrderType::orderInvalid);
			if ((this->_data.empty()) || (other._data.empty())) {
				return;
			}
			const IndexTypeDeque &xx = other._data;
			IndexTypeDeque &vv = this->_data;
			switch (order) {
			case MatElemOrderType::orderFirstFirst:
			{
				auto iend = xx.end();
				for (auto it = xx.begin(); it != iend; ++it) {
					vv.push_front(*it);
				}// it
			}
			break;
			case MatElemOrderType::orderFirstLast:
			{
				const IndexTypeDeque &xx = other._data;
				const int n = (int)(xx.size()) - 1;
				for (int i = n; i >= 0; --i) {
					vv.push_front(xx[i]);
				}// i
			}
			break;
			case MatElemOrderType::orderLastFirst:
			{
				auto iend = xx.end();
				for (auto it = xx.begin(); it != iend; ++it) {
					vv.push_back(*it);
				}// it
			}
			break;
			case MatElemOrderType::orderLastLast:
			{
				const IndexTypeDeque &xx = other._data;
				const int n = (int)(xx.size()) - 1;
				for (int i = n; i >= 0; --i) {
					vv.push_back(xx[i]);
				}// i
			}
			break;
			default:
				break;
			}// order
		}// add
	}; // class MatElem<U>
	/////////////////////////////////////////
	template <typename U = size_t, typename Z = long> class MatElemSort {
	public:
		typedef U IndexType;
		typedef Z DistanceType;
		//
		typedef std::deque<IndexType> IndexTypeDeque;
		typedef MatElem<IndexType> MatElemType;
		typedef std::shared_ptr<MatElemType> MatElemTypePtr;
		typedef std::vector<MatElemTypePtr> MatElemTypePtrVector;
		typedef std::map<IndexType, MatElemOrderType> MatOrderTypeMap;
		typedef std::map<IndexType, MatOrderTypeMap> SizeTMapType;
		typedef MatElemSort<IndexType, DistanceType> MatElemSortType;
		typedef std::vector<DistanceType> DistanceTypeVector;
		typedef ElemCrit<DistanceType> ElemCritType;
		typedef ElemCrits<DistanceType> ElemCritsType;
	private:
		size_t _n;
		const DistanceTypeVector *_pdist;
		MatElemTypePtrVector _vec;
	public:
		MatElemSort() :_n(0), _pdist(nullptr) {}
		MatElemSort(const size_t n, const DistanceTypeVector *pdist) :_n(n), _pdist(pdist) {
			assert(this->_n > 0);
			assert(this->_pdist != nullptr);
			assert(this->_pdist->size() >= (size_t)(this->_n * this->_n));
			MatElemTypePtrVector &oVec = this->_vec;
			oVec.resize(n);
			for (size_t i = 0; i < n; ++i) {
				oVec[i] = std::make_shared<MatElemType>((IndexType)i);
			}// i
		}
		MatElemSort(const MatElemSortType &other) :_n(other._n), _pdist(other._pdist), _vec(other._vec) {}
		MatElemSortType & operator=(const MatElemSortType &other) {
			if (this != &other) {
				this->_n = other._n;
				this->_pdist = other._pdist;
				this->_vec = other._vec;
			}
			return *this;
		}
		virtual ~MatElemSort() {}
	public:
		bool is_valid(void) const {
			return ((this->_n > 0) && (this->_pdist != nullptr) &&
				(this->_pdist->size() >= (size_t)(this->_n * this->_n)));
		}
	public:
		
		template <typename X>
		DistanceType criteria(const std::vector<X> &index) const {
			assert(this->is_valid());
			const size_t n = this->_n;
			assert(index.size() == n);
			double r = 0.0;
			for (size_t i = 1; i < n; ++i) {
				const size_t i1 = index[i - 1];
				assert(i1 < n);
				const size_t i2 = index[i];
				assert(i2 < n);
				r += this->indivs_distance(i1, i2);
			}// i
			return (DistanceType)(r / n);
		}// criteria
		template <typename X>
		DistanceType criteria(const std::valarray<X> &index) const {
			assert(this->is_valid());
			const size_t n = this->_n;
			assert(index.size() == n);
			double r = 0.0;
			for (size_t i = 1; i < n; ++i) {
				const size_t i1 = index[i - 1];
				assert(i1 < n);
				const size_t i2 = index[i];
				assert(i2 < n);
				r += this->indivs_distance(i1, i2);
			}// i
			return (DistanceType)(r / n);
		}// criteria
		template <typename X>
		void arrange(std::valarray<X> &index) {
			std::vector<X> temp;
			this->arrange(temp);
			const size_t n = temp.size();
			for (size_t i = 0; i < n; ++i) {
				index[i] = temp[i];
			}
		}// arrange
		template <typename X>
		void arrange(std::vector<X> &index) {
			if (!this->is_valid()) {
				index.clear();
				return;
			}
			const size_t n = this->_n;
			MatElemTypePtrVector &oVec = this->_vec;
			if (oVec.size() != n) {
				oVec.resize(n);
				for (size_t i = 0; i < n; ++i) {
					oVec[i] = std::make_shared<MatElemType>((IndexType)i);
				}// i
			}
			bool bDone = false;
			while (!bDone) {
				if (!update_candidates()) {
					bDone = true;
					break;
				}
			}// while
			assert(oVec.size() == 1);
			MatElemType *p = (oVec[0]).get();
			assert(p != nullptr);
			const IndexTypeDeque &src = p->data();
			assert(src.size() == n);
			index.resize(n);
			for (size_t i = 0; i < n; ++i) {
				index[i] = (X)src[i];
			}// i
			this->check_permutations(index);
		}// arrange
	protected:
		template <typename X>
		void check_permutations(std::vector<X> &index) const {
			const size_t n = (size_t)(index.size() / 2);
			if (n < 1) {
				return;
			}
			for (size_t i = 1; i <= n; ++i) {
				this->permute_one_step(index, i);
			}// i
		}//check_permutations
		template <typename X>
		void permute_one_step(std::vector<X> &index, const size_t iLength = 1) const {
			assert(iLength > 0);
			const size_t n = index.size();
			DistanceType bestCrit = this->criteria(index);
			std::vector<X> bestIndex(index);
			bool bDone = false;
			while (!bDone) {
				bDone = true;
				for (size_t i = 0; i < n; ++i) {
					if ((size_t)(i + iLength) > n) {
						break;
					}
					for (size_t j = i + iLength; j < n; ++j) {
						if ((size_t)(j + iLength) > n) {
							break;
						}
						std::vector<X> curIndex(index);
						if (!this->permute_indexes(curIndex, i, j, iLength)) {
							break;
						}
						const DistanceType dCur = this->criteria(curIndex);
						if (dCur < bestCrit) {
							bDone = false;
							bestCrit = dCur;
							bestIndex = curIndex;
						}// ok
					}// j
				}// i
			}// not done
			index = bestIndex;
		}//permute_one_step
		template <typename X>
		bool permute_indexes(std::vector<X> &index, const size_t iSrc, const size_t iDest, const size_t iLength = 1) const {
			const size_t n = index.size();
			assert(iSrc < n);
			assert(iDest < n);
			assert(iLength > 0);
			size_t i1 = iSrc;
			size_t i2 = iDest;
			if (i1 > i2) {
				size_t t = i1;
				i1 = i2;
				i2 = t;
			}
			else if (i1 == i2) {
				return (false);
			}
			if ((size_t)(i1 + iLength) > i2) {
				return (false);
			}
			if ((size_t)(i2 + iLength) > n) {
				return (false);
			}
			for (size_t i = 0; i < iLength; ++i) {
				X temp = index[i1 + i];
				index[i1 + i] = index[i2 + i];
				index[i2 + i] = temp;
			}// i
			return (true);
		}// permute_indexes 
		bool update_candidates(void) {
			MatElemTypePtrVector &oVec = this->_vec;
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
				const MatElemOrderType order = c.order();
				MatElemTypePtr o = oVec[i1];
				MatElemType *p = o.get();
				assert(p != nullptr);
				MatElemType *px = (oVec[i2]).get();
				assert(px != nullptr);
				p->add(*px, order);
				oSet.insert(i2);
			}// i
			MatElemTypePtrVector xtemp;
			for (size_t i = 0; i < n; ++i) {
				if (oSet.find(i) == oSet.end()) {
					xtemp.push_back(oVec[i]);
				}
			}//i
			oVec = xtemp;
			return (true);
		}//update_candidates
		void  find_best_step(ElemCritsType  &oCrits) const {
			oCrits.reset();
			const size_t n = this->_vec.size();
			for (size_t i = 0; i < n; ++i) {
				for (size_t j = i + 1; j < n; ++j) {
					ElemCritType c(i, j);
					this->distance(i, j, c);
					oCrits.add(c);
				}// j
			}// i
		}// st_find_best_step
		DistanceType distance(const size_t ipos, const size_t jpos, ElemCritType &oCrit) const {
			const MatElemTypePtrVector &oVec = this->_vec;
			const size_t n = oVec.size();
			assert(ipos < n);
			assert(jpos < n);
			assert(ipos != jpos);
			//
			const MatElemType *p1 = (oVec[ipos]).get();
			assert(p1 != nullptr);
			const MatElemType *p2 = (oVec[jpos]).get();
			assert(p2 != nullptr);
			//
			DistanceType d = 0;
			MatElemOrderType order = MatElemOrderType::orderInvalid;
			const size_t i1 = p1->front();
			const size_t j1 = p2->front();
			assert(i1 != j1);
			const DistanceType d1 = this->indivs_distance(i1, j1);
			if (p1->size() == 1) {
				if (p2->size() == 1) {
					order = MatElemOrderType::orderFirstFirst;
					oCrit.order(order);
					oCrit.criteria(d1);
					return (d1);
				}
				else {
					const size_t j2 = p2->back();
					assert(j2 != i1);
					const Z d2 = this->indivs_distance(i1, j2);
					if (d1 < d2) {
						order = MatElemOrderType::orderFirstFirst;
						oCrit.order(order);
						oCrit.criteria(d1);
						return (d1);
					}
					else {
						order = MatElemOrderType::orderFirstLast;
						oCrit.order(order);
						oCrit.criteria(d2);
						return (d2);
					}
				}
			}
			else {
				const size_t i2 = p1->back();
				assert(i1 != i2);
				if (p2->size() == 1) {
					const Z d2 = this->indivs_distance(i2, j1);
					if (d1 < d2) {
						order = MatElemOrderType::orderFirstFirst;
						oCrit.order(order);
						oCrit.criteria(d1);
						return (d1);
					}
					else {
						order = MatElemOrderType::orderLastFirst;
						oCrit.order(order);
						oCrit.criteria(d2);
						return (d2);
					}
				}
				else {
					const size_t j2 = p2->back();
					assert(j1 != j2);
					assert(i1 != j2);
					assert(i2 != j2);
					order = MatElemOrderType::orderFirstFirst;
					d = d1;
					const Z d2 = this->indivs_distance(i1, j2);
					if (d2 < d1) {
						d = d2;
						order = MatElemOrderType::orderFirstLast;
					}
					const Z d3 = this->indivs_distance(i2, j1);
					if (d3 < d) {
						d = d3;
						order = MatElemOrderType::orderLastFirst;
					}
					const Z d4 = this->indivs_distance(i2, j2);
					if (d4 < d) {
						d = d4;
						order = MatElemOrderType::orderLastLast;
					}
					oCrit.order(order);
					oCrit.criteria(d);
					return d;
				}
			}
		}// distance
		DistanceType indivs_distance(const size_t i1, const size_t i2) const {
			assert(this->is_valid());
			assert(i1 < this->_n);
			assert(i2 < this->_n);
			const DistanceTypeVector &d = *(this->_pdist);
			return (d[i1 * this->_n + i2]);
		}// distance
	 /////////
	}; // class MatElemSort<U,Z>
	/////////////////////////////////////////////
}// namespace info
///////////////////////////////////
#endif // !__MATEMEM_H__
///////////////////////////////
//eof: matelem.h
