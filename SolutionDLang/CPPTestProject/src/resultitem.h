#pragma once
#ifndef __RESULTITEM_H__
#define  __RESULTITEM_H__
//////////////////////////////
#include <cassert>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
///////////////////////////////////
namespace info {
	///////////////////////////////////////////////
	template <typename U = int, typename Z = size_t> class ResultItemEntry {
	public:
		typedef U IndexType;
		typedef Z ResultType;
		//
		typedef ResultItemEntry<IndexType, ResultType> ResultItemEntryType;
	private:
		IndexType _index1;
		IndexType _index2;
		ResultType _res;
	public:
		ResultItemEntry() :_index1(0), _index2(0), _res(0) {}
		ResultItemEntry(const IndexType i1, const IndexType i2, const ResultType r) :
			_index1(i1), _index2(i2), _res(r) {
			assert(this->_index1 < this->_index2);
		}
		ResultItemEntry(const ResultItemEntryType &other) :_index1(other._index1), _index2(other._index2), _res(other._res) {}
		ResultItemEntryType & operator=(const ResultItemEntryType &other) {
			if (this != &other) {
				this->_index1 = other._index1;
				this->_index2 = other._index2;
				this->_res = other._res;
			}
			return (*this);
		}
		virtual ~ResultItemEntry() {}
	public:
		bool operator<(const ResultItemEntryType &other) const {
			if (this->_res > other._res) {
				return(true);
			}
			else if (this->_res < other._res) {
				return (false);
			}
			if (this->_index1 < other._index1) {
				return (true);
			}
			else if (this->_index1 > other._index2) {
				return (false);
			}
			return (this->_index2 < other._index2);
		}// operator<
		bool operator==(const ResultItemEntryType &other) const {
			return ((this->_res == other._res) && (this->_index1 == other._index2) &&
				(this->_index2 == other._index2));
		}// operator<
	public:
		IndexType firstIndex(void) const {
			return (this->_index1);
		}
		IndexType secondIndex(void) const {
			return (this->_index2);
		}
		ResultType value(void) const {
			return (this->_res);
		}
	};// class ResultItemEntry<U,Z>
	////////////////////////////////////
	template <typename U = int, typename Z = size_t> class ResultItem {
	public:
		typedef U IndexType;
		typedef Z ResultType;
		//
		typedef std::map<IndexType, ResultType> IndexResultMapType;
		typedef std::map<IndexType, IndexResultMapType> IndexResultMapTypeMap;
		typedef ResultItemEntry<IndexType, ResultType> ResultItemEntryType;
		typedef std::vector<ResultItemEntryType> ResultItemEntryTypeVector;
		typedef ResultItem<IndexType, ResultType> ResultItemType;
	private:
		IndexResultMapTypeMap _map;
	public:
		ResultItem() {}
		ResultItem(const ResultItemType &other) :_map(other._map) {}
		ResultItemType & operator=(const ResultItemType &other) {
			if (this != &other) {
				this->_map = other._map;
			}
			return (*this);
		}
		virtual ~ResultItem() {}
	public:
		void get(ResultItemEntryTypeVector &oVec, const ResultType limit = 0) const {
			oVec.clear();
			const IndexResultMapTypeMap &oMap = this->_map;
			for (auto it = oMap.begin(); it != oMap.end(); ++it) {
				const IndexType index1 = (*it).first;
				auto m = (*it).second;
				for (auto jt = m.begin(); jt != m.end(); ++jt) {
					const ResultType r = (*jt).second;
					if (r >= limit) {
						const IndexType index2 = (*jt).first;
						ResultItemEntryType x(index1, index2, r);
						oVec.push_back(x);
					}// limit
				}// jt
			}// it
			std::sort(oVec.begin(), oVec.end());
		}// get
		void keys(std::set<IndexType> &oSet) const {
			oSet.clear();
			const IndexResultMapTypeMap &oMap = this->_map;
			for (auto it = oMap.begin(); it != oMap.end(); ++it) {
				oSet.insert((*it).first);
			}// it
		}// keys
		void reset(void) {
			this->_map.clear();
		}
		bool has_value(const IndexType index1, const IndexType index2, ResultType &r) const {
			bool bRet = false;
			IndexType i1 = index1;
			IndexType i2 = index2;
			if (i1 > i2) {
				IndexType t = i1;
				i1 = i2;
				i2 = t;
			}
			const IndexResultMapTypeMap &oMap = this->_map;
			auto it = oMap.find(i1);
			if (it != oMap.end()) {
				auto m = (*it).second;
				if (m.find(i2) != m.end()) {
					r = m[i2];
					bRet = true;
				}
			}
			return(bRet);
		}//has_value
		void add_value(const IndexType i1, const IndexType i2, const ResultType r) {
			IndexType i1 = index1;
			IndexType i2 = index2;
			if (i1 > i2) {
				IndexType t = i1;
				i1 = i2;
				i2 = t;
			}
			IndexResultMapTypeMap &oMap = this->_map;
			if (oMap.find(i1) != oMap ? end()) {
				IndexResultMapType &m = oMap[i1];
				m[i2] = r;
			}
			else {
				IndexResultMapType m;
				m[i2] = r;
				oMap[i1] = m;
			}
		}//add_value
		void increment(const IndexType index1, const IndexType index2) {
			IndexType i1 = index1;
			IndexType i2 = index2;
			if (i1 > i2) {
				IndexType t = i1;
				i1 = i2;
				i2 = t;
			}
			IndexResultMapTypeMap &oMap = this->_map;
			if (oMap.find(i1) != oMap.end()) {
				IndexResultMapType &m = oMap[i1];
				IndexType r = m[i2];
				r = (IndexType)(r + 1);
				m[i2] = r;
			}
			else {
				IndexResultMapType m;
				m[i2] = (ResultType)1;
				oMap[i1] = m;
			}
		}//increment
		void increment(const std::vector<IndexType> &oVec) {
			const size_t n = oVec.size();
			for (size_t i = 0; i < n; ++i) {
				const IndexType index1 = oVec[i];
				for (size_t j = i + 1; j < n; ++j) {
					const IndexType index2 = oVec[j];
					this->increment(index1, index2);
				}// j
			}// i
		}// increment
		void increment(const std::set<IndexType> &oSet) {
			std::vector<IndexType> oVec;
			for (auto it = oSet.begin(); it != oSet.end(); ++it) {
				oVec.push_back(*it);
			}// it
			this->increment(oVec);
		}// increment
	};// class ResultItem<U,Z>
	/////////////////////////////////////
}// namespace info
/////////////////////////////
#endif // !__RESULTITEM_H__
//////////////////////////
//eof: resulitem.h
