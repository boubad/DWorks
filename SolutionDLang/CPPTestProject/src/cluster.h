#pragma once
#ifndef __CLUSTER_H__
#define __CLUSTER_H__
///////////////////////////////////
#include "indiv.h"
//////////////////////////
#include <set>
//////////////////////////////////////////
namespace info {
	///////////////////////////////////
	template <typename T = int, typename U = int> class Cluster : public Indiv<T, U> {
	private:
		std::set<U> _members;
		std::valarray<double> _sum;
	public:
		Cluster() {}
		Cluster(const U aIndex) :Indiv<T, U>(aIndex) {}
		Cluster(const U aIndex, const std::valarray<T> &oCenter) :
			Indiv<T,U>(aIndex, oCenter) {}
		Cluster(const Cluster<T, U> &other) :Indiv<T, U>(other),
			_members(other._members), _sum(other._sum) {}
		Cluster<T, U> & operator=(const Cluster<T, U > &other) {
			if (this != &other) {
				Indiv<T, U>::operator=(other);
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
		const std::set<U> & members(void) const {
			return (this->_members);
		}
	public:
		void reset(void) {
			std::valarray<double> b;
			this->_members.clear();
			this->_sum = b;
		}// reset
		void update_center(void) {
			std::valarray <double>  &sum = this->_sum;
			const size_t n = sum.size();
			const size_t nTotal = this->_members.size();
			if ((n > 0) && (nTotal > 0)) {
				std::valarray<T> val(n);
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
		void add(const Indiv<T, U> &oInd, bool bUpdate = false) {
			this->add(oInd.index(), oInd.value(), bUpdate);
		}// add
		void add(const U aIndex, const std::valarray<T> &val, bool bUpdate = false) {
			if (!this->contains(aIndex)) {
				const size_t n = val.size();
				std::valarray<double> & sum = this->_sum;
				std::set<U> &oSet = this->_members;
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
		virtual std::ostream & write_to(std::ostream &os) const {
			os << "{" << this->index() << " ,[";
			const std::valarray<T> &data = this->value();
			const size_t n = data.size();
			for (size_t i = 0; i < n; ++i) {
				if (i > 0) {
					os << ", ";
				}
				os << data[i];
			}// i
			os << "], [";
			const std::set<U> &oSet = this->_members;
			for (auto it = oSet.begin(); it != oSet.end(); ++it) {
				if (it != oSet.begin()) {
					os << ", ";
				}
				os << *it;
			}// it
			os << " ] }";
			return (os);
		}// write_to
		virtual std::wostream & write_to(std::wostream &os) const {
			os << L"{" << this->index() << L" ,[";
			const std::valarray<T> &data = this->value();
			const size_t n = data.size();
			for (size_t i = 0; i < n; ++i) {
				if (i > 0) {
					os << L", ";
				}
				os << data[i];
			}// i
			os << L"], [";
			const std::set<U> &oSet = this->_members;
			for (auto it = oSet.begin(); it != oSet.end(); ++it) {
				if (it != oSet.begin()) {
					os << L", ";
				}
				os << *it;
			}// it
			os << L" ] }";
			return (os);
		}// write_to
	};// class Cluster<T,U>
	//////////////////////////////////////
}// namespace info
//////////////////////////////////////
#endif // !__CLUSTER_H__
