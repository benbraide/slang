#include "pointer_type.h"
#include "../address/address_table.h"

slang::type::pointer::pointer(ptr_type underlying_type)
	: object(id_type::pointer, attribute::pointer, "", 0u), underlying_type_(underlying_type){
	name_ = ("pointer_t<" + underlying_type->name() + ">");
	size_ = static_cast<size_type>(sizeof(address::table::uint64_type));
}

slang::type::pointer::~pointer() = default;

slang::type::object *slang::type::pointer::underlying_type(){
	return underlying_type_.get();
}

std::string slang::type::pointer::print() const{
	return ("pointer_t<" + underlying_type_->print() + ">");
}
