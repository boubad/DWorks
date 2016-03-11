#pragma once
#ifndef __CLUSTERIZE_H__
#define __CLUSTERIZE_H__
/////////////////////////////
#include "distance.h"
#include "matdata.h"
#include "cluster.h"
#include "utils.h"
/////////////////////////////
#include <set>
#include <vector>
#include <map>
#include <random>
////////////////////////////////
namespace info {
	////////////////////////////////////////////
	template <typename T = int, typename U = int, typename Z = long> class ClusterManager {
	public:
		typedef T DataType;
		typedef U IndexType;
		typedef Z DistanceType;
		//
		typedef DistanceFunc<DataType, DistanceType> DistanceFuncType;
		typedef MatData<DataType> MatDataType;
		typedef Cluster<DataType, DistanceType> ClusterType;
		typedef std::vector<ClusterType> ClusterTypeVector;
		typedef ClusterManager<DataType, IndexType, DistanceType> ClusterManagerType;
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
		ClusterManager() :_nbclusters(0), _mode(DataMode::modeRow),
			_pdata(nullptr), _pfunc(nullptr) {}
		ClusterManager(const MatDataType *pData, const size_t nbclusters = NB_DEFAULT_CLUSTERS,
			const DataMode m = DataMode::modeRow,
			DistanceFuncType *pDist = nullptr) : _nbclusters(nbclusters), _mode(m), _pdata(pData),
			_pfunc(pDist) {
			assert(_nbclusters > 0);
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
			return ((this->_nbclusters > 0) && (this->_pdata != nullptr));
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
	};// class ClusterManager<T,U,Z>
	///////////////////////////////////////
	template <typename T, typename U, typename Z>
	const size_t ClusterManager<T, U, Z>::NB_DEFAULT_CLUSTERS = 5;
	template <typename T, typename U, typename Z>
	const size_t ClusterManager<T, U, Z>::NB_MAX_ITERS = 50;
	/////////////////////////////////////////////
}// namespace info
/////////////////////////////
#endif // !__CLUSTERIZE_H__
///////////////////////////////
//eof: clusterize.h
