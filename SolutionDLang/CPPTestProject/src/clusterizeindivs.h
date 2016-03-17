#pragma once
#ifndef __CLUSTERIZEINDIVS_H__
#define __CLUSTERIZEINDIVS_H__
/////////////////////////////////
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
		typedef std::map<IndexType, IndexType> IndexTypeMap;
		//
		typedef Indivs<DataType, IndexType, DistanceType, StringType> IndivsType;
		typedef ClusterizeIndivs<T, U, Z, S> ClusterizeIndivsType;
	private:
		IndivsType *_pIndivs;
		ClusterTypePtrVector _clusters;
	public:
		ClusterizeIndivs() :_pIndivs(nullptr) {}
		ClusterizeIndivs(IndivsType *pIndivs, const ClusterTypePtrVector *pc = nullptr) :_pIndivs(pIndivs) {
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
			const ClusterType *p = (clusters[i]).get();
			assert(p != nullptr);
			return (p);
		}// cluster_at
	public:
		size_t clusterize(const size_t nbClusters = IndivsType::NB_DEFAULT_CLUSTERS,
			const size_t IndivsType::nbIters = NB_ITER_MAX) {
			assert(nbClusters > 0);
			assert(nbIters > 0);
			assert(this->is_valid());
			ClusterTypePtrVector &clusters = this->_clusters;
			IndivsType *pIndivs = this->_pIndivs;
			size_t count = 0;
			size_t nc = nbClusters;
			const size_t nbIndivs = this->indivs_count();
			if (nc > nbIndivs) {
				nc = nbIndivs;
			}
			clusters.resize(nc);
			{
				std::vector<size_t> oTemp(nbIndivs);
				for (size_t i = 0; i < nbIndivs; ++i) {
					oTemp[i] = i;
				}// i
				std::random_shuffle(oTemp.begin(), oTemp.end());
				for (size_t i = 0; i < nc; ++i) {
					size_t pos = oTemp[i];
					IndivTypePtr oInd = pIndivs->indiv_at(pos);
					IndivType *pInd = oInd.get();
					assert(pInd != nullptr);
					ClusterTypePtr o = std::make_shared<ClusterType>((IndexType)i, pInd->value());
					ClusterType *pCluster = o.get();
					assert(pCluster != nullptr);
					pCluster->add(oInd, true);
					clusters[i] = o;
				}// i
			}// init
			IndexTypeMap oldMap;
			this->aggreg_one_step(oldMap);
			assert(oldMap.size() == nbIndivs);
			ClusterTypePtrVector oldClusters(clusters);
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
		size_t find_nearest_cluster(const IndivTypePtr &oInd) const {
			IndivsType *pIndivs = this->_pIndivs;
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
			IndivsType *pIndivs = this->_pIndivs;
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
				IndivTypePtr oInd = pIndivs->indiv_at(i);
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
	};// class ClusterizeIndivs<T,U,Z,S>
	///////////////////////////////////////////////
}// namespace info
//////////////////////////////////////
#endif // !__CLUSTERIZEINDIVS_H__

