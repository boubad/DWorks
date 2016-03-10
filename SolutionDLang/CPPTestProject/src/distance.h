#pragma once
#ifndef __DISTANCE_H__
#define __DISTANCE_H__
/////////////////////////
#include <cassert>
#include <valarray>
/////////////////////////////
namespace info {
	//////////////////////////////////////
	template <typename T = int, typename Z = int> class DistanceFunc {
	public:
		DistanceFunc() {}
		DistanceFunc(const DistanceFunc<T> &) {}
		DistanceFunc<T> & operator=(const DistanceFunc<T> &) { return (*this); }
		virtual ~DistanceFunc() {}
	public:
		Z operator()(const std::valarray<T> &v1, const std::valarray<T> &v2) const {
			return this->perform_compute(v1, v2);
		}// operator()
	protected:
		virtual Z perform_compute(const std::valarray<T> & v1, const std::valarray<T> & v2) const {
			const size_t n = v1.size();
			assert(v2.size() == n);
			Z res = 0;
			for (size_t i = 0; i < n; ++i) {
				res += this->compute_one_step(v1[i], v2[i], i);
			}// i
			return res;
		}
		virtual Z compute_one_step(const T /*v1*/, const T /*v2*/, const size_t /*pos = 0*/) const {
			return 0;
		}
	};// class DistanceFunc
	////////////////////////////////////////
	template <typename T = int, typename Z = double, typename W = double> class WeightedDistanceFunc :
		public DistanceFunc<T, Z> {
	private:
		const DistanceFunc<T, Z> &_func;
		const std::valarray<W> &_weights;
	public:
		WeightedDistanceFunc(const DistanceFunc<T, Z> &func, const std::valarray<W> &ww) : DistanceFunc<T, Z>(other),
			_func(func), _weights(ww) {}
		WeightedDistanceFunc(const WeightedDistanceFunc<T, Z, W> &other) :_func(other._func), _weights(other._weights) {}
		WeightedDistanceFunc<T, Z, W> & operator=(const WeightedDistanceFunc<T, Z, W> &other) {
			if (this != &other) {
				DistanceFunc<T, Z>::operator=(other);
				this->_func = other._func;
				this->_weights = other._weights;
			}
			return (*this);
		}
		virtual ~WeightedDistanceFunc() {}
	protected:
		virtual Z perform_compute(const std::valarray<T> &v1, const std::valarray<T> &v2) const {
			const size_t n = v1.size();
			assert(v2.size() == n);
			assert(_weights.size() == n);
			Z res = 0;
			const std::valarray<W> &ww = this->_weights;
			for (size_t i = 0; i < n; ++i) {
				const W w = ww[i];
				assert(w >= 0);
				res += = (Z)(w * this->_func.compute_one_step(v1[i], v2[i], pos));
			}// i
			return res;
		}// perform_compute
		virtual Z compute_one_step(const T v1, const T v2, const size_t pos = 0) const {
			assert(pos < this->_weights.size());
			const W w = this->_weights[pos];
			assert(w >= 0);
			return (Z)(this->_weights[pos] * this->_func.compute_one_step(v1, v2, pos));
		}
	}; // class WeightedDistanceFunc<T,Z,W>
	////////////////////////////////////////
	template <typename T = int, typename Z = double> class ManhattanDistanceFunc : public DistanceFunc<T, Z> {
	public:
		ManhattanDistanceFunc() {}
		ManhattanDistanceFunc(const ManhattanDistanceFunc<T, T> &other) :DistanceFunc<T, Z>(other) {
		}
		ManhattanDistanceFunc<T, Z> & operator=(const ManhattanDistanceFunc<T, Z> &other) {
			if (this != &other) {
				DistanceFunc<T, Z>::operator=(other);
			}
			return (*this);
		}
		virtual ~ManhattanDistanceFunc() {}
	protected:
		virtual Z perform_compute(const std::valarray<T> &v1, const std::valarray<T> &v2) const {
			const size_t n = v1.size();
			assert(v2.size() == n);
			std::valarray<T> t = v1 - v2;
			std::valarray<T> t1 = abs(t);
			return (Z)t1.sum();
		}// perform_compute
		virtual Z compute_one_step(const T v1, const T v2, const size_t /*pos = 0*/) const {
			return (Z)((v1 > v2) ? v1 - v2 : v2 - v1);
		}
	}; // class ManhattanDistanceFunc
	///////////////////////////////////////////
	template <typename T = int, typename Z = double> class EuclideDistanceFunc : public DistanceFunc<T, Z> {
	public:
		EuclideDistanceFunc() {}
		EuclideDistanceFunc(const EuclideDistanceFunc<T, T> &other) :DistanceFunc<T, Z>(other) {
		}
		EuclideDistanceFunc<T, Z> & operator=(const EuclideDistanceFunc<T, Z> &other) {
			if (this != &other) {
				DistanceFunc<T, Z>::operator=(other);
			}
			return (*this);
		}
		virtual ~EuclideDistanceFunc() {}
	protected:
		virtual Z perform_compute(const std::valarray<T> &v1, const std::valarray<T> &v2) const {
			const size_t n = v1.size();
			assert(v2.size() == n);
			std::valarray<T> t = v1 - v2;
			std::valarray<T> t1 = t * t;
			return (Z)t1.sum();
		}// perform_compute
		virtual Z compute_one_step(const T v1, const T v2, const size_t /*pos = 0*/) const {
			Z t = (Z)(v2 - v1);
			return (Z)(t * t);
		}
	}; // class EuclideDistanceFunc
	////////////////////////////////////////
}// namespace info
//////////////////////////
#endif // __DISTANCE_H__
