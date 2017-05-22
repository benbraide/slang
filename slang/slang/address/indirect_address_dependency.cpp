#include "indirect_address_dependency.h"

slang::address::indirect_dependency::indirect_dependency(const entry_type &value, dependency_type previous)
	: value_(value), previous_(previous){}

slang::address::indirect_dependency::~indirect_dependency() = default;

void slang::address::indirect_dependency::set_value(const entry_type &value){
	value_ = value;
}

slang::address::indirect_dependency::entry_type &slang::address::indirect_dependency::value(){
	return value_;
}

slang::address::indirect_dependency::dependency_type slang::address::indirect_dependency::previous() const{
	return previous_;
}
