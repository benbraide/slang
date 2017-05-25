#include "array_type.h"
#include "../common/env.h"

slang::type::array_type::array_type(ptr_type underlying_type)
	: underlying_type_(underlying_type), name_("array<" + underlying_type->name() + ">"){}

slang::type::array_type::~array_type() = default;

slang::type::object::driver_object_type *slang::type::array_type::driver() const{
	return nullptr;
}

slang::type::object *slang::type::array_type::underlying_type() const{
	return underlying_type_.get();
}

slang::type::object *slang::type::array_type::remove_array() const{
	return underlying_type_.get();
}

const std::string &slang::type::array_type::name() const{
	return name_;
}

std::string slang::type::array_type::print() const{
	return ("array<" + underlying_type_->print() + ">");
}

slang::type::object::size_type slang::type::array_type::size() const{
	return static_cast<size_type>(sizeof(address::table::uint64_type));
}

int slang::type::array_type::score(const object *type, bool is_entry, bool check_const) const{
	auto value = object::score(type, is_entry);
	if (value != SLANG_MIN_TYPE_SCORE)
		return value;

	if (!type->is_array())
		return SLANG_MIN_TYPE_SCORE;

	if (!type->is_strong_array())
		return (SLANG_MAX_TYPE_SCORE - 2);

	auto underlying_type_value = underlying_type_->score(type->remove_array(), is_entry, true);
	return (underlying_type_value >= (SLANG_MAX_TYPE_SCORE - 2)) ? underlying_type_value : SLANG_MIN_TYPE_SCORE;
}

slang::type::object::id_type slang::type::array_type::id() const{
	return id_type::array_;
}

bool slang::type::array_type::is_array() const{
	return true;
}

bool slang::type::array_type::is_strong_array() const{
	return true;
}

bool slang::type::array_type::is_const_target() const{
	return underlying_type_->is_const();
}
