#pragma once
#ifndef __VARIABLE_H__
#define __VARIABLE_H__
///////////////////////////////////
#include <cassert>
#include <string>
#include <set>
#include <map>
////////////////////////////////////////////
#include "utils.h"
////////////////////////////////////////////
namespace info {
	/////////////////////////////////////////
	enum class VariableType {noType,booleanType,ordinalType,nominalType,numType};
	///////////////////////////////////////////////
	template <class STRINGTYPE = std::wstring> class VariableDesc {
	public:
		typedef STRINGTYPE StringType;
		typedef VariableDesc<StringType> VariableDescType;
	private:
		StringType _shortName;
		StringType _longName;
		StringType _description;
	public:
		virtual ~VariableDesc(){}
		virtual VariableType variableType(void) const = 0;
		virtual bool is_valid(void) const = 0;
	public:
		const StringType & shortName(void) const {
			return (this->_shortName);
		}
		void shortName(const StringType &s) {
			this->_shortName = s;
		}
		const StringType & longName(void) const {
			return (this->_longName);
		}
		void longName(const StringType &s) {
			this->_longName = s;
		}
		const StringType & description(void) const {
			return (this->_description);
		}
		void description(const StringType &s) {
			this->_description = s;
		}
	public:
		VariableDesc() {}
		VariableDesc(const StringType &sname,
			const StringType &lname = StringType(),
			const StringType &desc = StringType()) :_shortName(sname), _longName(lname), _description(desc) {}
		VariableDesc(const VariableDescType &other) :_shortName(other._shortName), _longName(other._longName),
			_description(other._description) {}
		VariableDescType & operator=(const VariableDescType &other) {
			if (this != &other) {
				this->_shortName = other._shortName;
				this->_longName = other._longName;
				this->_description = other._description;
			}
			return (*this);
		}
		template <typename T>
		void convert_to_bool_array(const std::vector<T> &data, const T nMax, std::vector<bool> &vRet) const {
			assert(nMax > 0);
			std::wstring sMax;
			std::map<T, std::wstring> oMap;
			const size_t nxMax = global_integer_to_binary_string(nMax, sMax);
			assert(nMax > 0);
			for (auto it = data.begin(); it != data.end(); ++it) {
				const T key = *it;
				assert(key >= 0);
				assert(key <= nMax);
				if (oMap.find(key) == oMap.end()) {
					std::wstring s;
					size_t nx = global_integer_to_binary_string(key, s);
					while (s.size() < nxMax) {
						s = L"0" + s;
					}// while
					oMap[key] = s;
				}
			}// it
			vRet.clear();
			std::wstring ss;
			for (auto it = data.begin(); it != data.end(); ++it) {
				const T key = *it;
				assert(oMap.find(key) != oMap.end());
				std::wstring s = oMap[key];
				ss += s;
			}// it
			for (auto it = ss.begin(); it != ss.end(); ++it) {
				wchar_t c = *it;
				if (c == L'0') {
					vRet.push_back(false);
				}
				else {
					vRet.push_back(true);
				}
			}// it
		}//convert_to_bool_array
	}; // class VariableDesc<STRINGTYPE>
	///////////////////////////////////////////////
	template <class S = std::wstring> 
	class NumVariableDesc : public VariableDesc<S> {
	public:
		typedef S StringType;
		//
		typedef VariableDesc<StringType> VariableDescType;
		typedef NumVariableDesc<StringType> NumVariableDescType;
	private:
		int	_nMax;
	public:
		NumVariableDesc():_nMax(7){}
		NumVariableDesc(const StringType &sname,
			const StringType &lname = StringType(),
			const StringType &desc = StringType()) : VariableDescType(sname,lname,desc),_nMax(7){}
		NumVariableDesc(const NumVariableDescType &other) : VariableDescType(other),_Nmax(other._nMax){}
		NumVariableDescType & operator=(const NumVariableDescType &other) {
			if (this != &other) {
				VariableDescType::operator=(other);
				this->_nMax = other._nMax;
			}
			return (*this);
		}
		virtual ~NumVariableDesc(){}
	public:
		int max_level(void) const {
			return (this->_nMax);
		}
		void max_level(int n) {
			if (n > 1) {
				this->_nMax = n;
			}
		}
		virtual VariableType variableType(void) const {
			return (VariableType::numType);
		}
		virtual bool is_valid(void) const {
			return (this->_nMax > 1);
		}
		template <class X>
		bool to_bool_array(const std::vector<X> &data, std::vector<bool> &vRet) const {
			assert(this->is_valid());
			int nMax = this->_nMax;
			this->convert_to_bool_array(data, nMax, vRet);
			return (true);
		}//to_bool_array
	}; // class NumVariableDesc<T,S>
	///////////////////////////////////////////
	template <class S = std::wstring>
	class OrdinalVariableDesc : public VariableDesc<S> {
	public:
		typedef S StringType;
		typedef VariableDesc<S> VariableDescType;
		typedef OrdinalVariableDesc<S> OrdinalVariableDescType;
	private:
		size_t _nbModal;
	public:
		OrdinalVariableDesc() :_nbModel(0){}
		OrdinalVariableDesc(const size_t nb,const StringType &sname,
			const StringType &lname = StringType(),
			const StringType &desc = StringType()) : VariableDescType(sname, lname, desc),_nbModal(nb){
			assert(_nbModal > 0);
		}
		OrdinalVariableDesc(const OrdinalVariableDescType &other) : VariableDescType(other), _nbModal(nb) {
		}
		OrdinalVariableDescType & operator=(const OrdinalVariableDescType &other) {
			if (this != &other) {
				VariableDescType::operator=(other);
				this->_nbModal = other._nbModel;
			}
			return (*this);
		}
		virtual ~OrdinalVariableDesc() {}
	public:
		virtual VariableType variableType(void) const {
			return (VariableType::ordinalType);
		}
		virtual bool is_valid(void) const {
			return (this->_nbModal > 0);
		}
		template <class X>
		bool to_bool_array(const std::vector<X> &data, std::vector<bool> &vRet) const {
			assert(this->is_valid());
			X nMax = (X)(this->_nbModal - 1);
			this->convert_to_bool_array(data, nMax, vRet);
			return (true);
		}//to_bool_array
		template <class X>
		bool to_bool_array(const std::valarray<X> &data, std::vector<bool> &vRet) const {
			assert(this->is_valid());
			const size_t n = data.size();
			assert(n > 0);
			std::vector<X> oVec(n);
			for (size_t i = 0; i < n; ++i) {
				oVec[i] = data[i];
			}
			return this->to_bool_array(oVec, vRet);
		}//to_bool_array
	public:
		size_t modalites_count(void) const {
			return (this->_nbModal);
		}
		void modalites_count(const size_t n) {
			this->_nbModal = n;
		}
	}; // class OrdinalVariableDesc<T,S>
	////////////////////////////////////////////////
	template <class S = std::wstring>
	class NominalVariableDesc : public VariableDesc<S> {
	public:
		typedef S StringType;
		typedef VariableDesc<S> VariableDescType;
		typedef NominalVariableDesc<S> NominalVariableDescType;
	private:
		std::set<StringType> _oSet;
	public:
		NominalVariableDesc(){}
		NominalVariableDesc(const StringType &sname,
			const std::set<StringType> &oSet = std::set<StringType>(),
			const StringType &lname = StringType(),
			const StringType &desc = StringType()) : VariableDescType(sname, lname, desc),_oSet(oSet) {}
		NominalVariableDesc(const NominalVariableDescType &other) : VariableDescType(other), _oSet(oSet) {}
		NominalVariableDescType & operator=(const NominalVariableDescType &other) {
			if (this != &other) {
				VariableDescType::operator=(other);
				this->_oSet = other._oSet;
			}
			return (*this);
		}
		virtual ~NominalVariableDesc() {}
	public:
		virtual VariableType variableType(void) const {
			return (VariableType::nominalType);
		}
		virtual bool is_valid(void) const {
			return (!this->_oSet.empty());
		}
		bool to_bool_array(const std::vector<StringType> &data, std::vector<bool> &vRet) const {
			assert(this->is_valid());
			std::map<StringType, int> oMap;
			this->get_modalites(oMap);
			int nMax = (int)(oMap.size() - 1);
			const size_t n = data.size();
			std::vector<int> vals(n);
			for (size_t i = 0; i < n; ++i) {
				StringType key = data[i];
				assert(oMap.find(key) != oMap.end());
				vals[i] = oMap[key];
			}// i
			VariableDescType::convert_to_bool_array(vals, nMax, vRet);
			return (true);
		}//to_bool_array
	public:
		void add_modalite(const StringType &s) {
			this->_oSet.insert(s);
		}
		void get_modalites(std::map<StringType, int> &oMap) const {
			oMap.clear();
			const std::set<StringType> &oSet = this->_oSet;
			int val = 0;
			for (auto it = oSet.begin(); it != oSet.end(); ++it) {
				StringType key = *it;
				oMap[key] = val++;
			}
		}// get_modalites
	}; // class NominalVariableDesc<S>
	//////////////////////////////////////////
	template <class S = std::wstring>
	class BooleanVariableDesc : public VariableDesc<S> {
	public:
		typedef S StringType;
		typedef VariableDesc<S> VariableDescType;
		typedef BooleanVariableDesc<S> BooleanVariableDescType;
	public:
		BooleanVariableDesc() {}
		BooleanVariableDesc(const StringType &sname,
			const StringType &lname = StringType(),
			const StringType &desc = StringType()) : VariableDescType(sname, lname, desc){}
		BooleanVariableDesc(const BooleanVariableDescType &other) : VariableDescType(other) {}
		BooleanVariableDescType & operator=(const BooleanVariableDescType &other) {
			if (this != &other) {
				VariableDescType::operator=(other);
			}
			return (*this);
		}
		virtual ~BooleanVariableDesc() {}
	public:
		virtual VariableType variableType(void) const {
			return (VariableType::booleanType);
		}
		virtual bool is_valid(void) const {
			return (true);
		}
	public:
		bool to_bool_array(const std::vector<bool> &data, std::vector<bool> &vRet) const {
			vRet = data;
			return (true);
		}//to_bool_array
	}; // class NominalVariableDesc<T,S>
	////////////////////////////////////////////////////
}// namespace info
/////////////////////////////////////
#endif // !__VARIABLE_H__
