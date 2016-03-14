#pragma once
#ifndef __TREEELEM_H__
#define __TREEELEM_H__
////////////////////////////////
#include "indivset.h"
////////////////////////////////////
#include <map>
///////////////////////////////////////
namespace info {
	//////////////////////////////////////
	enum class LinkMode { noLink, linkMin, linkMax, linkMean, linkCenter };
	/////////////////////////////////////
	template <typename T = int, typename U = int, typename Z = long, class S = std::wstring>
	class TreeElem {
	public:
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
		typedef std::map<IndexType, IndexType> IndexTypeMap;
		//
		typedef TreeElem<DataType, IndexType, DistanceType, StringType> TreeElemType;
		typedef std::shared_ptr<TreeElemType> TreeElemTypePtr;
		typedef std::vector<TreeElemTypePtr> TreeElemTypePtrVector;
	private:
		IndexType _index;
		size_t _order;
		LinkMode _link;
		DistanceFuncType *_pfunc;
		StringType _id;
		IndivTypePtr _center;
		TreeElemTypePtrVector _children;
	private:
		static std::shared_ptr<EuclideDistanceFunc<DataType, DistanceType> > _st_func;
	public:
		TreeElem() :_index(0), _order(0), _link(LinkMode::noLink), _pfunc(nullptr) {}
		TreeElem(IndexType aIndex, size_t aOrder,
			IndivTypePtr oCenter = IndivTypePtr(),
			LinkMode aLink = LinkMode::linkCenter,
			DistanceFuncType *pf = nullptr,
			const StringType &sId = StringType()) :_index(aIndex), _order(aOrder), _link(aLink),
			_pfunc(pf), _id(sId), _center(oCenter)
		{
			this->check_distance_func();
		}// TreeElem
		TreeElem(const TreeElemType &other) :_index(other._index), _order(other._order), _link(other._link),
			_pfunc(other._pfunc), _id(other._id), _center(other._center), _children(other._children) {}
		TreeElemType & operator=(const TreeElemType &other) {
			if (this != &other) {
				this->_index = other._index;
				this->_order = other._order;
				this->_link = other._link;
				this->_pfunc = other._pfunc;
				this->_id = other._id;
				this->_center = other._center;
				this->_children = other._children;
			}
			return (*this);
		}
		virtual ~TreeElem() {}
	public:
		DistanceType distance(const TreeElemType &other) const {
			DistanceFuncType *pf = this->_pfunc;
			assert(pf != nullptr);
			const TreeElemTypePtrVector &vv1 = this->_children;
			const TreeElemTypePtrVector &vv2 = other._children;
			DistanceType result = 0;
			const size_t n1 = vv1.size();
			const size_t n2 = vv2.size();
			if ((n1 > 1) && (n2 < 1)) {
				return other.distance(*this);
			}
			const IndivType *pInd1 = this->_center.get();
			const IndivType *pInd2 = other._center.get();
			if ((n1 < 1) && (n2 < 1)) {
				if ((pInd1 != nullptr) && (pInd2 != nullptr)) {
					pInd1->distance(*pInd2, result, pf);
				}
				return (result);
			}
			const LinkMode link = this->_link;
			if (link == LinkMode::linkCenter) {
				if ((pInd1 != nullptr) && (pInd2 != nullptr)) {
					pInd1->distance(*pInd2, result, pf);
				}
				return (result);
			}
			if ((n1 < 1) && (n2 > 0)) {
				if (pInd1 == nullptr) {
					return (result);
				}
				switch (link) {
				case LinkMode::linkMin:
				{
					TreeElemTypePtr t0 = vv2[0];
					const TreeElemType *p0 = t0.get();
					assert(p0 != nullptr);
					result = this->distance(*p0);
					for (size_t i = 1; i < n2; ++i) {
						TreeElemTypePtr t = vv2[i];
						const TreeElemType *p = t.get();
						assert(p != nullptr);
						DistanceType rr = this->distance(*p);
						if (rr < result) {
							result = rr;
						}
					}// i
				}
				break;
				case LinkMode::linkMax: {
					TreeElemTypePtr t0 = vv2[0];
					const TreeElemType *p0 = t0.get();
					assert(p0 != nullptr);
					result = this->distance(*p0);
					for (size_t i = 1; i < n2; ++i) {
						TreeElemTypePtr t = vv2[i];
						const TreeElemType *p = t.get();
						assert(p != nullptr);
						DistanceType rr = this->distance(*p);
						if (rr > result) {
							result = rr;
						}
					}// i
				}
				break;
				case LinkMode::linkMean:
				{
					for (size_t 0 = 1; i < n2; ++i) {
						TreeElemTypePtr t = vv2[i];
						const TreeElemType *p = t.get();
						assert(p != nullptr);
						DistanceType rr = this->distance(*p);
						result = (DistanceType)(result + rr);
					}// i
					result = (DistanceType)(result / n2);
				}
				default:
					break;
				}// link
			}
			else if ((n1 > 1) && (n2 > 1)) {
				switch (link) {
				case LinkMode::linkMin:
				{
					for (size_t i = 0; i < n1; ++i) {
						TreeElemTypePtr t1 = vv1[i];
						const TreeElemType *p1 = t1.get();
						assert(p1 != nullptr);
						for (size_t j = 0; j < n2; ++j) {
							TreeElemTypePtr t2 = vv2[i];
							const TreeElemType *p2 = t2.get();
							assert(p2 != nullptr);
							DistanceType rr = p1->distance(*p2);
							if ((i == 0) && (j == 0)) {
								result = rr;
							}
							else if (rr < result) {
								result = rr;
							}
						}// j
					}// i
				}
				break;
				case LinkMode::linkMax:
				{
					for (size_t i = 0; i < n1; ++i) {
						TreeElemTypePtr t1 = vv1[i];
						const TreeElemType *p1 = t1.get();
						assert(p1 != nullptr);
						for (size_t j = 0; j < n2; ++j) {
							TreeElemTypePtr t2 = vv2[i];
							const TreeElemType *p2 = t2.get();
							assert(p2 != nullptr);
							DistanceType rr = p1->distance(*p2);
							if ((i == 0) && (j == 0)) {
								result = rr;
							}
							else if (rr > result) {
								result = rr;
							}
						}// j
					}// i
				}
				break;
				case LinkMode::linkMean:
				{
					size_t count = 0;
					for (size_t i = 0; i < n1; ++i) {
						TreeElemTypePtr t1 = vv1[i];
						const TreeElemType *p1 = t1.get();
						assert(p1 != nullptr);
						for (size_t j = 0; j < n2; ++j) {
							TreeElemTypePtr t2 = vv2[i];
							const TreeElemType *p2 = t2.get();
							assert(p2 != nullptr);
							DistanceType rr = p1->distance(*p2);
							result = (DistanceType)(result + rr);
							++count;
						}// j
					}// i
					result = (Z)(result / count);
				}
				break;
				default:
					break;
				}// link
			}
			return (result);
		}// distance
	private:
		void update_center(void) {
			const TreeElemTypePtrVector &vv = this->_children;
			const size_t n = vv.size();
			if (n < 1) {
				return;
			}
			size_t nCols = 0;
			for (size_t i = 0; i < n; ++i) {
				TreeElemTypePtr t = vv[i];
				const TreeElemType *p = t.get();
				assert(p != nullptr);
				IndivTypePtr c = p->_center;
				IndivType *pInd = c.get();
				assert(pInd != nullptr);
				const DataTypeArray &d = pInd->value();
				if (d.size() > nCols) {
					nCols = d.size();
				}
			}// i
			assert(nCols > 0);
			std::valarray<double> sum(nCols);
			for (size_t i = 0; i < n; ++i) {
				TreeElemTypePtr t = vv[i];
				const TreeElemType *p = t.get();
				assert(p != nullptr);
				IndivTypePtr c = p->_center;
				IndivType *pInd = c.get();
				assert(pInd != nullptr);
				const DataTypeArray &d = pInd->value();
				assert(d.size() == nCols);
				if (i == 0) {
					for (size_t j = 0; j < nCols; ++j) {
						sum[j] = (double)d[j];
					}
				}
				else {
					for (size_t j = 0; j < nCols; ++j) {
						sum[j] = sum[j] + (double)d[j];
					}// j
				}
			}// i
			DataTypeArray val(nCols);
			for (size_t i = 0; i < nCols; ++i) {
				val[i] = (DataType)(sum[i] / nCols);
			}
			IndivTypePtr oCenter = this->_center;
			IndivType *pCenter = oCenter.get();
			if (pCenter == nullptr) {
				this->_center = std::make_shared<IndivType>();
				pCenter = this->_center.get();
				assert(pCenter != nullptr);
			}
			pCenter->value(val);
		}// update_center
		bool check_distance_func() {
			if (this->_pfunc != nullptr) {
				return (true);
			}
			DistanceFuncType *p = _st_func.get();
			if (p != nullptr) {
				this->_pfunc = p;
				return (true);
			}
			_st_func.reset(new EuclideDistanceFunc<DataType, DistanceType>());
			p = _st_func.get();
			if (p != nullptr) {
				this->_pfunc = p;
				return (true);
			}
			return (false);
		}//check_distance_func
	};// class TreeElem<T,U,Z,S>
	/////////////////////////////////////////
	template <typename T, typename U, typename Z, class S>
	std::shared_ptr<EuclideDistanceFunc<T, Z> > TreeElem<T, U, Z, S>::_st_func;
	//////////////////////////////////////////

	/////////////////////////////////////////////
}// namespace info
/////////////////////////////////
#endif // !__TREEELEM_H__
/////////////////////
//eof: treeelem.h
