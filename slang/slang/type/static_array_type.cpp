#include "static_array_type.h"

slang::type::static_array::static_array(ptr_type underlying_type, size_type count)
	: array_type(underlying_type), count_(count){
	name_ = (underlying_type_->name() + "[" + std::to_string(count_) + "]");
}

slang::type::static_array::~static_array() = default;

slang::type::object::driver_object_type *slang::type::static_array::driver() const{
	return nullptr;
}

std::string slang::type::static_array::print() const{
	return (underlying_type_->print() + "[" + std::to_string(count_) + "]");
}

slang::type::object::size_type slang::type::static_array::size() const{
	return (underlying_type_->size() * count_);
}

bool slang::type::static_array::is_dynamic() const{
	return false;
}

bool slang::type::static_array::is_static_array() const{
	return true;
}

slang::type::object::size_type slang::type::static_array::count() const{
	return count_;
}
