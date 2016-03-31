#pragma once
#ifndef __CLUSTER_H__
#define __CLUSTER_H__
///////////////////////////////////
#include "indiv.h"
//////////////////////////
namespace info {
	///////////////////////////////////
	template <typename T = int, typename U = int> class Cluster : public Indiv<T, U> {
	public:
		typedef T DataType;
		typedef U IndexType;
		//
		typedef std::set<IndexType> IndexTypeSet;
		typedef std::valarray<DataType> DataTypeArray;
		typedef std::valarray<double> DoubleTypeArray;
		//
		typedef Indiv<DataType, IndexType> IndivType;
		typedef Cluster<DataType, IndexType> ClusterType;
	private:
		std::set<U> _members;
		std::valarray<double> _sum;
	public:
		Cluster() {}
		Cluster(const IndexType aIndex) :IndivType(aIndex) {}
		Cluster(const IndexType aIndex,const StringType &sid) :IndivType(aIndex,sid) {}
		Cluster(const IndexType aIndex, const DataTypeArray &oCenter) :
			IndivType(aIndex, oCenter) {}
		Cluster(const IndexType aIndex, const StringType &sid,const DataTypeArray &oCenter) :
			IndivType(aIndex, sid,oCenter) {}
		Cluster(const ClusterType &other) :IndivType(other),
			_members(other._members), _sum(other._sum) {}
		ClusterType & operator=(const ClusterType &other) {
			if (this != &other) {
				IndivType::operator=(other);
				this->_members = other._members;
				this->_sum = other._sum;
			}
			return (*this);
		}
		virtual ~Cluster() {}
	public:
		bool empty(void) const {
			return (this->_members.empty());
		}
	public:
		const IndexTypeSet & members(void) const {
			return (this->_members);
		}
	public:
		void reset(void) {
			DoubleTypeArray b;
			this->_members.clear();
			this->_sum = b;
		}// reset
		void update_center(void) {
			DoubleTypeArray  &sum = this->_sum;
			const size_t n = sum.size();
			const size_t nTotal = this->_members.size();
			if ((n > 0) && (nTotal > 0)) {
				DataTypeArray val(n);
				for (size_t i = 0; i < n; ++i) {
					val[i] = (T)(sum[i] / nTotal);
				}// i
				this->value(val);
			}// n
		}// update_centers
		bool contains(const U aIndex) {
			auto it = this->_members.find(aIndex);
			return (it != this->_members.end());
		}// contains
		void add(const IndivType &oInd, bool bUpdate = false) {
			this->add(oInd.index(), oInd.value(), bUpdate);
		}// add
		void add(const IndexType aIndex, const DataTypeArray &val, bool bUpdate = false) {
			if (!this->contains(aIndex)) {
				const size_t n = val.size();
				DoubleTypeArray & sum = this->_sum;
				IndexTypeSet &oSet = this->_members;
				if (oSet.empty()) {
					sum.resize(n);
					for (size_t i = 0; i < n; ++i) {
						sum[i] = (double)val[i];
					}// i
				}
				else {
					const size_t nc = sum.size();
					assert(val.size() >= nc);
					for (size_t i = 0; i < nc; ++i) {
						sum[i] += (double)val[i];
					}// i
				}
				oSet.insert(aIndex);
				if (bUpdate) {
					this->update_center();
				}
			}//not contains
		}// add
	public:
		virtual OStreamType & write_to(OStreamType &os) const {
			os << START_OBJECT;
			os << this->index() << STRING_COMMA << this->id() << STRING_COMMA;
			info_write_array(os,this->value());
			os << STRING_COMMA;
			info_write_array(os, this->_members);
			os << END_OBJECT;
			return (os);
		}// write_to
	};// class Cluster<T,U>
	//////////////////////////////////////
}// namespace info
/////////////////////////////////////////
template <typename T, typename U>
info::OStreamType operator<<(info::OStreamType &os, const info::Cluster<T, U> &ind) {
	return ind.write_to(os);
}
//////////////////////////////////////
#endif // !__CLUSTER_H__
