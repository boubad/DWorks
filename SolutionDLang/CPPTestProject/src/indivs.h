#pragma once
#ifndef __INDIVS_H__
#define __INDIVS_H__
////////////////////////////////
#include "indivset.h"
#include "indiv.h"
#include "matdata.h"
///////////////////////////////////
namespace info {
	//////////////////////////////////////
	template <typename T = int, typename U = int, typename Z = long>
	class Indivs {
		static_assert(std::is_integral<U>::value, "index typename must be integral type");
		static_assert(sizeof(Z) >= sizeof(T), "Distance typename size must be greater or equal Data typename");
	public:
		typedef T DataType;
		typedef U IndexType;
		typedef Z DistanceType;
		//
		typedef std::valarray<DataType> DataTypeArray;
		typedef DistanceFunc<DataType, DistanceType> DistanceFuncType;
		typedef MatData<DataType> MatDataType;
		typedef Indiv<DataType, IndexType> IndivType;
		typedef std::shared_ptr<IndivType> IndivTypePtr;
		typedef std::vector<IndivTypePtr> IndivTypePtrVector;
		typedef IndivSet<DataType, IndexType> ClusterType;
		typedef std::shared_ptr<ClusterType> ClusterTypePtr;
		//
		typedef Indivs<DataType, IndexType, DistanceType> IndivsType;
	private:
		DataMode _mode;
		MatDataType *_pdata;
		DistanceFuncType *_pfunc;
		IndivTypePtrVector _vec;
	public:
		static const size_t NB_ITER_MAX;
		static const size_t NB_DEFAULT_CLUSTERS;
	public:
		Indivs() :_mode(DataMode::noMode), _pdata(nullptr), _pfunc(nullptr) {
		}
		Indivs(MatDataType *pData,
			DataMode m = DataMode::modeRow,
			DistanceFuncType *pf = nullptr) :
			_mode(m), _pdata(pData), _pfunc(pf) {
			assert(this->_pdata != nullptr);
			(void)this->check_data();
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
		void set_data(MatDataType *pData,
			DataMode m = DataMode::modeRow,
			DistanceFuncType *pf = nullptr) {
			assert(pData != nullptr);
			assert(m != DataMode::noMode);
			this->_mode = m;
			this->_pdata = pData;
			this->_pfunc = pf;
			this->_vec.clear();
			(void)this->check_data();
		}// set_data
		bool is_valid(void) const {
			return ((this->_mode != DataMode::noMode) && (this->_pdata != nullptr) &&
				(this->_pdata->is_valid()));
		}
		DataMode mode(void) const {
			return (this->_mode);
		}
		MatDataType *matdata(void) const {
			return (this->_pdata);
		}
		DistanceFuncType *distance_func(void) const {
			return (this->_pfunc);
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
		const IndivType *indiv(const size_t ipos) const {
			assert(ipos < this->_vec.size());
			const IndivTypePtr &o = this->_vec[ipos];
			const IndivType *p = o.get();
			assert(p != nullptr);
			return p;
		}
		const IndivType *find_indiv_by_index(const IndexType aIndex) const {
			const IndivTypePtrVector &vv = this->_vec;
			auto iend = vv.end();
			for (auto it = vv.begin(); it != iend; ++it) {
				const IndivTypePtr c = *it;
				const IndivType *pInd = c.get();
				assert(pInd != nullptr);
				if (pInd->index() == aIndex) {
					return (pInd);
				}
			}// it
			return (nullptr);
		}//find_indiv_by_index
		const IndivType *find_indiv_by_id(const StringType &sId) const {
			const IndivTypePtrVector &vv = this->_vec;
			auto iend = vv.end();
			for (auto it = vv.begin(); it != iend; ++it) {
				const IndivTypePtr c = *it;
				const IndivType *pInd = c.get();
				assert(pInd != nullptr);
				if (pInd->id() == sId) {
					return (pInd);
				}
			}// it
			return (nullptr);
		}//find_indiv_by_id
		DistanceType distance(const IndivType *pInd1, const IndivType *pInd2) const {
			assert(pInd1 != nullptr);
			assert(pInd2 != nullptr);
			DistanceType r = 0;
			pInd1->distance(*pInd2, r, this->_pfunc);
			return (r);
		}// distance
		DistanceType distance(const IndivType &pInd1, const IndivType &pInd2) const {
			return (this->distance(&pInd2, &pInd2));
		}// distance
		DistanceType distance(const IndivTypePtr &oInd1, const IndivTypePtr &oInd2) const {
			const IndivType *p1 = oInd1.get();
			const IndivType *p2 = oInd2.get();
			assert(p1 != nullptr);
			assert(p2 != nullptr);
			return this->distance(p1, p2);
		}// distance
		DistanceType distance_index(const size_t i1, const size_t i2) const {
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
		DistanceType distance(const ClusterTypePtr &oInd1, const ClusterTypePtr &oInd2) const {
			const ClusterType *p1 = oInd1.get();
			const ClusterType *p2 = oInd2.get();
			assert(p1 != nullptr);
			assert(p2 != nullptr);
			return this->distance(p1, p2);
		}// distance
		template <typename X>
		void compute_distances(std::vector<X> &dist) const {
			assert(this->is_valid());
			const size_t n = this->indivs_count();
			assert(n > 0);
			dist.resize(n * n);
			for (size_t i = 0; i < n; ++i) {
				dist[i * n + i] = 0;
				for (size_t j = i + 1; j < n; ++j) {
					X d = (X) this->distance_index(i, j);
					dist[i * n + j] = d;
					dist[j * n + i] = d;
				}// j
			}// i
		}// compute_distances
		template <typename X>
		void compute_distances(std::valarray<X> &dist) const {
			assert(this->is_valid());
			const size_t n = this->indivs_count();
			assert(n > 0);
			dist.resize(n * n);
			for (size_t i = 0; i < n; ++i) {
				dist[i * n + i] = 0;
				for (size_t j = i + 1; j < n; ++j) {
					X d = (X) this->distance_index(i, j);
					dist[i * n + j] = d;
					dist[j * n + i] = d;
				}// j
			}// i
		}// compute_distances
	protected:
		bool check_data(void) {
			if (!this->_vec.empty()) {
				return (true);
			}
			return (this->initialize_indivs());
		}// checkData
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
				StringType sid = pData->data_id_at(i, m);
				pData->data_at(i, m, v);
				IndexType aIndex = (IndexType)i;
				vv[i] = std::make_shared<IndivType>(aIndex, sid,v);
			}// i
			return (!vv.empty());
		}// initialize_indivs
	};// class Indivs<T,U,Z,S>
	//////////////////////////////////
	template <typename T, typename U, typename Z>
	const size_t Indivs<T, U, Z>::NB_DEFAULT_CLUSTERS = 5;
	template <typename T, typename U, typename Z>
	const size_t Indivs<T, U, Z>::NB_ITER_MAX = 100;
	//////////////////////////////////////
}// namespace info
////////////////////////////////
#endif // !__INDIVS_H__
