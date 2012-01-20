#ifndef ROXLU_TWITTER_PARAMETERH
#define ROXLU_TWITTER_PARAMETERH

#include <string>
#include <sstream>

namespace roxlu {
namespace twitter {
namespace parameter {

/**
 * Request GET/POST value. By default is a parameter a simple key=value pair
 * where value is a string. More advanced value types are types like Files, 
 * which can be used to simulate a file upload with CURL.
 *
 */ 
class Parameter {
public:	
	enum Types {
		 PARAM_STRING = 1
		,PARAM_FILE = 2	
	};
	
	Parameter();
	Parameter(const std::string& n, const std::string& v);
	Parameter(Parameter* other);
	~Parameter();
	
	virtual std::string& getName();
	virtual std::string& getValue();
	virtual void setName(const std::string& n);
	virtual void setValue(const std::string& v);
	virtual std::string getStringValue();
	virtual bool mustUseInSignature();
	
	// pass in "any" value and convert it to string.
	template<typename T>
	void setStringValue(const T& v) {
		std::stringstream ss;
		ss << v;
		setValue(ss.str());
	}
	
	template<typename T>
	Parameter& operator=(const T& v) {
		setStringValue(v);
		return *this;
	}
	
	bool operator==(const Parameter& other) const;
	void print();
	int type;
	
protected:
	std::string name;
	std::string value;
};


inline std::string& Parameter::getName() {
	return name;
}

inline std::string& Parameter::getValue() {
	return value;
}



inline void Parameter::setName(const std::string& n) {
	name = n;
}

inline void Parameter::setValue(const std::string& v) {
	value = v;
}

inline std::string Parameter::getStringValue() {
	return value;
}

inline bool Parameter::operator==(const Parameter& other) const {
	return (name == other.name && value == other.value);
}




}}} // roxlu::twitter::parameter
#endif