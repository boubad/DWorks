#pragma once
#ifndef __ARRANGEDESC_H__
#define __ARRANGEDESC_H__
///////////////////////////////////////////
#include <cassert>
#include <deque>
#include <vector>
#include <set>
#include <algorithm>
/////////////////////////////////////////////
namespace info {
	////////////////////////////////////////
	template <typename Z = long, typename U = int> class ArrangeDesc {
	public:
		typedef Z DistanceType;
		typedef U IndexType;
		//
		typedef std::vector<DistanceType> DistanceTypeVector;
		typedef std::vector<IndexType> IndexTypeVector;
		typedef std::vector<IndexTypeVector> IndexTypeArray;
		typedef std::deque<IndexType> IndexTypeList;
		typedef ArrangeDesc<DistanceType, IndexType> ArrangeDescType;
		typedef std::set<IndexType> IndexTypeSet;
	private:
		size_t _n;
		const DistanceTypeVector &_dist;
		IndexTypeVector _index;
	public:
		ArrangeDesc() :_n(0) {}
		ArrangeDesc(const size_t n, const DistanceTypeVector &v) :_n(n), _dist(v), _index(n) {
			assert(n > 0);
			assert(v.size() >= (size_t)(n * n));
			IndexTypeVector &vv = this->_index;
			assert(vv.size() == n);
			for (size_t i = 0; i < n; ++i) {
				vv[i] = (IndexType)i;
			}// i
		}
		ArrangeDesc(const ArrangeDescType &other) :_n(other._n), _dist(other._dist), _index(other._index) {}
		ArrangeDescType & operator=(const ArrangeDescType &other) {
			if (this != &other) {
				this->_n = other._n;
				this->_dist = other._n;
				this->_index = other._index;
			}
			return (*this);
		}
		virtual ~ArrangeDesc() {}
	protected:
		void index(const IndexTypeVector &v) {
			const size_t n = v.size();
			assert(this->_n == n);
			this->_index = v;
		}
	public:
		bool is_valid(void) const {
			return ((this->_n > 0) && (this->_index.size() == this->_n) &&
				(this->_dist.size() >= (size_t)(this->_n * this->_n)));
		}
		size_t size(void) const {
			return (this->_n);
		}
		const IndexTypeVector & index(void) const {
			return (this->_index);
		}
		void reset(void) {
			IndexTypeVector &v = this->_index;
			const size_t n = v.size();
			for (size_t i = 0; i < n; ++i) {
				v[i] = (IndexType)i;
			}// i
		}
		void shuffle(void) {
			assert(this->is_valid());
			const IndexTypeVector &ind = this->_index;
			std::random_shuffle(ind.begin(), ind.end());
		}// shuffle
		DistanceType distance(const size_t index1, const size_t index2, const bool bIndirect = true) const {
			assert(this->is_valid());
			const IndexTypeVector &ind = this->_index;
			const size_t n = ind.size();
			size_t i1, i2;
			assert(index1 < n);
			assert(index2 < n);
			if (bIndirect) {
				i1 = ind[index1];
				i2 = ind[index2];
			}
			else {
				i1 = index1;
				i2 = index2;
			}
			assert(i1 < n);
			assert(i2 < n);
			if (i1 > i2) {
				size_t temp = i1;
				i1 = i2;
				i2 = temp;
			}
			const DistanceTypeVector &dd = this->_dist;
			return (dd[i1 * n + i2]);
		}// distance
		DistanceType criteria(const IndexTypeVector &ind) const {
			assert(this->is_valid());
			const size_t n = this->size();
			assert(ind.size() == n);
			double dd = 0.0;
			const size_t nm1 = (size_t)(n - 1);
			for (size_t i = 0; i < nm1; ++i) {
				dd += this->distance(ind[i], ind[i + 1], false);
			}// i
			return (DistanceType)(dd / n);
		}// criteria
		DistanceType criteria(const bool bIndirect = true) const {
			assert(this->is_valid());
			const size_t n = this->size();
			assert(n > 0);
			double dd = 0.0;
			const size_t nm1 = (size_t)(n - 1);
			for (size_t i = 0; i < nm1; ++i) {
				dd += this->distance(i, i + 1, bIndirect);
			}// i
			return (DistanceType)(dd / n);
		}// criteria
		bool next_permutation(const DistanceType oldCriteria, IndexType &ind, DistanceType &crit) const {
			if (!std::next_permutation(ind.begin(), ind.end())) {
				crit = oldCriteria;
				return (false);
			}
			crit = this->criteria(ind);
			return (true);
		}// next_permutation
		DistanceType best_permutations(IndexTypeArray &oAr,
			const size_t nLimit = 0) const {
			assert(this->is_valid());
			const size_t n = this->size();
			IndexTypeVector ind(n);
			for (size_t i = 0; i < n; ++i) {
				ind[i] = (IndexType)i;
			}// i
			DistanceType oldCriteria = this->criteria(ind);
			oAr.clear();
			IndexTypeVector vx(ind);
			oAr.push_back(vx);
			bool bDone = false;
			size_t limit = 0;
			while (!bDone) {
				if (!std::next_permutation(ind.begin(), ind.end())) {
					break;
				}
				if (nLimit != 0) {
					if (limit >= nLimit) {
						bDone = true;
						break;
					}
				}
				++limit;
				if (limit >= 100000) {
					bDone = true;
					break;
				}
				DistanceType c = this->criteria(ind);
				if (c < oldCriteria) {
					oldCriteria = c;
					oAr.clear();
					IndexTypeVector vy(ind);
					oAr.push_back(vy);
				}
				else if (c == oldCriteria) {
					IndexTypeVector vy(ind);
					oAr.push_back(vy);
				}
			}// bDone
			return (oldCriteria);
		}//first_best_permutation
		void get_start_index(IndexTypeVector &ind) const {
			assert(this->is_valid());
			const size_t n = this->size();
			IndexTypeSet oSet;
			IndexTypeList oList;
			for (size_t i = 0; i < n; ++i) {
				IndexType aIndex = (IndexType)i;
				oSet.insert(aIndex);
			}// i
			bool bDone = false;
			while (!bDone) {
				if (!this->find_best_assoc(oSet, oList)) {
					bDone = true;
					break;
				}
			}// bDone
			ind.resize(n);
			for (size_t i = 0; i < n; ++i) {
				ind[i] = oList[i];
			}// i
		}// getStartIndex
		void arrange(IndexTypeVector &ind,DistanceType &crit) const {
			DistanceType old = this->criteria();
			this->get_start_index(ind);
			crit = this->criteria(ind);
			bool bDone = false;
			while (!bDone) {
				if (!this->one_permutation(ind, crit)) {
					bDone = true;
					break;
				}
			}// bDone
		}// arrange
	protected:
		bool one_permutation(IndexTypeVector &ind, DistanceType &dist) const {
			DistanceType dMin = this->criteria(ind);
			IndexType i1 = 0, i2 = 0;
			bool bFirst = true;
			const size_t n = ind.size();
			for (size_t i = 0; i < n; ++i) {
				for (size_t j = i + 1; j < n; ++j) {
					IndexTypeVector v(ind);
					const IndexType k = v[i];
					v[i] = v[j];
					v[j] = k;
					DistanceType r = this->criteria(v);
					if (r < dMin) {
						dMin = r;
						i1 = i;
						i2 = j;
					}
				}// j
			}// i
			if (i1 == i2) {
				return (false);
			}
			const IndexType kk = ind[i1];
			ind[i1] = ind[i2];
			ind[i2] = kk;
			dist = dMin;
			return (true);
		}// one_permutation
		bool find_best_assoc(IndexTypeSet &candidates, IndexTypeList &oList) const {
			if (candidates.empty()) {
				return (false);
			}
			if (oList.empty()) {
				DistanceType dMin = 0;
				IndexType i1 = 0, i2 = 0;
				bool bFirst = true;
				for (auto it = candidates.begin(); it != candidates.end(); it++) {
					IndexType i = *it;
					auto kt = it;
					for (auto jt = ++kt; jt != candidates.end(); ++jt) {
						IndexType j = *jt;
						DistanceType d = this->distance(i, j);
						if (bFirst) {
							bFirst = false;
							dMin = d;
							i1 = i;
							i2 = j;
						}
						else if (d < dMin) {
							dMin = d;
							i1 = i;
							i2 = j;
						}
					}// jt
				}// it
				if (i1 == i2) {
					return (false);
				}
				oList.push_back(i1);
				oList.push_back(i2);
				candidates.erase(candidates.find(i1));
				candidates.erase(candidates.find(i2));
				return (true);
			}
			else {
				const size_t n = oList.size();
				assert(n > 1);
				const IndexType iSup = oList[0];
				const IndexType iDown = oList[n - 1];
				if (candidates.size() == 1) {
					const IndexType i = *(candidates.begin());
					DistanceType d1 = this->distance(iSup, i);
					DistanceType d2 = this->distance(iDown, i);
					if (d1 < d2) {
						oList.push_front(i);
					}
					else {
						oList.push_back(i);
					}
					candidates.clear();
					return true;
				}
				DistanceType dMinSup = 0;
				IndexType i1 = 0;
				DistanceType dMinDown = 0;
				IndexType i2 = 0;
				bool bFirst = true;
				for (auto it = candidates.begin(); it != candidates.end(); ++it) {
					const IndexType  k = *it;
					const DistanceType d1 = this->distance(iSup, k);
					const DistanceType d2 = this->distance(iDown, k);
					if (bFirst) {
						bFirst = false;
						dMinSup = d1;
						i1 = k;
						dMinDown = d2;
						i2 = k;
					}
					else {
						if (d1 < dMinSup) {
							dMinSup = d1;
							i1 = k;
						}
						if (d2 < dMinSup) {
							dMinSup = d2;
							i2 = k;
						}
					}
				}// it
				if (dMinSup < dMinDown) {
					oList.push_front(i1);
					candidates.erase(candidates.find(i1));
				}
				else {
					oList.push_back(i2);
					candidates.erase(candidates.find(i2));
				}
			}// not empty
			return (true);
		}// find_best_assos
	}; // class ArrangeDesc<Z,U>
	/////////////////////////////////////////
}// namespace info
//////////////////////////////////////////
#endif //
