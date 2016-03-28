#pragma once
#ifndef __ANACOMPO_H__
#define __ANACOMPO_H__
///////////////////////////////////////
#include <cassert>
#include <vector>
#include <valarray>
#include <algorithm>
#include <cmath>
#include <memory>
///////////////////////////////////
#include "eigen.h"
////////////////////////////////////
namespace info {
	///////////////////////////////////////////////
	template<typename T = double>
	class IntraEigenSolver {
	public:
		template<typename X, typename Z>
		static bool compute_eigen(const size_t nr,
			const X *pSrc, size_t &nFacts, Z *pa, Z *pv) {
			//
			assert(nr > 1);
			assert(pSrc != nullptr);
			assert(pa != nullptr);
			assert(pv != nullptr);
			//
			const size_t nn = (size_t)(nr * nr);
			//
			std::unique_ptr<double> xData(new double[nn]);
			double *pData = xData.get();
			assert(pData != nullptr);
			std::unique_ptr<double> xVals(new double[nr]);
			double *pVals = xVals.get();
			assert(pVals != nullptr);
			std::unique_ptr<double> xVecs(new double[nn]);
			double *pVecs = xVecs.get();
			assert(pVecs != nullptr);
			nFacts = 0;
			for (size_t i = 0; i < nr; ++i) {
				for (size_t j = 0; j <= i; ++j) {
					const size_t k = (size_t)(i * nr + j);
					double v = (double)pSrc[k];
					pData[k] = v;
					if (j != i) {
						pData[j * nr + i] = v;
					}
				}
			}
			if (!compute_eigen_impl(nr, pData, pVals, pVecs, &nFacts)) {
				return false;
			}
			for (size_t i = 0; i < nFacts; ++i) {
				pa[i] = (Z)pVals[i];
				for (size_t j = 0; j < nr; ++j) {
					const size_t k = (size_t)(j * nFacts + i);
					pv[k] = (Z)pVecs[k];
				}
			}
			return (true);
		}// compute_eigen

		template<typename X, typename Z>
		static bool compute_eigen(const size_t nr,
			const std::valarray<X> &oSrc, size_t &nFacts,
			std::valarray<Z> &oVals, std::valarray<Z> &oVecs) {
			//
			assert(nr > 1);
			const size_t nn = (size_t)(nr * nr);
			assert(oSrc.size() >= nn);
			//
			std::unique_ptr<double> xData(new double[nn]);
			double *pData = xData.get();
			assert(pData != nullptr);
			std::unique_ptr<double> xVals(new double[nr]);
			double *pVals = xVals.get();
			assert(pVals != nullptr);
			std::unique_ptr<double> xVecs(new double[nn]);
			double *pVecs = xVecs.get();
			assert(pVecs != nullptr);
			nFacts = 0;
			for (size_t i = 0; i < nr; ++i) {
				for (size_t j = 0; j <= i; ++j) {
					const size_t k = (size_t)(i * nr + j);
					double v = (double)oSrc[k];
					pData[k] = v;
					if (j != i) {
						pData[j * nr + i] = v;
					}
				}
			}
			if (!compute_eigen_impl(nr, pData, pVals, pVecs, &nFacts)) {
				return false;
			}
			oVals.resize(nFacts);
			oVecs.resize(nFacts * nr);
			for (size_t i = 0; i < nFacts; ++i) {
				oVals[i] = (Z)pVals[i];
				for (size_t j = 0; j < nr; ++j) {
					const size_t k = (size_t)(j * nFacts + i);
					oVecs[k] = (Z)pVecs[k];
				}
			}
			return (true);
		}// compute_eigen

