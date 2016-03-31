#pragma once
#ifndef __VARIABLE_H__
#define __VARIABLE_H__
///////////////////////////////////
#include "utils.h"
////////////////////////////////////////////
namespace info {
	/////////////////////////////////////////
	enum class VariableType { noType, booleanType, ordinalType, nominalType, numType };
	///////////////////////////////////////////////
	class VariableDesc {
	private:
		StringType _shortName;
		StringType _longName;
		StringType _description;
	public:
		virtual ~VariableDesc();
		virtual VariableType variableType(void) const = 0;
		virtual bool is_valid(void) const = 0;
	public:
		const StringType & shortName(void) const;
		void shortName(const StringType &s);
		const StringType & longName(void) const;
		void longName(const StringType &s);
		const StringType & description(void) const;
		void description(const StringType &s);
	public:
		VariableDesc();
		VariableDesc(const StringType &sname,
			const StringType &lname = StringType(),
			const StringType &desc = StringType());
		VariableDesc(const VariableDesc &other);
		VariableDesc & operator=(const VariableDesc &other);
		template <typename T>
		void convert_to_bool_array(const std::vector<T> &data, const T nMax, std::vector<bool> &vRet) const {
			assert(nMax > 0);
			StringType sMax;
			std::map<T, StringType> oMap;
			const size_t nxMax = global_integer_to_binary_string(nMax, sMax);
			assert(nMax > 0);
			for (auto it = data.begin(); it != data.end(); ++it) {
				const T key = *it;
				assert(key >= 0);
				assert(key <= nMax);
				if (oMap.find(key) == oMap.end()) {
					StringType s;
					size_t nx = global_integer_to_binary_string(key, s);
					while (s.size() < nxMax) {
						s = STRING_ZERO + s;
					}// while
					oMap[key] = s;
				}
			}// it
			vRet.clear();
			StringType ss;
			for (auto it = data.begin(); it != data.end(); ++it) {
				const T key = *it;
				assert(oMap.find(key) != oMap.end());
				std::wstring s = oMap[key];
				ss += s;
			}// it
			for (auto it = ss.begin(); it != ss.end(); ++it) {
				wchar_t c = *it;
				if (c == CHAR_ZERO) {
					vRet.push_back(false);
				}
				else {
					vRet.push_back(true);
				}
			}// it
		}//convert_to_bool_array
	}; // class VariableDesc
	///////////////////////////////////////////////
	class NumVariableDesc : public VariableDesc {
	private:
		int	_nMax;
	public:
		NumVariableDesc();
		NumVariableDesc(const StringType &sname,
			const StringType &lname = StringType(),
			const StringType &desc = StringType());
		NumVariableDesc(const NumVariableDesc &other);
		NumVariableDesc & operator=(const NumVariableDesc &other);
		virtual ~NumVariableDesc();
	public:
		int max_level(void) const;
		void max_level(int n);
		virtual VariableType variableType(void) const;
		virtual bool is_valid(void) const;
		template <class X>
		bool to_bool_array(const std::vector<X> &data, std::vector<bool> &vRet) const {
			assert(this->is_valid());
			int nMax = this->_nMax;
			this->convert_to_bool_array(data, nMax, vRet);
			return (true);
		}//to_bool_array
	}; // class NumVariableDesc
	///////////////////////////////////////////
	class OrdinalVariableDesc : public VariableDesc {
	private:
		size_t _nbModal;
	public:
		OrdinalVariableDesc();
		OrdinalVariableDesc(const size_t nb, const StringType &sname,
			const StringType &lname = StringType(),
			const StringType &desc = StringType());
		OrdinalVariableDesc(const OrdinalVariableDesc &other);
		OrdinalVariableDesc & operator=(const OrdinalVariableDesc &other);
		virtual ~OrdinalVariableDesc();
	public:
		virtual VariableType variableType(void) const;
		virtual bool is_valid(void) const;
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
		size_t modalites_count(void) const;

		void modalites_count(const size_t n);
	}; // class OrdinalVariableDesc
	////////////////////////////////////////////////
	class NominalVariableDesc : public VariableDesc {
	private:
		std::set<StringType> _oSet;
	public:
		NominalVariableDesc();
		NominalVariableDesc(const StringType &sname,
			const std::set<StringType> &oSet = std::set<StringType>(),
			const StringType &lname = StringType(),
			const StringType &desc = StringType());
		NominalVariableDesc(const NominalVariableDesc & other);
		NominalVariableDesc & operator=(const NominalVariableDesc &other);
		virtual ~NominalVariableDesc();
	public:
		virtual VariableType variableType(void) const;
		virtual bool is_valid(void) const;
		bool to_bool_array(const std::vector<StringType> &data, std::vector<bool> &vRet) const;
	public:
		void add_modalite(const StringType &s);
		void get_modalites(std::map<StringType, int> &oMap) const;
	}; // class NominalVariableDesc<S>
	//////////////////////////////////////////
	class BooleanVariableDesc : public VariableDesc {
	public:
		BooleanVariableDesc();
		BooleanVariableDesc(const StringType &sname,
			const StringType &lname = StringType(),
			const StringType &desc = StringType());
		BooleanVariableDesc(const BooleanVariableDesc &other);
		BooleanVariableDesc & operator=(const BooleanVariableDesc &other);
		virtual ~BooleanVariableDesc();
	public:
		virtual VariableType variableType(void) const;
		virtual bool is_valid(void) const;
		bool to_bool_array(const std::vector<bool> &data, std::vector<bool> &vRet) const;
	}; // class NominalVariableDesc<T,S>
	////////////////////////////////////////////////////
}// namespace info
/////////////////////////////////////
#endif // !__VARIABLE_H__
