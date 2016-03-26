/*
* eigen3_impl.cpp
*
*  Created on: 18 oct. 2012
*      Author: boubad
*/
/////////////////////////////////////////////////////////
#if defined (__INTEL_COMPILER)
#pragma warning (disable:2196)
#endif
/////////////////////////////
#include <Eigen/Eigenvalues>
///////////////////////////////////////////
#include "eigen.h"
////////////////////////////////
#include <cassert>
#include <vector>
#include <algorithm>
//////////////////////////////////////////////
namespace info {
	//////////////////////////////////////////
	using namespace Eigen;
	////////////////////////////////////////////////
	typedef std::pair<size_t, double> MyPair;
	typedef Matrix<double, Dynamic, Dynamic> MyMatrix;
	//
	struct MyComparePairDescFunc : public std::binary_function<MyPair, MyPair, bool> {
		bool operator()(const MyPair &v1, const MyPair &v2) const {
			return (v1.second > v2.second);
		} // operator()
	};
	// MyComparePairFunc
	///////////////////////////////////////////////////
	extern bool compute_eigen_impl(const size_t nr, const double *pData,
		double *pVals, double *pVecs, size_t *pNbFacts /*= nullptr */) {
		//
		assert(nr > 0);
		assert(pData != nullptr);
		assert(pVals != nullptr);
		assert(pVecs != nullptr);
		//
		const int n = (int)nr;
		MyMatrix m(n, n);
		for (int i = 0; i < n; ++i) {
			for (int j = 0; j <= i; ++j) {
				double v = pData[n * i + j];
				m(i, j) = v;
				if (j != i) {
					m(j, i) = v;
				}
			}	// j
		}	// i
		EigenSolver<MyMatrix> solver(m);
		typename EigenSolver<MyMatrix>::EigenvalueType val = solver.eigenvalues();
		int nx = (int)val.rows();
		int ny = (int)val.cols();
		MyMatrix rval(nx, ny);
		for (int i = 0; i < nx; ++i) {
			for (int j = 0; j < ny; ++j) {
				rval(i, j) = val(i, j).real();
			}	// j
		}	// i
		typename EigenSolver<MyMatrix>::EigenvectorsType vec =
			solver.eigenvectors();
		nx = (int)vec.rows();
		ny = (int)vec.cols();
		MyMatrix rvec(nx, ny);
		for (int i = 0; i < nx; ++i) {
			for (int j = 0; j < ny; ++j) {
				rvec(i, j) = vec(i, j).real();
			}	// j
		}	// i
		std::vector<MyPair> oPairs;
		for (int i = 0; i < n; ++i) {
			double xx = rval(i, 0);
			if (xx > 0) {
				MyPair o((size_t)i, xx);
				oPairs.push_back(o);
			}
		}				// i
		size_t nFacts = oPairs.size();
		bool bRet = (nFacts > 0);
		if (bRet) {
			std::sort(oPairs.begin(), oPairs.end(), MyComparePairDescFunc());
			for (size_t i = 0; i < nFacts; ++i) {
				const MyPair &pp = oPairs[i];
				const size_t ii = pp.first;
				pVals[i] = pp.second;
				for (size_t j = 0; j < nr; ++j) {
					double xx = rvec(j, ii);
					pVecs[j * nFacts + i] = xx;
				}				// j
			}				// i
		}				// bRet
		if (pNbFacts != nullptr) {
			*pNbFacts = nFacts;
		}
		return (bRet);
	}// compute_eigen_impl
					//////////////////////////////////////////
}	// namespace info
	/////////////////////////////////////////////////