		template<typename X, typename Z, class ALLOCX, class ALLOCZ>
		static bool compute_eigen(const size_t nr,
			const std::vector<X, ALLOCX> &oSrc, size_t &nFacts,
			std::vector<Z, ALLOCZ> &oVals, std::vector<Z, ALLOCZ> &oVecs) {
			//
			assert(nr > 1);
			const size_t nn = (size_t)(nr * nr);
			assert(oSrc.size() >= nn);
			//
			std::unique_ptr<double> xData(new double[nn]);
			double *pData = xData.get();
			assert(pData != nullptr);
			std::unique_ptr<double> xVals(new double[nr]);
			double *pVals = xVals.get();
			assert(pVals != nullptr);
			std::unique_ptr<double> xVecs(new double[nn]);
			double *pVecs = xVecs.get();
			assert(pVecs != nullptr);
			nFacts = 0;
			for (size_t i = 0; i < nr; ++i) {
				for (size_t j = 0; j <= i; ++j) {
					const size_t k = (size_t)(i * nr + j);
					double v = (double)oSrc[k];
					pData[k] = v;
					if (j != i) {
						pData[j * nr + i] = v;
					}
				}
			}
			if (!compute_eigen_impl(nr, pData, pVals, pVecs, &nFacts)) {
				return false;
			}
			oVals.resize(nFacts);
			oVecs.resize(nFacts * nr);
			for (size_t i = 0; i < nFacts; ++i) {
				oVals[i] = (Z)pVals[i];
				for (size_t j = 0; j < nr; ++j) {
					const size_t k = (size_t)(j * nFacts + i);
					oVecs[k] = (Z)pVecs[k];
				}
			}
			return (true);
		}
		//////////////////////////////////////////////
		template<typename X, class ALLOCX, class ALLOCT>
		static bool compute_stats(const size_t nr, const size_t nv,
			const std::vector<X, ALLOCX> &oSrc, std::vector<T, ALLOCT> &oMeans,
			std::vector<T, ALLOCT> &oStds, std::vector<T, ALLOCT> &oCorr) {
			//
			assert(nv > 1);
			assert(nr > nv);
			//
			const size_t nnv = nv * nv;
			const size_t nnr = nr * nv;
			//
			assert(oSrc.size() >= nnr);
			//
			oMeans = std::vector<T, ALLOCT >(nv);
			oStds = std::vector<T, ALLOCT >(nv);
			oCorr = std::vector<T, ALLOCT >(nnv);
			//
			std::vector<double> oTemp(nnr);
			std::vector<double> xCorr(nnv);
			std::vector<double> xStds(nv);
			bool bRet = true;
			double dnr = (double)nr;
			double dnr1 = (double)(nr - 1);
			for (size_t ivar = 0; ivar < nv; ++ivar) {
				xCorr[ivar * nv + ivar] = 1.0;
				oCorr[ivar * nv + ivar] = (T) 1.0;
				double s = 0.0;
				for (size_t i = 0; i < nr; ++i) {
					const size_t k = i * nv + ivar;
					double x = (double)oSrc[k];
					oTemp[k] = x;
					s += x;
				} // i
				double moy = s / dnr;
				oMeans[ivar] = (T)moy;
				s = 0.0;
				for (size_t i = 0; i < nr; ++i) {
					const size_t k = i * nv + ivar;
					double x = oTemp[k] - moy;
					oTemp[k] = x;
					s += x * x;
				} // i
				double dvar = s / dnr;
				if (dvar > 0.0) {
					double ec1 = std::sqrt(dvar);
					xStds[ivar] = ec1;
					oStds[ivar] = (T)ec1;
					for (size_t ivar1 = 0; ivar1 < ivar; ++ivar1) {
						double s = 0.0;
						for (size_t i = 0; i < nr; ++i) {
							double x = oTemp[i * nv + ivar1]
								* oTemp[i * nv + ivar];
							s += x;
						}
						s /= dnr;
						double r = s / (ec1 * xStds[ivar1]);
						T rt = (T)r;
						xCorr[ivar * nv + ivar1] = r;
						oCorr[ivar * nv + ivar1] = rt;
						xCorr[ivar1 * nv + ivar] = r;
						oCorr[ivar1 * nv + ivar] = rt;
					}
				}
				else {
					bRet = false;
				}
			}
			return (bRet);
		}// compute_stats
		template<typename X>
		static bool compute_stats(const size_t nr, const size_t nv,
			const std::valarray<X> &oSrc, std::valarray<T> &oMeans,
			std::valarray<T> &oStds, std::valarray<T> &oCorr) {
			//
			assert(nv > 1);
			assert(nr > nv);
			//
			const size_t nnv = nv * nv;
			const size_t nnr = nr * nv;
			//
			assert(oSrc.size() >= nnr);
			//
			std::valarray<double> means(nv), stds(nv), corrs(nnv), temp(nnr);
			//
			for (size_t ivar = 0; ivar < nv; ++ivar) {
				std::valarray<double> data(nr);
				for (size_t i = 0; i < nr; ++i) {
					const size_t ipos = i * nv + ivar;
					const double d = (double)oSrc[ipos];
					data[i] = d;
					temp[ipos] = d;
				}// i
				const double moyenne = data.sum() / nr;
				means[ivar] = moyenne;
				std::valarray<double> dt = data - moyenne;
				temp[std::slice(ivar, nr, nv)] = dt;
				std::valarray<double> dt2 = dt * dt;
				const double ds2 = dt2.sum() / nr;
				if (ds2 <= 0.0) {
					return (false);
				}
				stds[ivar] = std::sqrt(ds2);
			}// ivar
			//
			oMeans.resize(nv);
			oStds.resize(nv);
			oCorr.resize(nnr);
			for (size_t ivar1 = 0; ivar1 < nv; ++ivar1) {
				oMeans[ivar1] = (T)means[ivar1];
				const double ec1 = stds[ivar1];
				oStds[ivar1] = (T)ec1;
				oCorr[ivar1 * nv + ivar1] = 1.0;
				std::valarray<double> data1 = temp[std::slice(ivar1,nr,nv)];
				for (size_t ivar2 = 0; ivar2 < ivar1; ++ivar2) {
					std::valarray<double> data2 = temp[std::slice(ivar2, nr, nv)];
					std::valarray<double> dt2 = data1 * data2;
					const double cc = dt2.sum() / nr;
					const T r = (T)(cc / (ec1 * stds[ivar2]));
					oCorr[ivar1 * nv + ivar2] = r;
					oCorr[ivar2 * nv + ivar1] = r;
				}// ivar2
			}// ivar1
			//
			return (true);
		}// compute_stats
		////////////////////////////////////////////////
		template<typename X, class ALLOCX, class ALLOCT>
		static bool compute_anacompo(const size_t nr, const size_t nv,
			const std::vector<X, ALLOCX> &oSrc, std::vector<T, ALLOCT> &oMeans,
			std::vector<T, ALLOCT> &oStds, std::vector<T, ALLOCT> &oCorr,
			size_t &nFacts, std::vector<T, ALLOCT> &oFreq,
			std::vector<T, ALLOCT> &oVals, std::vector<T, ALLOCT> &oVecs,
			std::vector<T, ALLOCT> &oVars, std::vector<T, ALLOCT> &oInds) {
			//
			assert(nv > 1);
			assert(nr > nv);
			//
			const size_t nnv = nv * nv;
			const size_t nnr = nr * nv;
			//
			assert(oSrc.size() >= nnr);
			//
			oMeans = std::vector<T, ALLOCT >(nv);
			oStds = std::vector<T, ALLOCT >(nv);
			oCorr = std::vector<T, ALLOCT >(nnv);
			//
			std::vector<double> oTemp(nnr);
			std::vector<double> xCorr(nnv);
			std::vector<double> xStds(nv);
			bool bRet = true;
			double dnr = (double)nr;
			double dnr1 = (double)(nr - 1);
			for (size_t ivar = 0; ivar < nv; ++ivar) {
				xCorr[ivar * nv + ivar] = 1.0;
				oCorr[ivar * nv + ivar] = (T) 1.0;
				double s = 0.0;
				for (size_t i = 0; i < nr; ++i) {
					const size_t k = i * nv + ivar;
					double x = (double)oSrc[k];
					oTemp[k] = x;
					s += x;
				} // i
				double moy = s / dnr;
				oMeans[ivar] = (T)moy;
				s = 0.0;
				for (size_t i = 0; i < nr; ++i) {
					const size_t k = i * nv + ivar;
					double x = oTemp[k] - moy;
					oTemp[k] = x;
					s += x * x;
				} // i
				double dvar = s / dnr;
				if (dvar > 0.0) {
					double ec1 = std::sqrt(dvar);
					xStds[ivar] = ec1;
					oStds[ivar] = (T)ec1;
					for (size_t ivar1 = 0; ivar1 < ivar; ++ivar1) {
						double s = 0.0;
						for (size_t i = 0; i < nr; ++i) {
							double x = oTemp[i * nv + ivar1]
								* oTemp[i * nv + ivar];
							s += x;
						}
						s /= dnr;
						double r = s / (ec1 * xStds[ivar1]);
						T rt = (T)r;
						xCorr[ivar * nv + ivar1] = r;
						oCorr[ivar * nv + ivar1] = rt;
						xCorr[ivar1 * nv + ivar] = r;
						oCorr[ivar1 * nv + ivar] = rt;
					}
				}
				else {
					bRet = false;
				}
			}
			if (bRet) {
				for (size_t ivar = 0; ivar < nv; ++ivar) {
					double vec = xStds[ivar];
					for (size_t i = 0; i < nr; ++i) {
						const size_t k = i * nv + ivar;
						double x = oTemp[k] / vec;
						oTemp[k] = x;
					}
				}
			}
			nFacts = 0;
			std::vector<double> xVals, xVecs;
			double stotal = 0.0;
			bRet = bRet
				&& IntraEigenSolver<T>::compute_eigen(nv, xCorr, nFacts, xVals,
					xVecs);
			if (bRet) {
				oVals.resize(nFacts);
				oVecs.resize(nFacts * nv);
				for (size_t i = 0; i < nFacts; ++i) {
					double x = xVals[i];
					oVals[i] = (T)x;
					stotal += x;
				}
				if (stotal <= 0) {
					bRet = false;
				}
			}
			if (bRet) {
				oFreq.resize(nFacts);
				oVecs.resize(nFacts * nv);
				oVars.resize(nFacts * nv);
				oInds.resize(nFacts * nr);
				double act2 = std::sqrt((double)nv);
				for (size_t iFact = 0; iFact < nFacts; ++iFact) {
					double val = xVals[iFact];
					oFreq[iFact] = (T)(val / stotal);
					double fval = std::sqrt(val);
					for (size_t ivar = 0; ivar < nv; ++ivar) {
						const size_t k = ivar * nFacts + iFact;
						double vx = xVecs[k];
						oVecs[k] = (T)vx;
						oVars[k] = (T)(fval * xVecs[k]);
					}
					for (size_t irow = 0; irow < nr; ++irow) {
						double s = 0.0;
						for (size_t ivar = 0; ivar < nv; ++ivar) {
							double x = oTemp[irow * nv + ivar]
								* xVecs[ivar * nFacts + iFact];
							s += x;
						}
						oInds[irow * nFacts + iFact] = (T)(s / act2);
					}
				}
			}
			return (bRet);
		}// compute_anacompo

