#include "modified_type.h"

slang::type::modified::modified(ptr_type underlying_type, attribute attributes)
	: object(underlying_type->id(), attributes | attribute::modified, underlying_type->name(), underlying_type->size()),
	underlying_type_(underlying_type){
	if (SLANG_IS(attributes_, attribute::nan))
		name_ = "nan_t";
}

slang::type::modified::~modified() = default;

slang::type::object *slang::type::modified::underlying_type(){
	return underlying_type_.get();
}

std::string slang::type::modified::print() const{
	if (SLANG_IS(attributes_, attribute::nan))
		return "nan_t";

	std::string prefix;
	if (SLANG_IS(attributes_, attribute::const_))
		prefix = "const ";

	if (SLANG_IS(attributes_, attribute::ref))
		prefix += "ref ";

	return (prefix + underlying_type_->print());
}
