#pragma once
#ifndef __CLUSTERIZE_H__
#define __CLUSTERIZE_H__
/////////////////////////////
#include "distance.h"
#include "matdata.h"
#include "cluster.h"
#include "utils.h"
/////////////////////////////
#include <memory>
#include <set>
#include <vector>
#include <map>
#include <algorithm>
////////////////////////////////
namespace info {
	////////////////////////////////////////////
	template <typename T = int, typename U = int, class S = std::wstring, typename Z = long> class ClusterManager {
	public:
		typedef T DataType;
		typedef U IndexType;
		typedef S StringType;
		typedef Z DistanceType;
		//
		typedef std::valarray<T> DataTypeArray;
		typedef DistanceFunc<DataType, DistanceType> DistanceFuncType;
		typedef MatData<DataType> MatDataType;
		typedef Cluster<DataType, IndexType,StringType> ClusterType;
		typedef std::vector<ClusterType> ClusterTypeVector;
		typedef ClusterManager<DataType, IndexType, StringType, DistanceType> ClusterManagerType;
	private:
		size_t _nbclusters;
		DataMode _mode;
		const MatDataType *_pdata;
		DistanceFuncType *_pfunc;
		ClusterTypeVector _clusters;
		//
		static const size_t NB_DEFAULT_CLUSTERS;
		static const size_t NB_MAX_ITERS;
	public:
		ClusterManager() :_nbclusters(0), _mode(DataMode::noMode),
			_pdata(nullptr), _pfunc(nullptr) {}
		ClusterManager(const MatDataType *pData, const size_t nbclusters = NB_DEFAULT_CLUSTERS,
			const DataMode m = DataMode::modeRow,
			DistanceFuncType *pDist = nullptr) : _nbclusters(nbclusters), _mode(m), _pdata(pData),
			_pfunc(pDist) {
			assert(_nbclusters > 0);
			assert(this->_mode != DataMode::noMode);
			assert(this->_pdata != nullptr);
			assert(this->_pdata->is_valid());
			if ((this->_mode == DataMode::modeRow) && (this->_nbclusters > this->_pdata->rows())) {
				this->_nbclusters = this->_pdata->rows();
			}
			else if ((this->_mode == DataMode::modeCol) && (this->_nbclusters > this->_pdata->cols())) {
				this->_nbclusters = this->_pdata->cols();
			}
		}
		ClusterManager(const ClusterManagerType &other) :_nbclusters(other._nbclusters), _mode(other._mode),
			_pdata(other._pdata), _pfunc(other._pfunc), _clusters(other._clusters) {}
		ClusterManagerType & operator=(const ClusterManagerType &other) {
			if (this != &other) {
				this->_nbclusters = other._nbclusters;
				this->_mode = other._mode;
				this->_pdata = other._pdata;
				this->_pfunc = other._pfunc;
				this->_clusters = other._clusters;
			}
			return (*this);
		}
		virtual ~ClusterManager() {}
	public:
		bool is_valid(void) const {
			return ((this->_nbclusters > 0) && (this->_mode != DataMode::noMode) &&  (this->_pdata != nullptr));
		}// is_valid
		const ClusterTypeVector & clusters(void) const {
			return (this->_clusters);
		}
		DataMode mode(void) const {
			return (this->_mode);
		}
		void mode(const DataMode m) {
			if (this->_mode != m) {
				this->_mode = m;
				this->_clusters.clear();
				if ((this->_mode == DataMode::modeRow) && (this->_nbclusters > this->_pdata->rows())) {
					this->_nbclusters = this->_pdata->rows();
				}
				else if ((this->_mode == DataMode::modeCol) && (this->_nbclusters > this->_pdata->cols())) {
					this->_nbclusters = this->_pdata->cols();
				}
			}
		}
		DistanceFuncType *distance_func(void) const {
			return (this->_pfunc);
		}
		void distance_func(DistanceFuncType *f) {
			if (this->_pfunc != f) {
				this->_pfunc = f;
				this->_clusters.clear();
			}
		}
		size_t current_clusters_count(void) const {
			return (this->_clusters.size());
		}
		size_t candidate_clusters_count(void) const {
			return (this->_nbclusters);
		}
		void candidate_clusters_count(const size_t n) {
			if (this->_nbclusters != n) {
				this->_nbclusters = n;
				this->_clusters.clear();
				if ((this->_mode == DataMode::modeRow) && (this->_nbclusters > this->_pdata->rows())) {
					this->_nbclusters = this->_pdata->rows();
				}
				else if ((this->_mode == DataMode::modeCol) && (this->_nbclusters > this->_pdata->cols())) {
					this->_nbclusters = this->_pdata->cols();
				}
			}
		}
	public:
		size_t clusterize(const size_t nbIters = NB_MAX_ITERS) {
			assert(nbIters > 0);
			assert(this->is_valid());
			DistanceFuncType *pf = this->_pfunc;
			std::unique_ptr<ManhattanDistanceFunc<T, Z > > oDist;
			if (pf == nullptr) {
				oDist.reset(new ManhattanDistanceFunc<T, Z >());
				pf = oDist.get();
			}
			assert(pf != nullptr);
			this->initialize();
			std::map<U, U> oldMap;
			this->perform_one_step(oldMap, pf);
			size_t count = 0;
			for (size_t iter = 0; iter < nbIters; ++iter) {
				std::map<U, U> curMap;
				this->perform_one_step(curMap, pf);
				++count;
				bool bDone = true;
				for (auto it = oldMap.begin(); it != oldMap.end(); ++it) {
					U key = (*it).first;
					U val = (*it).second;
					auto jt = curMap.find(key);
					assert(jt != curMap.end());
					U val2 = curMap[key];
					if (val != val2) {
						bDone = false;
						break;
					}
				}// it
				if (bDone) {
					break;
				}
				oldMap = curMap;
			}// iter
			ClusterTypeVector v;
			ClusterTypeVector & cc = this->_clusters;
			const size_t nc = cc.size();
			for (size_t i = 0; i < nc; ++i) {
				ClusterType &c = cc[i];
				if (!c.empty()) {
					v.push_back(cc[i]);
				}
			}// i
			std::sort(v.begin(), v.end());
			this->_clusters = v;
			return count;
		}// clusterize
	protected:
		void initialize(void) {
			const size_t nc = this->_nbclusters;
			assert(nc > 0);
			this->_clusters = ClusterTypeVector(nc);
			ClusterTypeVector & cc = this->_clusters;
			DataMode m = this->_mode;
			const MatDataType *pData = this->_pdata;
			assert(pData != nullptr);
			for (size_t i = 0; i < nc; ++i) {
				DataTypeArray v;
				pData->get_random_data(m, v);
				assert(v.size() > 0);
				cc[i] = ClusterType((U)i, v);
			}// i
		}// initialize
		size_t get_nearest_cluster(const DataTypeArray &data, DistanceFuncType *pf) const {
			assert(pf != nullptr);
			const ClusterTypeVector & cc = this->_clusters;
			const size_t nc = cc.size();
			assert(nc > 0);
			const ClusterType &c0 = cc[0];
			Z mindistance = (*pf)(c0.value(), data);
			size_t minindex = 0;
			for (size_t i = 1; i < nc; ++i) {
				const ClusterType &c = cc[i];
				Z d = (*pf)(c.value(), data);
				if (d < mindistance) {
					mindistance = d;
					minindex = i;
				}
			}// i
			return minindex;
		} // get_nearest_cluster
		void perform_one_step(std::map<U, U> &oMap, DistanceFuncType *pf) {
			assert(pf != nullptr);
			ClusterTypeVector & cc = this->_clusters;
			const size_t nc = cc.size();
			assert(nc > 0);
			DataMode m = this->_mode;
			const MatDataType *pData = this->_pdata;
			assert(pData != nullptr);
			oMap.clear();
			const size_t n = pData->get_data_count(m);
			for (size_t i = 0; i < nc; ++i) {
				ClusterType &c = cc[i];
				c.reset();
			}
			for (size_t i = 0; i < n; ++i) {
				DataTypeArray data;
				pData->data_at(i, m, data);
				size_t ipos = this->get_nearest_cluster(data, pf);
				assert(ipos < nc);
				ClusterType &c = cc[ipos];
				c.add((U)i, data, false);
			}// i
			for (size_t i = 0; i < nc; ++i) {
				ClusterType &c = cc[i];
				c.update_center();
				IndexType val = c.index();
				auto oSet = c.members();
				for (auto it = oSet.begin(); it != oSet.end(); ++it) {
					IndexType key = *it;
					oMap[key] = val;
				}// it
			}// i
		}// perform_one_step
	public:
		virtual std::ostream & write_to(std::ostream &os) const {
			const ClusterTypeVector &cc = this->_clusters;
			for (auto it = cc.begin(); it != cc.end(); ++it) {
				auto c = *it;
				c.write_to(os);
				os << "\t" << c.trace() << std::endl;
			}
			return (os);
		}// write_to
		virtual std::wostream & write_to(std::wostream &os) const {
			const ClusterTypeVector &cc = this->_clusters;
			for (auto it = cc.begin(); it != cc.end(); ++it) {
				auto c = *it;
				c.write_to(os);
				os << L"\t" << c.trace() << std::endl;
			}
			return (os);
		}// write_to
		void toString(std::string &s) const {
			std::stringstream os;
			this->write_to(os);
			s = os.str();
		}// toString
		void toString(std::wstring &s) const {
			std::wstringstream os;
			this->write_to(os);
			s = os.str();
		}// toString
	};// class ClusterManager<T,U,Z>
	///////////////////////////////////////
	template <typename T, typename U, class S,typename Z>
	const size_t ClusterManager<T, U, S, Z>::NB_DEFAULT_CLUSTERS = 5;
	template <typename T, typename U, class S, typename Z>
	const size_t ClusterManager<T, U, S, Z>::NB_MAX_ITERS = 50;
	/////////////////////////////////////////////
}// namespace info
/////////////////////////////////////
template <typename T, typename U, class S,typename Z>
std::ostream & operator<<(std::ostream &os, const info::ClusterManager<T, U, S,Z> &d) {
	return d.write_to(os);
}
template <typename T, typename U, class S, typename Z>
std::wostream & operator<<(std::wostream &os, const info::ClusterManager<T, U, S,Z> &d) {
	return d.write_to(os);
}
/////////////////////////////
#endif // !__CLUSTERIZE_H__
///////////////////////////////
//eof: clusterize.h
