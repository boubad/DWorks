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
		template <class T>
		virtual bool to_bool_array(const std::vector<T> &data, std::vector<bool> &vRet) const = 0;
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
			const StringType &desc = StringType()) :_shortname(sname), _longname(lname), _description(desc) {}
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
	template <typename T, class S = std::wstring> 
	class NumVariableDesc : public VariableDesc<S> {
	public:
		typedef T DataType;
		typedef S StringType;
		//
		typedef VariableDesc<StringType> VariableDescType;
		typedef NumVariableDesc<DataType, StringType> NumVariableDescType;
	privat
		DataType _vmin;
		DataType _vmax;
	public:
		NumVariableDesc():_vmin(0),_vmax(0){}
		NumVariableDesc(const StringType &sname,
			const StringType &lname = StringType(),
			const StringType &desc = StringType()) : VariableDescType(sname,lname,desc),
			_vmin(0),_vmax(0){}
		NumVariableDesc(const NumVariableDescType &other) : VariableDescType(other),
			_vmin(other._vmin), _vmax(other._vmax) {}
		NumVariableDescType & operator=(const NumVariableDescType &other) {
			if (this != &other) {
				VariableDescType::operator=(other);
				this->_vmin = other._vmin;
				this->_vmax = other._vmax;
			}
			return (*this);
		}
		virtual ~NumVariableDesc(){}
	public:
		virtual VariableType variableType(void) const {
			return (VariableType::numType);
		}
		virtual bool is_valid(void) const {
			return (this->_min < this->_vmax);
		}
		template <class X>
		virtual bool to_bool_array(const std::vector<X> &data, std::vector<bool> &vRet) const {
			size_t nClasses = 5;
			std::vector<X> limits;
			std::vector<int> vals;
			bool bRet = make_discrete(data, nClasses, limits, vals);
			if (!bRet) {
				return (false);
			}
			assert(nClasses > 0);
			size_t nMax = (size_t)(nClasses - 1);
			this->convert_to_bool_array(vals, nMax, vRet);
			return (true);
		}//to_bool_array
	public:
		DataType min(void) const {
			return (this->_vmin);
		}
		void min(const DataType v) {
			this->_vmin = v;
		}
		DataType max(void) const {
			return (this->_vmax);
		}
		void max(const DataType v) {
			this->_vmax = v;
		}
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
			assert(_nbModel > 0);
		}
		OrdinalVariableDesc(const OrdinalVariableDescType &other) : VariableDescType(other), _nbModal(nb) {
		}
		OrdinalVariableDescType & operator=(constOrdinalVariableDescType &other) {
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
		virtual bool to_bool_array(const std::vector<X> &data, std::vector<bool> &vRet) const {
			assert(this->is_valid());
			X nMax = (X)(this->_nbModal - 1);
			this->convert_to_bool_array(data, nMax, vRet);
			return (true);
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
		virtual bool to_bool_array(const std::vector<StringType> &data, std::vector<bool> &vRet) const {
			assert(this->is_valid());
			std::map<StringType, int> oMap;
			this->get_modalites(oMap);
			size_t nMax = (size_t)(oMap.size() - 1);
			const size_t n = data.size();
			std::vector<int> vals(n);
			for (size_t i = 0; i < n; ++i) {
				StringType key = data[i];
				assert(oMap.find(key) != oMap.end());
				vals[i] = oMap[key];
			}// i
			this->convert_to_bool_array(vals, nMax, vRet);
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
		template <class T>
		virtual bool to_bool_array(const std::vector<bool> &data, std::vector<bool> &vRet) const {
			vRet = data;
			return (true);
		}//to_bool_array
	}; // class NominalVariableDesc<T,S>
	////////////////////////////////////////////////////
}// namespace info
/////////////////////////////////////
#endif // !__VARIABLE_H__
