#include "pointer_type.h"
#include "../common/env.h"

slang::type::pointer::pointer(ptr_type underlying_type)
	: underlying_type_(underlying_type), name_("pointer_t<" + underlying_type->name() + ">"){}

slang::type::pointer::~pointer() = default;

slang::type::object::driver_object_type *slang::type::pointer::driver() const{
	return &common::env::pointer_driver;
}

slang::type::object *slang::type::pointer::underlying_type() const{
	return underlying_type_.get();
}

slang::type::object *slang::type::pointer::remove_pointer() const{
	return underlying_type_.get();
}

const std::string &slang::type::pointer::name() const{
	return name_;
}

std::string slang::type::pointer::print() const{
	return ("pointer_t<" + underlying_type_->print() + ">");
}

slang::type::object::size_type slang::type::pointer::size() const{
	return static_cast<size_type>(sizeof(address::table::uint64_type));
}

int slang::type::pointer::score(const object *type, bool is_entry, bool check_const) const{
	auto value = object::score(type);
	if (value != SLANG_MIN_TYPE_SCORE)
		return value;

	if (!type->is_pointer()){
		if (type->is_nullptr())
			return (SLANG_MAX_TYPE_SCORE - 2);

		if (!is_entry || !type->is_array())
			return SLANG_MIN_TYPE_SCORE;

		auto underlying_type_value = underlying_type_->score(type->remove_array(), true, true);
		return (underlying_type_value >= (SLANG_MAX_TYPE_SCORE - 2)) ? underlying_type_value : SLANG_MIN_TYPE_SCORE;
	}

	if (type->is_dynamic())
		return (SLANG_MAX_TYPE_SCORE - 2);

	auto underlying_type_value = underlying_type_->score(type->remove_pointer(), is_entry, true);
	return (underlying_type_value >= (SLANG_MAX_TYPE_SCORE - 2)) ? underlying_type_value : SLANG_MIN_TYPE_SCORE;
}

slang::type::object::id_type slang::type::pointer::id() const{
	return id_type::pointer;
}

bool slang::type::pointer::is_dynamic() const{
	return false;
}

bool slang::type::pointer::is_pointer() const{
	return true;
}

bool slang::type::pointer::is_strong_pointer() const{
	return true;
}

bool slang::type::pointer::is_string() const{
	return underlying_type_->is(id_type::char_);
}

bool slang::type::pointer::is_const_string() const{
	return (underlying_type_->is_const() && is_string());
}

bool slang::type::pointer::is_wstring() const{
	return underlying_type_->is(id_type::wchar);
}

bool slang::type::pointer::is_const_wstring() const{
	return (underlying_type_->is_const() && is_wstring());
}

bool slang::type::pointer::is_const_target() const{
	return underlying_type_->is_const();
}