		template<typename X>
		static bool compute_anacompo(const size_t nr, const size_t nv,
			const std::valarray<X> &oSrc, std::valarray<T> &oMeans,
			std::valarray<T> &oStds, std::valarray<T> &oCorr,
			size_t &nFacts, std::valarray<T> &oFreq,
			std::valarray<T> &oVals, std::valarray<T> &oVecs,
			std::valarray<T> &oVars, std::valarray<T> &oInds) {
			//
			assert(nv > 1);
			assert(nr > nv);
			//
			const size_t nnv = nv * nv;
			const size_t nnr = nr * nv;
			//
			assert(oSrc.size() >= nnr);
			//
			oMeans.resize(nv);
			oStds.resize(nv);
			oCorr.resize(nnv);
			//
			std::vector<double> oTemp(nnr);
			std::vector<double> xCorr(nnv);
			std::vector<double> xStds(nv);
			bool bRet = true;
			double dnr = (double)nr;
			double dnr1 = (double)(nr - 1);
			for (size_t ivar = 0; ivar < nv; ++ivar) {
				xCorr[ivar * nv + ivar] = 1.0;
				oCorr[ivar * nv + ivar] = (T) 1.0;
				double s = 0.0;
				for (size_t i = 0; i < nr; ++i) {
					const size_t k = i * nv + ivar;
					double x = (double)oSrc[k];
					oTemp[k] = x;
					s += x;
				} // i
				double moy = s / dnr;
				oMeans[ivar] = (T)moy;
				s = 0.0;
				for (size_t i = 0; i < nr; ++i) {
					const size_t k = i * nv + ivar;
					double x = oTemp[k] - moy;
					oTemp[k] = x;
					s += x * x;
				} // i
				double dvar = s / dnr;
				if (dvar > 0.0) {
					double ec1 = std::sqrt(dvar);
					xStds[ivar] = ec1;
					oStds[ivar] = (T)ec1;
					for (size_t ivar1 = 0; ivar1 < ivar; ++ivar1) {
						double s = 0.0;
						for (size_t i = 0; i < nr; ++i) {
							double x = oTemp[i * nv + ivar1]
								* oTemp[i * nv + ivar];
							s += x;
						}
						s /= dnr;
						double r = s / (ec1 * xStds[ivar1]);
						T rt = (T)r;
						xCorr[ivar * nv + ivar1] = r;
						oCorr[ivar * nv + ivar1] = rt;
						xCorr[ivar1 * nv + ivar] = r;
						oCorr[ivar1 * nv + ivar] = rt;
					}
				}
				else {
					bRet = false;
				}
			}
			if (bRet) {
				for (size_t ivar = 0; ivar < nv; ++ivar) {
					double vec = xStds[ivar];
					for (size_t i = 0; i < nr; ++i) {
						const size_t k = i * nv + ivar;
						double x = oTemp[k] / vec;
						oTemp[k] = x;
					}
				}
			}

			nFacts = 0;
			std::vector<double> xVals, xVecs;
			double stotal = 0.0;
			bRet = bRet
				&& IntraEigenSolver<T>::compute_eigen(nv, xCorr, nFacts, xVals,
					xVecs);
			if (bRet) {
				oVals.resize(nFacts);
				oVecs.resize(nFacts * nv);
				for (size_t i = 0; i < nFacts; ++i) {
					double x = xVals[i];
					oVals[i] = (T)x;
					stotal += x;
				}
				if (stotal <= 0) {
					bRet = false;
				}
			}
			if (bRet) {
				oFreq.resize(nFacts);
				oVecs.resize(nFacts * nv);
				oVars.resize(nFacts * nv);
				oInds.resize(nFacts * nr);
				double act2 = std::sqrt((double)nv);
				for (size_t iFact = 0; iFact < nFacts; ++iFact) {
					double val = xVals[iFact];
					oFreq[iFact] = (T)(val / stotal);
					double fval = std::sqrt(val);
					for (size_t ivar = 0; ivar < nv; ++ivar) {
						const size_t k = ivar * nFacts + iFact;
						double vx = xVecs[k];
						oVecs[k] = (T)vx;
						oVars[k] = (T)(fval * xVecs[k]);
					}
					for (size_t irow = 0; irow < nr; ++irow) {
						double s = 0.0;
						for (size_t ivar = 0; ivar < nv; ++ivar) {
							double x = oTemp[irow * nv + ivar]
								* xVecs[ivar * nFacts + iFact];
							s += x;
						}
						oInds[irow * nFacts + iFact] = (T)(s / act2);
					}
				}
			}
			return (bRet);
		}// compute_anacompo

