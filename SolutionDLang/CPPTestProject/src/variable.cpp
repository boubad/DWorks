// variable.cpp
#include "variable.h"
/////////////////////////////
namespace info {
	/////////////////////////////////
	VariableDesc::~VariableDesc() {}
	const StringType & VariableDesc::shortName(void) const {
		return (this->_shortName);
	}
	void VariableDesc::shortName(const StringType &s) {
		this->_shortName = s;
	}
	const StringType & VariableDesc::longName(void) const {
		return (this->_longName);
	}
	void VariableDesc::longName(const StringType &s) {
		this->_longName = s;
	}
	const StringType & VariableDesc::description(void) const {
		return (this->_description);
	}
	void VariableDesc::description(const StringType &s) {
		this->_description = s;
	}
	VariableDesc::VariableDesc() {}
	VariableDesc::VariableDesc(const StringType &sname,
		const StringType &lname /*= StringType()*/,
		const StringType &desc /*= StringType()*/) :_shortName(sname), _longName(lname), _description(desc) {}
	VariableDesc::VariableDesc(const VariableDesc &other) : _shortName(other._shortName), _longName(other._longName),
		_description(other._description) {}
	VariableDesc & VariableDesc::operator=(const VariableDesc &other) {
		if (this != &other) {
			this->_shortName = other._shortName;
			this->_longName = other._longName;
			this->_description = other._description;
		}
		return (*this);
	}
	///////////////////////////////////////////////
	NumVariableDesc::NumVariableDesc() :_nMax(7) {}
	NumVariableDesc::NumVariableDesc(const StringType &sname,
		const StringType &lname /*= StringType()*/,
		const StringType &desc /*= StringType()*/) : VariableDesc(sname, lname, desc), _nMax(7) {}
	NumVariableDesc::NumVariableDesc(const NumVariableDesc &other) : VariableDesc(other), _nMax(other._nMax) {}
	NumVariableDesc & NumVariableDesc::operator=(const NumVariableDesc &other) {
		if (this != &other) {
			VariableDesc::operator=(other);
			this->_nMax = other._nMax;
		}
		return (*this);
	}
	NumVariableDesc::~NumVariableDesc() {}
	int NumVariableDesc::max_level(void) const {
		return (this->_nMax);
	}
	void NumVariableDesc::max_level(int n) {
		if (n > 1) {
			this->_nMax = n;
		}
	}
	VariableType NumVariableDesc::variableType(void) const {
		return (VariableType::numType);
	}
	bool NumVariableDesc::is_valid(void) const {
		return (this->_nMax > 1);
	}
	///////////////////////////////////////////
	OrdinalVariableDesc::OrdinalVariableDesc() :_nbModal(0) {}
	OrdinalVariableDesc::OrdinalVariableDesc(const size_t nb, const StringType &sname,
		const StringType &lname /* = StringType()*/,
		const StringType &desc /*= StringType()*/) : VariableDesc(sname, lname, desc), _nbModal(nb) {
		assert(_nbModal > 0);
	}
	OrdinalVariableDesc::OrdinalVariableDesc(const OrdinalVariableDesc &other) : VariableDesc(other), _nbModal(other._nbModal) {
	}
	OrdinalVariableDesc & OrdinalVariableDesc::operator=(const OrdinalVariableDesc &other) {
		if (this != &other) {
			VariableDesc::operator=(other);
			this->_nbModal = other._nbModal;
		}
		return (*this);
	}
	OrdinalVariableDesc::~OrdinalVariableDesc() {}
	VariableType OrdinalVariableDesc::variableType(void) const {
		return (VariableType::ordinalType);
	}
	bool OrdinalVariableDesc::is_valid(void) const {
		return (this->_nbModal > 0);
	}
	size_t OrdinalVariableDesc::modalites_count(void) const {
		return (this->_nbModal);
	}
	void OrdinalVariableDesc::modalites_count(const size_t n) {
		this->_nbModal = n;
	}
	////////////////////////////////////////////////

	NominalVariableDesc::NominalVariableDesc() {}
	NominalVariableDesc::NominalVariableDesc(const StringType &sname,
		const std::set<StringType> &oSet /*= std::set<StringType>()*/,
		const StringType &lname /*= StringType()*/,
		const StringType &desc /*= StringType()*/) : VariableDesc(sname, lname, desc), _oSet(oSet) {}
	NominalVariableDesc::NominalVariableDesc(const NominalVariableDesc &other) : VariableDesc(other), _oSet(other._oSet) {}
	NominalVariableDesc & NominalVariableDesc::operator=(const NominalVariableDesc &other) {
		if (this != &other) {
			VariableDesc::operator=(other);
			this->_oSet = other._oSet;
		}
		return (*this);
	}
	NominalVariableDesc::~NominalVariableDesc() {}
	VariableType NominalVariableDesc::variableType(void) const {
		return (VariableType::nominalType);
	}
	bool NominalVariableDesc::is_valid(void) const {
		return (!this->_oSet.empty());
	}
	bool NominalVariableDesc::to_bool_array(const std::vector<StringType> &data, std::vector<bool> &vRet) const {
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
		VariableDesc::convert_to_bool_array(vals, nMax, vRet);
		return (true);
	}//to_bool_array
	void NominalVariableDesc::add_modalite(const StringType &s) {
		this->_oSet.insert(s);
	}
	void NominalVariableDesc::get_modalites(std::map<StringType, int> &oMap) const {
		oMap.clear();
		const std::set<StringType> &oSet = this->_oSet;
		int val = 0;
		for (auto it = oSet.begin(); it != oSet.end(); ++it) {
			StringType key = *it;
			oMap[key] = val++;
		}
	}// get_modalites
   //////////////////////////////////////////
	BooleanVariableDesc::BooleanVariableDesc() {}
	BooleanVariableDesc::BooleanVariableDesc(const StringType &sname,
		const StringType &lname /*= StringType()*/,
		const StringType &desc /*= StringType()*/) : VariableDesc(sname, lname, desc) {}
	BooleanVariableDesc::BooleanVariableDesc(const BooleanVariableDesc &other) : VariableDesc(other) {}
	BooleanVariableDesc & BooleanVariableDesc::operator=(const BooleanVariableDesc &other) {
		if (this != &other) {
			VariableDesc::operator=(other);
		}
		return (*this);
	}
	BooleanVariableDesc::~BooleanVariableDesc() {}
	VariableType BooleanVariableDesc::variableType(void) const {
		return (VariableType::booleanType);
	}
	bool BooleanVariableDesc::is_valid(void) const {
		return (true);
	}
	bool BooleanVariableDesc::to_bool_array(const std::vector<bool> &data, std::vector<bool> &vRet) const {
		vRet = data;
		return (true);
	}//to_bool_array
////////////////////////////////////
}// namespace info
//////////////////////////////
//eof: variable.cpp
