#pragma once
#ifndef __CLUSTERIZEINDIVS_H__
#define __CLUSTERIZEINDIVS_H__
/////////////////////////////////
#include "resultitem.h"
#include "indivs.h"
/////////////////////////////////////////
namespace info {
	//////////////////////////////////////////////
	template <typename T = int, typename U = int, typename Z = long, class S = std::wstring>
	class ClusterizeIndivs {
	public:
		typedef T DataType;
		typedef U IndexType;
		typedef Z DistanceType;
		typedef S StringType;
		//
		typedef std::valarray<DataType> DataTypeArray;
		typedef DistanceFunc<DataType, DistanceType> DistanceFuncType;
		typedef MatData<DataType,StringType> MatDataType;
		typedef Indiv<DataType, IndexType, StringType> IndivType;
		typedef std::shared_ptr<IndivType> IndivTypePtr;
		typedef std::vector<IndivTypePtr> IndivTypePtrVector;
		typedef IndivSet<DataType, IndexType, StringType> ClusterType;
		typedef std::shared_ptr<ClusterType> ClusterTypePtr;
		typedef std::vector<ClusterTypePtr> ClusterTypePtrVector;
		typedef std::set<IndexType> IndexTypeSet;
		typedef std::map<IndexType, IndexTypeSet> IndexTypeMap;
		//
		typedef Indivs<DataType, IndexType, DistanceType, StringType> IndivsType;
		typedef ClusterizeIndivs<DataType, IndexType, DistanceType, StringType> ClusterizeIndivsType;
		typedef ResultItem<IndexType, size_t> ResultItemType;
	private:
		const IndivsType *_pIndivs;
		ClusterTypePtrVector _clusters;
	public:
		ClusterizeIndivs() :_pIndivs(nullptr) {}
		ClusterizeIndivs(const IndivsType *pIndivs, const ClusterTypePtrVector *pc = nullptr) :_pIndivs(pIndivs) {
			assert(this->_pIndivs != nullptr);
			assert(this->_pIndivs->is_valid());
			assert(this->_pIndivs->indivs_count() > 2);
			if (pc != nullptr) {
				this->_clusters = *pc;
			}
		}
		ClusterizeIndivs(const ClusterizeIndivsType &other):_pIndivs(other._pIndivs),_clusters(other._clusters){}
		ClusterizeIndivsType & operator=(const ClusterizeIndivsType &other) {
			if (this != &other) {
				this->_pIndivs = other._pIndivs;
				this->_clusters = other._clusters;
			}
			return (*this);
		}
		virtual ~ClusterizeIndivs(){}
	public:
		bool is_valid(void) const {
			return ((this->_pIndivs != nullptr) && (this->_pIndivs->is_valid()) &&
				(this->_pIndivs->indivs_count() > 2));
		}
		size_t clusters_count(void) const {
			return (this->_clusters.size());
		}
		const ClusterType *cluster_at(const size_t i) const {
			const ClusterTypePtrVector & clusters = this->_clusters;
			assert(i < clusters.size());
			const ClusterTypePtr &c = clusters[i];
			const ClusterType *p = c.get();
			assert(p != nullptr);
			return (p);
		}// cluster_at
	public:
		void multi_clusterize(ResultItemType &oResult,
			const size_t nCount = 100,
			const size_t nbClusters = IndivsType::NB_DEFAULT_CLUSTERS,
			const size_t nbIters = IndivsType::NB_ITER_MAX) {
			oResult.reset();
			for (size_t i = 0; i < nCount; ++i) {
				IndexTypeMap resultMap;
				this->clusterize(resultMap, nbClusters, nbIters);
				for (auto it = resultMap.begin(); it != resultMap.end(); ++it) {
					auto oSet = (*it).second;
					oResult.increment(oSet);
				}// it
			}// i
		}//multi_clusterize
		size_t clusterize(IndexTypeMap &resultMap,
			const size_t nbClusters = IndivsType::NB_DEFAULT_CLUSTERS,
			const size_t nbIters = IndivsType::NB_ITER_MAX) {
			assert(nbClusters > 0);
			assert(nbIters > 0);
			assert(this->is_valid());
			ClusterTypePtrVector &clusters = this->_clusters;
			const IndivsType *pIndivs = this->_pIndivs;
			size_t count = 0;
			size_t nc = nbClusters;
			const size_t nbIndivs = pIndivs->indivs_count();
			if (nc >= nbIndivs) {
				nc = (size_t)(nbIndivs - 1);
			}
			assert(nc > 0);
			clusters.resize(nc);
			{
				std::vector<size_t> oTemp(nbIndivs);
				for (size_t i = 0; i < nbIndivs; ++i) {
					oTemp[i] = i;
				}// i
				std::random_shuffle(oTemp.begin(), oTemp.end());
				for (size_t i = 0; i < nc; ++i) {
					size_t ipos = oTemp[i];
					const IndivType *pInd = pIndivs->indiv(ipos);
					assert(pInd != nullptr);
					ClusterTypePtr o = std::make_shared<ClusterType>((IndexType)i, pInd->id(), pInd->value());
					ClusterType *pCluster = o.get();
					assert(pCluster != nullptr);
					pCluster->add(o, true);
					clusters[i] = o;
				}// i
			}// init
			IndexTypeMap oldMap;
			this->aggreg_one_step(oldMap);
			for (size_t iter = 0; iter < nbIters; ++iter) {
				this->aggreg_one_step(resultMap);
				++count;
				bool bDone = true;
				for (auto it = oldMap.begin(); it != oldMap.end(); ++it) {
					if (!bDone) {
						break;
					}
					const IndexType key = (*it).first;
					const IndexTypeSet &oldSet = (*it).second;
					assert(resultMap.find(key) != resultMap.end());
					const IndexTypeSet &curSet = resultMap[key];
					for (auto jt = oldSet.begin(); jt != oldSet.end(); ++jt) {
						const IndexType val = *jt;
						if (curSet.find(val) == curSet.end()) {
							bDone = false;
							break;
						}
					}// jt
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
		void compute_stats(DataTypeArray &vCenter, DistanceType &vTotal, DistanceType &vInter, DistanceType & vIntra) const {
			assert(this->is_valid());
			const IndivsType *pIndivs = this->_pIndivs;
			const ClusterTypePtrVector &clusters = this->_clusters;
			const size_t nbIndivs = pIndivs->indivs_count();
			assert(nbIndivs > 0);
			{
				ClusterTypePtr oo = std::make_shared<ClusterType>();
				ClusterType *pCenter = oo.get();
				assert(pCenter != nullptr);
				for (size_t i = 0; i < nbIndivs; ++i) {
					IndivTypePtr o = this->indiv_at(i);
					pCenter->add(o, false);
				}// i
				pCenter->update_center();
				vCenter = pCenter->value();
			}// center
			const size_t nc = clusters.size();
			assert(nc > 0);
			vTotal = 0;
			vInter = 0;
			vIntra = 0;
			for (size_t i = 0; i < nc; ++i) {
				ClusterTypePtr c = clusters[i];
				const ClusterType *pCluster = c.get();
				assert(pCluster != nullptr);
				DistanceType x1 = 0;
				pCluster->intra_inertia(x1);
				vIntra = (DistanceType)(vIntra + x1);
				DataTypeArray  v = pCluster->value();
				DataTypeArray t = v - vCenter;
				DataTypeArray tt = t * t;
				vInter = (DistanceType)(vInter + tt.sum());
			}// i
			vInter = (DistanceType)(vInter / nc);
			vIntra = (DistanceType)(vIntra / nc);
			vTotal = (DistanceType)(vInter + vIntra);
		}// compute_stats
	protected:
		void copy_clusters(ClusterTypePtrVector &oRes) const {
			const ClusterTypePtrVector &vs = this->_clusters;
			const size_t nc = vs.size();
			oRes.resize(nc);
			for (size_t i = 0; i < nc; ++i) {
				const ClusterTypePtr &o = vs[i];
				const ClusterType *p = o.get();
				assert(p != nullptr);
				oRes[i] = std::make_shared<ClusterType>(*p);
			}// i
		}//copy_clusters 
		size_t find_nearest_cluster(const IndivTypePtr &oInd) const {
			const IndivsType *pIndivs = this->_pIndivs;
			assert(pIndivs != nullptr);
			assert(oInd.get() != nullptr);
			const ClusterTypePtrVector &cc = this->_clusters;
			const size_t nc = cc.size();
			assert(nc > 0);
			size_t imin = 0;
			DistanceType dmin = pIndivs->distance(cc[0], oInd);
			for (size_t i = 1; i < nc; ++i) {
				DistanceType d = pIndivs->distance(cc[i], oInd);
				if (d < dmin) {
					dmin = d;
					imin = i;
				}
			}// i
			return imin;
		}//find_nearest_cluster
		void aggreg_one_step(IndexTypeMap &oMap) {
			const IndivsType *pIndivs = this->_pIndivs;
			assert(pIndivs != nullptr);
			ClusterTypePtrVector &cc = this->_clusters;
			const size_t nc = cc.size();
			const size_t n = pIndivs->indivs_count();
			for (size_t i = 0; i < nc; ++i) {
				ClusterTypePtr o = cc[i];
				ClusterType *p = o.get();
				assert(p != nullptr);
				p->reset();
			}
			oMap.clear();
			for (size_t i = 0; i < n; ++i) {
				const IndivTypePtr oInd = pIndivs->indiv_at(i);
				size_t ipos = this->find_nearest_cluster(oInd);
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
				IndexTypeSet oSet;
				const IndexType key = pCluster->index();
				for (size_t j = 0; j < nn; ++j) {
					IndivTypePtr oInd = vv[j];
					const IndivType *pInd = oInd.get();
					assert(pInd != nullptr);
					const IndexType key = pInd->index();
					oSet.insert(key);
				}// j
				oMap[key] = oSet;
			}// i
		}// aggreg_one_step
	};// class ClusterizeIndivs<T,U,Z,S>
	///////////////////////////////////////////////
}// namespace info
//////////////////////////////////////
#endif // !__CLUSTERIZEINDIVS_H__