		template<typename X, class ALLOCX, class ALLOCT>
		static bool compute_anacompo(const size_t nr, const size_t nv,
			const std::vector<X, ALLOCX> &oSrc, size_t &nFacts,
			std::vector<T, ALLOCT> &oVars, std::vector<T, ALLOCT> &oInds) {
			std::vector<T, ALLOCT> oMeans, oStds, oCorr, oFreq, oVals, oVecs;
			return (compute_anacompo(nr, nv, oSrc, oMeans,
				oStds, oCorr, nFacts, oFreq, oVals, oVecs, oVars, oInds));
		}

		template<typename X>
		static bool compute_anacompo(const size_t nr, const size_t nv,
			const std::valarray<X> &oSrc, size_t &nFacts,
			std::valarray<T> &oVars, std::valarray<T> &oInds) {
			std::valarray<T> oMeans, oStds, oCorr, oFreq, oVals, oVecs;
			return (compute_anacompo(nr, nv, oSrc, oMeans,
				oStds, oCorr, nFacts, oFreq, oVals, oVecs, oVars, oInds));
		}
		template<typename X>
		static bool compute_anacompo(const size_t nr, const size_t nv,
			const std::valarray<X> &oSrc, size_t &nFacts, std::valarray<T> &oFreq,
			std::valarray<T> &oVars, std::valarray<T> &oInds) {
			std::valarray<T> oMeans, oStds, oCorr, oVals, oVecs;
			return (compute_anacompo(nr, nv, oSrc, oMeans,
				oStds, oCorr, nFacts, oFreq, oVals, oVecs, oVars, oInds));
		}
		template<typename X, class ALLOCX, class ALLOCT>
		static bool compute_anacompo(const size_t nr, const size_t nv,
			const std::vector<X, ALLOCX> &oSrc, size_t &nFacts, std::vector<T, ALLOCT> &oFreq,
			std::vector<T, ALLOCT> &oVars, std::vector<T, ALLOCT> &oInds) {
			std::vector<T, ALLOCT> oMeans, oStds, oCorr, oVals, oVecs;
			return (compute_anacompo(nr, nv, oSrc, oMeans,
				oStds, oCorr, nFacts, oFreq, oVals, oVecs, oVars, oInds));
		}
	};
	// class IntraEigenSolver<T>
}
///////////////////////////////////////
#endif // __ANACOMPO_H__
