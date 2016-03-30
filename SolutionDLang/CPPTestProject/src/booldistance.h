#pragma once
#ifndef __BOOLDISTANCE_H__
#define __BOOLDISTANCE_H__
//////////////////////////////
#include "distance.h"
/////////////////////////////////
namespace info {
	/////////////////////////////////////
	enum class IndiceDistanceType {
		indiceInvalid,
		RusselAndRao,
		Jaccard,
		Dice,
		SokalAndSneath_2,
		Kulczinski_1,
		Kulczinski_2,
		Ochiai,
		Simpson,
		KochenAndWong,
		SokalAndMichener,
		SokalAndSneath_1,
		RogersAndTanimoto,
		SokalAndSneath_4,
		SokalAndSneath_5,
		Roux_1,
		Hamann,
		Yule,
		PhiPearson
	};//IndiceDistanceType
	////////////////////////////////////////
	template <typename Z = double> class IndiceDistanceFunc : public DistanceFunc<bool, Z> {
	public:
		typedef bool DataType;
		typedef Z DistanceType;
		typedef std::valarray<DataType> DataTypeArray;
		typedef DistanceFunc<DataType, DistanceType> DistanceFuncType;
		typedef IndiceDistanceFunc<DistanceType> IndiceDistanceFuncType;
	private:
		IndiceDistanceType _type;
	public:
		IndiceDistanceFunc(const IndiceDistanceType t = IndiceDistanceType::indiceInvalid) :_type(t) {}
		IndiceDistanceFunc(const IndiceDistanceFuncType &other) :DistanceFuncType(other), _type(other._type) {}
		IndiceDistanceFuncType & operator=(const IndiceDistanceFuncType &other) {
			if (this != &other) {
				DistanceFuncType::operator=(other);
				this->_type = other._type;
			}
			return (*this);
		}
		virtual ~IndiceDistanceFunc() {}
	public:
		virtual bool is_valid(void) const {
			return (this->_type != IndiceDistanceType::indiceInvalid);
		}
		IndiceDistanceType indiceDistanceType(void) const {
			return (this->_type);
		}
		void indiceDistanceType(const IndiceDistanceType c) {
			this->_type = c;
		}
	public:
		static void get_all_indices(std::vector< IndiceDistanceType> &oVec) {
			oVec.clear();
			oVec.push_back(IndiceDistanceType::Dice);
			oVec.push_back(IndiceDistanceType::Hamann);
			oVec.push_back(IndiceDistanceType::Jaccard);
			oVec.push_back(IndiceDistanceType::KochenAndWong);
			oVec.push_back(IndiceDistanceType::Kulczinski_1);
			oVec.push_back(IndiceDistanceType::Kulczinski_2);
			oVec.push_back(IndiceDistanceType::Ochiai);
			oVec.push_back(IndiceDistanceType::PhiPearson);
			oVec.push_back(IndiceDistanceType::RogersAndTanimoto);
			oVec.push_back(IndiceDistanceType::Roux_1);
			oVec.push_back(IndiceDistanceType::RusselAndRao);
			oVec.push_back(IndiceDistanceType::Simpson);
			oVec.push_back(IndiceDistanceType::SokalAndMichener);
			oVec.push_back(IndiceDistanceType::SokalAndSneath_1);
			oVec.push_back(IndiceDistanceType::SokalAndSneath_2);
			oVec.push_back(IndiceDistanceType::SokalAndSneath_4);
			oVec.push_back(IndiceDistanceType::SokalAndSneath_5);
			oVec.push_back(IndiceDistanceType::Yule);
		}//get_all_indice
	public:
		virtual DistanceType perform_compute(const DataTypeArray & v1, const DataTypeArray & v2) const {
			const size_t nz = v1.size();
			assert(nz > 0);
			assert(v2.size() == nz);
			double res = 0;
			int n = 0, p = 0, q = 0, c = 0, d = 0;
			this->compute_params(v1, v2, n, p, q, c, d);
			double dn = (double)n;
			double dp = (double)p;
			double dq = (double)q;
			double dc = (double)c;
			double dd = (double)d;
			double dMax = 1.0;
			const IndiceDistanceType tt = this->_type;
			switch (tt) {
			case IndiceDistanceType::RusselAndRao:
				res = dc / dn;
				break;
			case IndiceDistanceType::Jaccard:
				res = dc / (dp + dq - dc);
				break;
			case IndiceDistanceType::Dice:
				res = (2.0 * dc) / (dp + dq);
				break;
			case IndiceDistanceType::SokalAndSneath_2:
				res = dc / (2.0 * (p + q) - 3.0 * c);
				break;
			case IndiceDistanceType::Kulczinski_1:
				res = dc / (dp + dq - 2.0 * dc);
				break;
			case IndiceDistanceType::Kulczinski_2:
				res = ((dc / dp) + (dc / dq)) / 2.0;
				break;
			case IndiceDistanceType::Ochiai:
				res = (dc / std::sqrt(dp * dq));
				break;
			case IndiceDistanceType::Simpson:
				if (dp < dq) {
					res = dc / dp;
				}
				else {
					res = dc / dq;
				}
				break;
			case IndiceDistanceType::KochenAndWong:
				dMax = dn;
				res = (dn * dc) / (dp * dq);
				break;
			case IndiceDistanceType::SokalAndMichener:
				res = (dc + dd) / dn;
				break;
			case IndiceDistanceType::SokalAndSneath_1:
				res = 2.0 * ((dc + dd) / (dn + dc + dd));
				break;
			case IndiceDistanceType::RogersAndTanimoto:
				res = (dc + dd) / (2.0 *dn - (dc + dd));
				break;
			case IndiceDistanceType::SokalAndSneath_4:
			{
				double temp = (dc / dp) + (dc / dq) + dd / (dn - dp) + dd / (dn - dq);
				res = temp / 4.0;
			}
			break;
			case IndiceDistanceType::SokalAndSneath_5:
				res = (dc * dn) / (dp*dq*(dn - dp)*(dn - dq));
				break;
			case IndiceDistanceType::Roux_1:
			{
				double d1 = (dp < dq) ? dp : dq;
				double d2 = ((dn - dp) < (dn - dq)) ? dn - dp : dn - dq;
				res = (dc + dd) / (d1 + d2);
			}
			break;
			case IndiceDistanceType::Hamann:
				res = ((dc + dd) - (dp - dc) - (dq - dc)) / dn;
				break;
			case IndiceDistanceType::Yule:
			{
				double dtn = dc * dd - (dp - dc)*(dq - dc);
				double dtd = dc * dd + (dp - dc) *(dq - dc);
				res = dtn / dtd;
			}
			break;
			case IndiceDistanceType::PhiPearson:
			{
				double dtn = dc * dd - (dp - dc)*(dq - dc);
				double dtd = dp*dq*(dn - dp)*(dn - dq);
				res = dtn / dtd;
			}
			break;
			default:
				// should not fall here
				assert(false);
				break;
			}// tt
			return (DistanceType)(dMax - res);
		}// perform_compute
	protected:
		void compute_params(const DataTypeArray &data1, const DataTypeArray &data2,
			int &n, int &p, int &q, int &c, int &d) const {
			n = (int)data1.size();
			assert(n > 0);
			assert(data1.size() == data2.size());
			p = 0;
			q = 0;
			c = 0;
			d = 0;
			for (int i = 0; i < n; ++i) {
				const DataType b1 = data1[i];
				const DataType b2 = data2[i];
				if (b1) {
					++p;
				}
				if (b2) {
					++q;
				}
				if (b1 && b2) {
					++c;
				}
				if ((!b1) && (!b2)) {
					++d;
				}
			}// i
		}// compute_params
	}; // class  IndiceDistanceFunc<Z>
	/////////////////////////////////////////
}// namespace info
///////////////////////////
#endif // !__BOOLDISTANCE_H__
// eof: booldistance.h
