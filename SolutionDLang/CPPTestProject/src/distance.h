#pragma once
#ifndef __DISTANCE_H__
#define __DISTANCE_H__
/////////////////////////
#include <cassert>
#include <cmath>
#include <valarray>
#include <vector>
/////////////////////////////
namespace info {
	//////////////////////////////////////
	template <typename T = int, typename Z = long> class DistanceFunc {
	public:
		typedef T DataType;
		typedef Z DistanceType;
		//
		typedef DistanceFunc<DataType, DistanceType> DistanceFuncType;
	public:
		DistanceFunc() {}
		DistanceFunc(const DistanceFuncType &) {}
		DistanceFuncType & operator=(const DistanceFuncType &) { return (*this); }
		virtual ~DistanceFunc() {}
	public:
		virtual bool is_valid(void) const {
			return (true);
		}
		DistanceType operator()(const std::valarray<DataType> &v1, const std::valarray<DataType> &v2) const {
			return (this->perform_compute(v1, v2));
		}// operator()
		DistanceType operator()(const std::vector<DataType> &v1, const std::vector<DataType> &v2) const {
			const size_t n = v1.size();
			assert(v2.size() == n);
			std::valarray<DataType> vv1(n);
			std::valarray<DataType> vv2(n);
			for (size_t i = 0; i < n; ++i) {
				vv1[i] = v1[i];
				vv2[i] = v2[i];
			}
			return (this->perform_compute(vv1, vv2));
		}// operator()
		DistanceType operator()(const std::vector<DataType> &v1, const std::valarray<DataType> &v2) const {
			const size_t n = v1.size();
			assert(v2.size() == n);
			std::valarray<DataType> vv1(n);
			for (size_t i = 0; i < n; ++i) {
				vv1[i] = v1[i];
			}
			return (this->perform_compute(vv1, v2));
		}// operator()
		DistanceType operator()(const std::valarray<DataType> &v1, const std::vector<DataType> &v2) const {
			const size_t n = v1.size();
			assert(v2.size() == n);
			std::valarray<DataType> vv2(n);
			for (size_t i = 0; i < n; ++i) {
				vv2[i] = v2[i];
			}
			return (this->perform_compute(v1, vv2));
		}// operator()
	public:
		virtual DistanceType perform_compute(const std::valarray<DataType> & v1, const std::valarray<DataType> & v2) const {
			const size_t n = v1.size();
			assert(v2.size() == n);
			DistanceType res = 0;
			for (size_t i = 0; i < n; ++i) {
				res += this->compute_one_step(v1[i], v2[i], i);
			}// i
			return res;
		}// perform_compute
		virtual DistanceType compute_one_step(const DataType v1, const DataType v2, const size_t /*pos = 0*/) const {
			DistanceType r = (DistanceType)(v1 - v2);
			return ((DistanceType)(r * r));
		}//compute_one_step
	};// class DistanceFunc
	////////////////////////////////////////
	template <typename T = int, typename Z = long, typename W = double> class WeightedDistanceFunc :
		public DistanceFunc<T, Z> {
	public:
		typedef T DataType;
		typedef Z DistanceType;
		typedef W WeightType;
		//
		typedef DistanceFunc<DataType, DistanceType> DistanceFuncType;
		typedef WeightedDistanceFunc<DataType, DistanceType, WeightType> WeightedDistanceFuncType;
	private:
		const DistanceFuncType &_func;
		std::valarray<WeightType> _weights;
	public:
		WeightedDistanceFunc() {}
		WeightedDistanceFunc(const DistanceFuncType &func) :
			_func(func) {}
		WeightedDistanceFunc(const DistanceFuncType &func, const std::valarray<WeightType> &ww) :
			_func(func), _weights(ww) {}
		WeightedDistanceFunc(const WeightedDistanceFuncType &other) : DistanceFuncType(other), _func(other._func), _weights(other._weights) {}
		WeightedDistanceFuncType & operator=(const WeightedDistanceFuncType &other) {
			if (this != &other) {
				DistanceFuncType::operator=(other);
				this->_func = other._func;
				this->_weights = other._weights;
			}
			return (*this);
		}
		virtual ~WeightedDistanceFunc() {}
	public:
		virtual bool is_valid(void) const {
			return ((this->_func.is_valid()) && (this->_weights.size() > 0));
		}// is_valid
		virtual DistanceType compute_one_step(const DataType v1, const DataType v2, const size_t pos = 0) const {
			assert(pos < this->_weights.size());
			const WeightType w = this->_weights[pos];
			assert(w >= 0);
			return (DistanceType)(this->_weights[pos] * this->_func.compute_one_step(v1, v2, pos));
		}
	}; // class WeightedDistanceFunc<T,Z,W>
	////////////////////////////////////////
	template <typename T = int, typename Z = long> class ManhattanDistanceFunc : public DistanceFunc<T, Z> {
	public:
		typedef T DataType;
		typedef Z DistanceType;
		//
		typedef DistanceFunc<DataType, DistanceType> DistanceFuncType;
		typedef ManhattanDistanceFunc<DataType, DistanceType> ManhattanDistanceFuncType;
	public:
		ManhattanDistanceFunc() {}
		ManhattanDistanceFunc(const ManhattanDistanceFuncType &other) :DistanceFuncType(other) {
		}
		ManhattanDistanceFuncType & operator=(const ManhattanDistanceFuncType &other) {
			if (this != &other) {
				DistanceFuncType::operator=(other);
			}
			return (*this);
		}
		virtual ~ManhattanDistanceFunc() {}
		virtual DistanceType perform_compute(const std::valarray<DataType> &v1, const std::valarray<DataType> &v2) const {
			const size_t n = v1.size();
			assert(v2.size() == n);
			DistanceType res = 0;
			for (size_t i = 0; i < n; ++i) {
				DistanceType r = (DistanceType)(v1[i] - v2[i]);
				DistanceType rr = (r < 0) ? -r : r;
				res = (DistanceType)(res + rr);
			}// i
			return (res);
		}// perform_compute
		virtual DistanceType compute_one_step(const DataType v1, const DataType v2, const size_t /*pos = 0*/) const {
			return (DistanceType)((v1 > v2) ? v1 - v2 : v2 - v1);
		}
	}; // class ManhattanDistanceFunc
	///////////////////////////////////////////
	template <typename T = int, typename Z = long> class EuclideDistanceFunc : public DistanceFunc<T, Z> {
	public:
		typedef T DataType;
		typedef Z DistanceType;
		//
		typedef DistanceFunc<DataType, DistanceType> DistanceFuncType;
		typedef EuclideDistanceFunc<DataType, DistanceFunc> EuclideDistanceFuncType;
	public:
		EuclideDistanceFunc() {}
		EuclideDistanceFunc(const EuclideDistanceFuncType &other) :DistanceFuncType(other) {
		}
		EuclideDistanceFuncType & operator=(const EuclideDistanceFuncType &other) {
			if (this != &other) {
				DistanceFuncType::operator=(other);
			}
			return (*this);
		}
		virtual ~EuclideDistanceFunc() {}
		virtual DistanceType perform_compute(const std::valarray<DataType> &v1, const std::valarray<DataType> &v2) const {
			const size_t n = v1.size();
			assert(v2.size() == n);
			assert(n > 0);
			double somme = 0;
			for (size_t i = 0; i < n; ++i) {
				double x = (double)(v1[i] - v2[i]);
				somme += x * x;
			}// i
			if (n > 1) {
				somme /= (double)n;
			}
			return ((DistanceType)std::sqrt(somme));
		}// perform_compute
		virtual DistanceType compute_one_step(const DataType v1, const DataType v2, const size_t /*pos = 0*/) const {
			DistanceType t = (DistanceType)(v2 - v1);
			return (DistanceType)(t * t);
		}
	}; // class EuclideDistanceFunc
	////////////////////////////////////////
}// namespace info
//////////////////////////
#endif // __DISTANCE_H__
