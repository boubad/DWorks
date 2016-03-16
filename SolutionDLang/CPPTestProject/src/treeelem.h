#pragma once
#ifndef __TREEELEM_H__
#define __TREEELEM_H__
////////////////////////////////
#include "indivset.h"
#include "matdata.h"
////////////////////////////////////
#include <map>
#include <set>
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
		DistanceType _dist;
		DistanceFuncType *_pfunc;
		StringType _id;
		IndivTypePtr _center;
		TreeElemTypePtrVector _children;
	private:
		static std::shared_ptr<EuclideDistanceFunc<DataType, DistanceType> > _st_func;
	public:
		TreeElem() :_index(0), _order(0), _link(LinkMode::noLink),_dist(0), _pfunc(nullptr) {
		}
		TreeElem(IndexType aIndex, size_t aOrder,
			IndivTypePtr oCenter = IndivTypePtr(),
			LinkMode aLink = LinkMode::linkCenter,
			DistanceType dist = 0,
			DistanceFuncType *pf = nullptr,
			const StringType &sId = StringType()) :_index(aIndex), _order(aOrder), _link(aLink),_dist(dist),
			_pfunc(pf), _id(sId), _center(oCenter)
		{
			this->check_distance_func();
		}// TreeElem
		TreeElem(const TreeElemType &other) :_index(other._index), _order(other._order), _link(other._link),
			_dist(other._dist),
			_pfunc(other._pfunc), _id(other._id), _center(other._center), _children(other._children) {}
		TreeElemType & operator=(const TreeElemType &other) {
			if (this != &other) {
				this->_index = other._index;
				this->_order = other._order;
				this->_link = other._link;
				this->_dist = other._dist;
				this->_pfunc = other._pfunc;
				this->_id = other._id;
				this->_center = other._center;
				this->_children = other._children;
			}
			return (*this);
		}
		virtual ~TreeElem() {}
	public:
		std::wostream & write_to(std::wostream &os) const {
			os << L"{" << std::endl;
			os << L"\tindex: " << this->index() << std::endl;
			os << L"\tlink order: " << this->order() << std::endl;
			os << L"\tlink criteria: " << this->linkDistance() << std::endl;
			if (this->_center.get() != nullptr) {
				const IndivType *p = this->_center.get();
				assert(p != nullptr);
				os << L"\tcenter: " << *p << std::endl;
			}
			const TreeElemTypePtrVector &vv = this->children();
			const size_t n = vv.size();
			if (n > 0) {
				os << L"\tchildren: "<< n << L"\t{" <<std::endl;
				for (size_t i = 0; i < n; ++i) {
					const TreeElemTypePtr &c = vv[i];
					const TreeElemType *p = c.get();
					assert(p != nullptr);
					os << L"\t\t";
					p->write_to(os);
				}// i
				os << L"\t}" << std::endl;
			}// n
			os << L"}" << std::endl;
			return os;
		}// write_to
		std::ostream & write_to(std::ostream &os) const {
			os << "{" << std::endl;
			os << "\tindex: " << this->index() << std::endl;
			os << "\tlink order: " << this->order() << std::endl;
			os << "\tlink criteria: " << this->linkDistance() << std::endl;
			if (this->_center.get() != nullptr) {
				const IndivType *p = this->_center.get();
				assert(p != null);
				os << "\tcenter: " << *p << std::endl;
			}
			const TreeElemTypePtrVector &vv = this->children();
			const size_t n = vv.size();
			if (n > 0) {
				os << "\tchildren: " << n << "\t{" << std::endl;
				for (size_t i = 0; i < n; ++i) {
					const TreeElemTypePtr &c = vv[i];
					const TreeElemType *p = c.get();
					assert(p != nullptr);
					os << "\t\t";
					p->write_to(os);
				}// i
				os << "\t}" << std::endl;
			}// n
			os << "}" << std::endl;
			return os;
		}// write_to
	public:
		DistanceType linkDistance(void) const {
			return (this->_dist);
		}
		void linkDistance(DistanceType d) {
			this->_dist = d;
		}
		const TreeElemTypePtrVector &children(void) const {
			return (this->_children);
		}
		void children(const TreeElemTypePtrVector &vv) {
			this->_children = vv;
		}
		bool is_leaf(void) const {
			return (this->_children.empty());
		}
		const StringType &id(void) const {
			return (this->_id);
		}
		void id(const StringType &s) {
			this->_id = s;
		}
		IndexType index(void) const {
			return (this->_index);
		}
		size_t order(void) const {
			return (this->_order);
		}
		const IndivTypePtr &center(void) const {
			return (this->_center);
		}
		IndivTypePtr &center(void) {
			return (this->_center);
		}
		void center(const  IndivTypePtr &c) {
			this->_center = c;
		}
		DistanceType distance(const TreeElemType &other) const {
			DistanceFuncType *pf = this->_pfunc;
			assert(pf != nullptr);
			DistanceType result = 0;
			const IndivType *pInd1 = this->_center.get();
			if (pInd1 == nullptr) {
				return (result);
			}
			const TreeElemTypePtrVector &vv1 = this->_children;
			const TreeElemTypePtrVector &vv2 = other._children;
			const size_t n1 = vv1.size();
			const size_t n2 = vv2.size();
			if ((n1 > 1) && (n2 < 1)) {
				return other.distance(*this);
			}
			if ((n1 < 1) && (n2 < 1)) {
				const IndivType *pInd2 = other._center.get();
				if ((pInd1 != nullptr) && (pInd2 != nullptr)) {
					pInd1->distance(*pInd2, result, pf);
				}
				return (result);
			}
			const LinkMode link = this->_link;
			if (link == LinkMode::linkCenter) {
				const IndivType *pInd2 = other._center.get();
				if ((pInd1 != nullptr) && (pInd2 != nullptr)) {
					pInd1->distance(*pInd2, result, pf);
				}
				return (result);
			}
			if ((n1 < 1) && (n2 > 0)) {
				switch (link) {
				case LinkMode::linkMin:
				{
					const TreeElemTypePtr &t0 = vv2[0];
					const TreeElemType *p0 = t0.get();
					assert(p0 != nullptr);
					result = this->distance(*p0);
					for (size_t i = 1; i < n2; ++i) {
						const TreeElemTypePtr &t = vv2[i];
						const TreeElemType *p = t.get();
						assert(p != nullptr);
						const IndivTypePtr &c = p->_center;
						const IndivType *pInd = c.get();
						assert(pInd != nullptr);
						DistanceType rr = 0;
						pInd1->distance(*pInd, rr, pf);
						if (rr < result) {
							result = rr;
						}
					}// i
				}
				break;
				case LinkMode::linkMax: {
					const TreeElemTypePtr &t0 = vv2[0];
					const TreeElemType *p0 = t0.get();
					assert(p0 != nullptr);
					result = this->distance(*p0);
					for (size_t i = 1; i < n2; ++i) {
						const TreeElemTypePtr &t = vv2[i];
						const TreeElemType *p = t.get();
						assert(p != nullptr);
						const IndivTypePtr &c = p->_center;
						const IndivType *pInd = c.get();
						assert(pInd != nullptr);
						DistanceType rr = 0;
						pInd1->distance(*pInd, rr, pf);
						if (rr > result) {
							result = rr;
						}
					}// i
				}
										break;
				case LinkMode::linkMean:
				{
					for (size_t i = 1; i < n2; ++i) {
						const TreeElemTypePtr &t = vv2[i];
						const TreeElemType *p = t.get();
						assert(p != nullptr);
						const IndivTypePtr &c = p->_center;
						const IndivType *pInd = c.get();
						assert(pInd != nullptr);
						DistanceType rr = 0;
						pInd1->distance(*pInd, rr, pf);
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
						const TreeElemTypePtr &t1 = vv1[i];
						const TreeElemType *p1 = t1.get();
						assert(p1 != nullptr);
						for (size_t j = 0; j < n2; ++j) {
							const TreeElemTypePtr &t2 = vv2[j];
							const TreeElemType *p2 = t2.get();
							assert(p2 != nullptr);
							const IndivTypePtr &c = p2->_center;
							const IndivType *pInd = c.get();
							assert(pInd != nullptr);
							DistanceType rr = 0;
							pInd1->distance(*pInd, rr, pf);
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
						const TreeElemTypePtr &t1 = vv1[i];
						const TreeElemType *p1 = t1.get();
						assert(p1 != nullptr);
						for (size_t j = 0; j < n2; ++j) {
							const TreeElemTypePtr &t2 = vv2[j];
							const TreeElemType *p2 = t2.get();
							assert(p2 != nullptr);
							const IndivTypePtr &c = p2->_center;
							const IndivType *pInd = c.get();
							assert(pInd != nullptr);
							DistanceType rr = 0;
							pInd1->distance(*pInd, rr, pf);
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
						const TreeElemTypePtr &t1 = vv1[i];
						const TreeElemType *p1 = t1.get();
						assert(p1 != nullptr);
						for (size_t j = 0; j < n2; ++j) {
							const TreeElemTypePtr &t2 = vv2[j];
							const TreeElemType *p2 = t2.get();
							assert(p2 != nullptr);
							const IndivTypePtr &c = p2->_center;
							const IndivType *pInd = c.get();
							assert(pInd != nullptr);
							DistanceType rr = 0;
							pInd1->distance(*pInd, rr, pf);
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
		void update_center(void) {
			const TreeElemTypePtrVector &vv = const_cast<TreeElemTypePtrVector &>(this->_children);
			const size_t n = vv.size();
			if (n < 1) {
				return;
			}
			size_t nCols = 0;
			for (size_t i = 0; i < n; ++i) {
				const TreeElemTypePtr &t = vv[i];
				const TreeElemType *p = t.get();
				assert(p != nullptr);
				const IndivTypePtr &c = p->_center;
				const IndivType *pInd = c.get();
				assert(pInd != nullptr);
				const DataTypeArray &d = pInd->value();
				if (d.size() > nCols) {
					nCols = d.size();
				}
			}// i
			assert(nCols > 0);
			std::valarray<double> sum(nCols);
			for (size_t i = 0; i < n; ++i) {
				const TreeElemTypePtr &t = vv[i];
				const TreeElemType *p = t.get();
				assert(p != nullptr);
				const IndivTypePtr &c = p->_center;
				const IndivType *pInd = c.get();
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
				this->_center = std::make_shared<IndivType>(this->index());
				pCenter = this->_center.get();
				assert(pCenter != nullptr);
			}
			pCenter->value(val);
		}// update_center
		void get_indivs(IndivTypePtrVector &oVec) const {
			const TreeElemTypePtrVector &vv = this->_children;
			const size_t n = vv.size();
			if (n < 1) {
				if (this->_center.get() != nullptr) {
					oVec.push_back(this->_center);
				}
				return;
			}
			for (size_t i = 0; i < n; ++i) {
				const TreeElemTypePtr &c = vv[i];
				const TreeElemType *p = c.get();
				if (p != nullptr) {
					p->get_indivs(oVec);
				}
			}// i
		}//get_indivs
	private:
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
	template <typename T = int, typename U = int, typename Z = long, class S = std::wstring>
	class Tree {
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
		typedef Tree<DataType, IndexType, DistanceType, StringType> TreeType;

	private:
		LinkMode _link;
		DistanceFuncType *_pfunc;
		TreeElemTypePtrVector _elems;
		std::vector<DistanceType> _distances;
	public:
		Tree() :_link(LinkMode::noLink), _pfunc(nullptr) {}
		Tree(const MatDataType *pMat,
			const LinkMode m = LinkMode::linkMean,
			const DataMode dmode = DataMode::modeRow,
			DistanceFuncType *pFunc = nullptr) :_link(LinkMode::noLink), _pfunc(nullptr) {
			this->initialize(pMat, m, dmode, pFunc);
		}
		Tree(const TreeType &other) :_link(other._link), _pfunc(other._pfunc), _elems(other._elems),
		_distances(other._distances){}
		TreeType & operator=(const TreeType &other) {
			if (this != &other) {
				this->_link = other._link;
				this->_pfunc = other._pfunc;
				this->_elems = other._elems;
				this->_distances = other._distances;
			}
			return (*this);
		}
		virtual ~Tree() {}
	public:
		std::ostream & write_to(std::ostream &os) const {
			os << "{" << std::endl;
			const size_t n = this->_elems.size();
			os << "\tsize: " << n << std::endl;
			for (size_t i = 0; i < n; ++i) {
				const TreeElemType *p = ((this->_elems)[i]).get();
				assert(p != nullptr);
				os << *p << std::endl;
			}// i
			os << "}" << std::endl;
			return os;
		}// write_to
		std::wostream & write_to(std::wostream &os) const {
			os << L"{" << std::endl;
			const size_t n = this->_elems.size();
			os << L"\tsize: " << n << std::endl;
			for (size_t i = 0; i < n; ++i) {
				const TreeElemType *p = ((this->_elems)[i]).get();
				assert(p != nullptr);
				os << *p << std::endl;
			}// i
			os << L"}" << std::endl;
			return os;
		}// write_to
	public:
		const size_t size(void) const {
			return (this->_elems.size());
		}
		const TreeElemType *element_at(const size_t i) const {
			assert(i < this->_elems.size());
			const TreeElemTypePtr &c = this->_elems[i];
			const TreeElemType *p = c.get();
			assert(p != nullptr);
			return (p);
		}// element_at
		void initialize(const MatDataType *pMat,
			const LinkMode m = LinkMode::linkMean,
			const DataMode dmode = DataMode::modeRow,
			DistanceFuncType *pFunc = nullptr) {
			assert(pMat != nullptr);
			assert(dmode != DataMode::noMode);
			this->_link = m;
			this->_pfunc = pFunc;
			TreeElemTypePtrVector &vv = this->_elems;
			const size_t n = pMat->get_data_count(dmode);
			assert(n > 0);
			vv.resize(n);
			const size_t order = 0;
			const DistanceType dist = 0;
			for (size_t i = 0; i < n; ++i) {
				DataTypeArray v;
				pMat->data_at(i, dmode, v);
				const IndexType aIndex = (IndexType)i;
				IndivTypePtr oCenter = std::make_shared<IndivType>(aIndex, v);
				assert(oCenter.get() != nullptr);
				TreeElemTypePtr t = std::make_shared<TreeElemType>(aIndex, order, oCenter, m,dist, pFunc);
				assert(t.get() != nullptr);
				vv[i] = t;
			}// i
		}// initialize
		void aggregate(const size_t nClasses = 1) {
			assert(nClasses > 0);
			this->compute_distances();
			size_t nOrder = 0;
			while (this->_elems.size() > nClasses) {
				if (!this->aggreg_one_step(nOrder)) {
					break;
				}
			}// while size
		}// aggregate
	private:
		bool aggreg_one_step(size_t &nOrder) {
			typedef std::set<size_t> MySet;
			std::map<size_t, MySet > oMap;
			DistanceType dMin = 0;
			const TreeElemTypePtrVector &vv = const_cast<TreeElemTypePtrVector &>(this->_elems);
			const size_t n = vv.size();
			for (size_t i = 0; i < n; ++i) {
				for (size_t j = i + 1; j < n; ++j) {
					DistanceType d = this->elems_distances(i, j);
					if ((i == 0) && (j == 1)) {
						dMin = d;
						MySet o;
						o.insert(j);
						oMap[i] = o;
					}
					else if (d < dMin) {
						oMap.clear();
						dMin = d;
						MySet o;
						o.insert(j);
						oMap[i] = o;
					}
					else if (d == dMin) {
						if (oMap.find(i) != oMap.end()) {
							(oMap[i]).insert(j);
						}
						else {
							MySet o;
							o.insert(j);
							oMap[i] = o;
						}
					}
				}// j
			}// i
			if (oMap.empty()) {
				return (false);
			}
			std::set<size_t> zSet;
			std::map<size_t, TreeElemTypePtrVector> chMap;
			for (auto it = oMap.begin(); it != oMap.end(); ++it) {
				TreeElemTypePtrVector vx;
				size_t i = (*it).first;
				TreeElemTypePtr ti = vv[i];
				TreeElemType *pi = ti.get();
				assert(pi != nullptr);
				vx.push_back(ti);
				zSet.insert(pi->index());
				std::set<size_t> &xSet = (*it).second;
				for (auto jt = xSet.begin(); jt != xSet.end(); ++jt) {
					size_t j = *jt;
					TreeElemTypePtr tj = vv[j];
					TreeElemType *pj = tj.get();
					assert(pj != nullptr);
					vx.push_back(tj);
					zSet.insert(pj->index());;
				}// jt
				chMap[i] = vx;
			}// *it
			TreeElemTypePtrVector vres;
			IndexType indexMax = 0;
			for (size_t i = 0; i < n; ++i) {
				TreeElemTypePtr t = vv[i];
				TreeElemType *p = t.get();
				assert(p != nullptr);
				IndexType aIndex = p->index();
				if (i == 0) {
					indexMax = aIndex;
				}
				else if (aIndex > indexMax) {
					indexMax = aIndex;
				}
				if (zSet.find(aIndex) == zSet.end()) {
					vres.push_back(t);
				}
			}// i
			++nOrder;
			for (auto it = chMap.begin(); it != chMap.end(); ++it) {
				TreeElemTypePtrVector vr = (*it).second;
				IndivTypePtr oCenter;
				TreeElemTypePtr t = std::make_shared<TreeElemType>(++indexMax, nOrder, oCenter, this->_link, dMin, this->_pfunc);
				TreeElemType *p = t.get();
				assert(p != nullptr);
				p->children(vr);
				p->update_center();
				vres.push_back(t);
			}// it
			this->_elems = vres;
			this->compute_distances();
			return (true);
		}// aggreg_one_step
		DistanceType elems_distances(const size_t i1, const size_t i2) const {
			const TreeElemTypePtrVector &vv = const_cast<TreeElemTypePtrVector &>(this->_elems);
			const size_t n = vv.size();
			assert(i1 < n);
			assert(i2 < n);
			if (i1 == i2) {
				return (0);
			}
			const size_t pos = (size_t)(i1 * n + i2);
			assert(pos < this->_distances.size());
			return ((this->_distances)[pos]);
		}//elems_distance
		void compute_distances(void) {
			const TreeElemTypePtrVector &vv = const_cast<TreeElemTypePtrVector &>(this->_elems);
			const size_t n = vv.size();
			std::vector<DistanceType> &vec = this->_distances;
			if (n > 1) {
				vec.resize(n * n);
				for (size_t i = 0; i < n; ++i) {
					const TreeElemTypePtr &t1 = vv[i];
					const TreeElemType *p1 = t1.get();
					assert(p1 != nullptr);
					vec[i * n + i] = (DistanceType)0;
					for (size_t j = i + 1; j < n; ++j) {
						const TreeElemTypePtr &t2 = vv[j];
						const TreeElemType *p2 = t2.get();
						assert(p2 != nullptr);
						DistanceType d = p1->distance(*p2);
						vec[i * n + j] = d;
						vec[j * n + i] = d;
					}// j
				}// i
			}
			else {
				vec.clear();
			}
		}// compute_distances
	};// class Tree<T,U,Z,S>
	/////////////////////////////////////////////
}// namespace info
//////////////////////////////////////
template <typename T, typename U, typename Z, class S>
std::wostream & operator<<(std::wostream &os, const info::TreeElem<T, U, Z, S> &d) {
	return d.write_to(os);
}
template <typename T, typename U, typename Z, class S>
std::ostream & operator<<(std::ostream &os, const info::TreeElem<T, U, Z, S> &d) {
	return d.write_to(os);
}
////////////////////////////////////
template <typename T, typename U, typename Z, class S>
std::wostream & operator<<(std::wostream &os, const info::Tree<T, U, Z, S> &d) {
	return d.write_to(os);
}
template <typename T, typename U, typename Z, class S>
std::ostream & operator<<(std::ostream &os, const info::Tree<T, U, Z, S> &d) {
	return d.write_to(os);
}
/////////////////////////////////

/////////////////////////////////
#endif // !__TREEELEM_H__
/////////////////////
//eof: treeelem.h
