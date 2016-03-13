#pragma once
#ifndef __INDIVS_H__
#define __INDIVS_H__
////////////////////////////////
#include "indivset.h"
#include "matdata.h"
#include <map>
///////////////////////////////////
namespace info {
	//////////////////////////////////////
	template <typename T = int, typename U = int, typename Z = long, class S = std::wstring>
	class Indivs {
		typedef T DataType;
		typedef U IndexType;
		typedef Z DistanceType;
		typedef S StringType;
		//
		typedef std::valarray<T> DataTypeArray;
		typedef DistanceFunc<DataType, DistanceType> DistanceFuncType;
		typedef MatData<DataType> MatDataType;
		typedef Indiv<T, U, S> IndivType;
		typedef std::shared_ptr<IndivType> IndivTypePtr;
		typedef std::vector<IndivTypePtr> IndivTypePtrVector;
		typedef IndivSet<DataType, IndexType, StringType> ClusterType;
		typedef std::shared_ptr<ClusterType> ClusterTypePtr;
		typedef std::vector<ClusterTypePtr> ClusterTypePtrVector;
		typedef std::map<IndexType, IndexType> IndexTypeMap;
		//
		typedef Indivs<DataType, IndexType, DistanceType, StringType> IndivsType;
	private:
		DataMode _mode;
		MatDataType *_pdata;
		DistanceFuncType *_pfunc;
		IndivTypePtrVector _vec;
	private:
		static const size_t NB_ITER_MAX;
		static std::shared_ptr<ManhattanDistanceFunc<DataType, DistanceType> > _st_func;
	public:
		Indivs() :_mode(DataMode::noMode), _pdata(nullptr), _pfunc(nullptr) {
			if (this->_pfunc == nullptr) {
				this->_pfunc = _st_func.get();
			}
		}
		Indivs(MatDataType *pData,
			DataMode m = DataMode::modeRow,
			DistanceFuncType *pf = nullptr) :
			_mode(m), _pdata(pData), _pfunc(pf) {
			if (this->_pfunc == nullptr) {
				this->_pfunc = _st_func.get();
			}
		}
		Indivs(const IndivsType &other) :_mode(other._mode), _pdata(other._pdata),
			_pfunc(other._pfunc), _vec(other._vec) {
		}
		IndivsType & operator=(const IndivsType &other) {
			if (this != &other) {
				this->_mode = other._mode;
				this->_pdata = other._pdata;
				this->_pfunc = other._pfunc;
				this->_vec = other._vec;
			}
			return (*this);
		}
		virtual ~Indivs() {}
	public:
		bool is_valid(void) const {
			return ((this->_mode != DataMode::noMode) && (this->_pdata != nullptr) &&
				(this->_pdata->is_valid()));
		}
		DataMode mode(void) const {
			return (this->_mode);
		}
		void mode(const DataMode m) {
			if (this->mode != m) {
				this->_mode = m;
				this->_vec.clear();
			}
		}
		MatDataType *matdata(void) const {
			return (this->_pdata);
		}
		void matdata(MatDataType *p) {
			if (p != this->_pdata) {
				this->_pdata = p;
				this->_vec.clear();
			}
		}
		DistanceFuncType *distance_func(void) const {
			return (this->_pfunc);
		}
		void distance_func(DistanceFuncType *p) {
			if (p != this->_pfunc) {
				this->_pfunc = p;
				(void)this->check_distance_func();
				this->_vec.clear();
			}
		}
		const IndivTypePtrVector & indivs(void) const {
			return (this->_vec);
		}
		size_t indivs_count(void) const {
			return (this->_vec.size());
		}
		IndivTypePtr indiv_at(const size_t ipos) const {
			assert(ipos < this->_vec.size());
			return ((this->_vec)[ipos]);
		}
		DistanceType distance(const IndivType *pInd1, const IndivType *pInd2) const {
			assert(pInd1 != nullptr);
			assert(pInd2 != nullptr);
			DistanceType r = 0;
			if (this->_pfunc != nullptr) {
				pInd1->distance(*pInd2, r, this->_pfunc);
			}
			else if (_st_func.get() != nullptr) {
				pInd1->distance(*pInd2, r, _st_func.get());
			}
			return (r);
		}// distance
		DistanceType distance(const IndivTypePtr &oInd1, const IndivTypePtr &oInd2) const {
			const IndivType *p1 = oInd1.get();
			const IndivType *p2 = oInd2.get();
			assert(p1 != nullptr);
			assert(p2 != nullptr);
			return this->distance(p1, p2);
		}// distance
		DistanceType distance_index(const size_t i, const size_t i2) const {
			const IndivTypePtrVector & vv = this->_vec;
			assert(i1 < vv.size());
			assert(i2 < vv.size());
			return (this->distance(vv[i1], vv[i2]));
		}// distance
		DistanceType distance(const ClusterTypePtr &oInd1, const IndivTypePtr &oInd2) const {
			const ClusterType *p1 = oInd1.get();
			const IndivType *p2 = oInd2.get();
			assert(p1 != nullptr);
			assert(p2 != nullptr);
			return this->distance(p1, p2);
		}// distance
		DistanceType distance(const IndivTypePtr &oInd1, const ClusterTypePtr &oInd2) const {
			const IndivType *p1 = oInd1.get();
			const ClusterType *p2 = oInd2.get();
			assert(p1 != nullptr);
			assert(p2 != nullptr);
			return this->distance(p1, p2);
		}// distance
	public:
		bool check_data(void) {
			if (!this->_vec.empty()) {
				return (true);
			}
			if (!this->check_distance_func()) {
				return (false);
			}
			return (this->initialize_indivs());
		}// checkData
		bool generate_random_cluster(ClusterTypePtr &oCluster, const IndexType aIndex) {
			oCluster.reset();
			if (!this->check_data()) {
				return (false);
			}
			DataTypeArray v;
			this->_pdata->get_random_data(this->_mode, v);
			oCluster = std::make_shared<ClusterType>(aIndex, v);
			return (oCluster.get() != nullptr);
		}//generate_random_cluster
		size_t clusterize(ClusterTypePtrVector &clusters,
			const size_t nbClusters = 5,
			const size_t nbIters = NB_ITER_MAX) {
			assert(nbClusters > 0);
			assert(nbIters > 0);
			assert(this->is_valid());
			size_t count = 0;
			if (!this->check_data()) {
				return (count);
			}
			size_t nc = nbClusters;
			const size_t nbIndivs = this->indivs_count();
			if (nc > nbIndivs) {
				nc = nbIndivs;
			}
			clusters.resize(nc);
			for (size_t i = 0; i < nc; ++i) {
				ClusterTypePtr o;
				if (!this->generate_random_cluster(o, (IndexType)i)) {
					clusters.clear();
					return (count);
				}
				clusters[i] = o;
			}// i
			IndexTypeMap oldMap;
			this->aggreg_one_step(oldMap, clusters);
			ClusterTypePtrVector oldClusters(clusters);
			assert(oldMap.size() == nbIndivs);
			for (size_t iter = 0; iter < nbIters; ++iter) {
				IndexTypeMap curMap;
				this->aggreg_one_step(curMap, clusters);
				assert(curMap.size() == nbIndivs);
				++count;
				bool bDone = true;
				for (auto it = oldClusters.begin(); it != oldClusters.end(); ++it) {
					ClusterTypePtr c1 = *it;
					const ClusterType *p1 = c1.get();
					assert(p1 != nullptr);
					bool bFound = false;
					for (auto jt = clusters.begin(); jt != clusters.end(); ++jt) {
						ClusterTypePtr c2 = *jt;
						const ClusterType *p2 = c2.get();
						assert(p2 != nullptr);
						if (p1->same_set(*p2)) {
							bFound = true;
							break;
						}
					}// jt
					if (!bFound) {
						bDone = false;
						break;
					}
				}// it
				if (bDone) {
					break;
				}
			}// iter
			ClusterTypePtrVector temp;
			for (auto it = clusters.begin(); it != clusters.end(); ++it) {
				ClusterTypePtr c = *it;
				ClusterType *pCluster = c.get();
				assert(pCluster != nullptr);
				if (!pCluster->empty()) {
					temp.push_back(c);
				}
			}// it
			if (temp.size() < clusters.size()) {
				clusters = temp;
			}
			return count;
		}// clusterize
	protected:
		size_t find_nearest_cluster(const ClusterTypePtrVector &cc, const IndivTypePtr &oInd) const {
			const size_t nc = cc.size();
			assert(nc > 0);
			size_t imin = 0;
			DistanceType dmin = this->distance(cc[0], oInd);
			for (size_t i = 1; i < nc; ++i) {
				DistanceType d = this->distance(cc[i], oInd);
				if (d < dmin) {
					dmin = d;
					imin = i;
				}
			}// i
			return imin;
		}//find_nearest_cluster
		void aggreg_one_step(IndexTypeMap &oMap, ClusterTypePtrVector &cc) const {
			const size_t nc = cc.size();
			const size_t n = this->indivs_count();
			for (size_t i = 0; i < nc; ++i) {
				ClusterTypePtr o = cc[i];
				ClusterType *p = o.get();
				assert(p != nullptr);
				p->reset();
			}
			oMap.clear();
			for (size_t i = 0; i < n; ++i) {
				IndivTypePtr oInd = this->indiv_at(i);
				size_t ipos = this->find_nearest_cluster(cc, oInd);
				ClusterTypePtr c = cc[ipos];
				ClusterType *pCluster = c.get();
				assert(pCluster != nullptr);
				pCluster->add(oInd, false);
			}// i
			for (size_t i = 0; i < nc; ++i) {
				ClusterTypePtr c = cc[i];
				ClusterType *pCluster = c.get();
				assert(pCluster != nullptr);
				pCluster->update_center();
				const IndivTypePtrVector & vv = pCluster->members();
				const size_t nn = vv.size();
				const IndexType val = pCluster->index();
				for (size_t j = 0; j < nn; ++j) {
					IndivTypePtr oInd = vv[j];
					const IndivType *pInd = oInd.get();
					assert(pInd != nullptr);
					const IndexType key = pInd->index();
					oMap[key] = val;
				}// i
			}// i
		}// aggreg_one_step
		bool check_distance_func() {
			if (this->_pfunc != nullptr) {
				return (true);
			}
			DistanceFuncType *p = _st_func.get();
			if (p != nullptr) {
				this->_pfunc = p;
				return (true);
			}
			_st_func.reset(new ManhattanDistanceFunc<DataType, DistanceType>());
			p = _st_func.get();
			if (p != nullptr) {
				this->_pfunc = p;
				return (true);
			}
			return (false);
		}
		bool initialize_indivs(void) {
			if (!this->is_valid()) {
				return (false);
			}
			
			IndivTypePtrVector &vv = this->_vec;
			MatDataType *pData = this->_pdata;
			DataMode m = this->_mode;
			size_t n = pData->get_data_count(m);
			vv.resize(n);
			for (size_t i = 0; i < n; ++i) {
				DataTypeArray v;
				pData->data_at(i, m, v);
				IndexType aIndex = (IndexType)i;
				vv[i] = std::make_shared<IndivType>(aIndex, v);
			}// i
			return (!vv.empty());
		}// initialize_indivs
	};// class Indivs<T,U,Z,S>
	//////////////////////////////////
	template <typename T, typename U, typename Z, class S>
	const size_t Indivs<T, U, Z, S>::NB_ITER_MAX = 100;
	template <typename T, typename U, typename Z, class S>
	std::shared_ptr<ManhattanDistanceFunc<T, Z> > Indivs<T, U, Z, S>::_st_func;
	//////////////////////////////////////
}// namespace info
////////////////////////////////
#endif // !__INDIVS_H__
