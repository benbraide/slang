#include "named_storage.h"

slang::storage::named::named(const std::string &name)
	: name_(name){}

slang::storage::named::named(const std::string &name, object &parent)
	: object(parent), name_(name){}

slang::storage::named::~named() = default;

slang::storage::object *slang::storage::named::match(const std::string &name){
	return (name_ == name) ? this : object::match(name);
}

const std::string &slang::storage::named::name() const{
	return name_;
}

std::string slang::storage::named::print() const{
	if (parent_ == nullptr)
		return name_;

	auto named_parent = dynamic_cast<named *>(parent_);
	return (named_parent == nullptr) ? name_ : (named_parent->print() + "::" + name_);
}
