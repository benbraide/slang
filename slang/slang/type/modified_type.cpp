#include "modified_type.h"
#include "../common/env.h"

slang::type::modified::modified(ptr_type underlying_type, attribute attributes)
	: underlying_type_(underlying_type), attributes_(attributes){}

slang::type::modified::~modified() = default;

slang::type::object::driver_object_type *slang::type::modified::driver() const{
	if (is_ref())
		return &common::env::ref_driver;
	return underlying_type_->driver();
}

slang::type::object *slang::type::modified::underlying_type() const{
	return underlying_type_.get();
}

slang::type::object *slang::type::modified::remove_modified() const{
	return underlying_type_->remove_modified();
}

slang::type::object *slang::type::modified::remove_pointer() const{
	return underlying_type_->remove_pointer();
}

slang::type::object *slang::type::modified::remove_array() const{
	return underlying_type_->remove_array();
}

slang::type::object *slang::type::modified::remove_variadic() const{
	return underlying_type_->remove_variadic();
}

const std::string &slang::type::modified::name() const{
	return underlying_type_->name();
}

std::string slang::type::modified::print() const{
	std::string prefix;
	if (SLANG_IS(attributes_, attribute::const_))
		prefix = "const ";

	if (SLANG_IS(attributes_, attribute::ref))
		prefix += "ref ";
	else if (SLANG_IS(attributes_, attribute::rval))
		prefix += "ref! ";

	return (prefix + underlying_type_->print());
}

slang::type::object::size_type slang::type::modified::size() const{
	return (is_ref() && !underlying_type_->is_static_array()) ? static_cast<size_type>(sizeof(address::table::uint64_type)) :
		underlying_type_->size();
}

int slang::type::modified::score(const object *type, bool is_entry, bool check_const) const{
	if (!is_entry && (is_const() != type->is_const() || is_ref() != type->is_ref() || is_rval_ref() != type->is_rval_ref()))
		return SLANG_MIN_TYPE_SCORE;//Specificity must match

	if (is_entry && check_const && type->is_const() && !is_const())
		return SLANG_MIN_TYPE_SCORE;//Constness must match

	auto value = underlying_type_->score(type, is_entry, check_const);
	if (value == SLANG_MIN_TYPE_SCORE)
		return value;

	return (!is_explicit() || value >= (SLANG_MAX_TYPE_SCORE - 2)) ? value : SLANG_MIN_TYPE_SCORE;
}

int slang::type::modified::score(const storage::entry &entry) const{
	auto head = entry.address_head();
	if (head == nullptr)
		return score(entry.type().get());

	auto is_const = this->is_const();
	if (is_ref()){
		if (entry.owner() == nullptr && !entry.is_lval()){//rvalue
			if (!is_const)//Constant reference is required for rvalue
				return SLANG_MIN_TYPE_SCORE;
		}

		if (!is_const && entry.is_const())
			return SLANG_MIN_TYPE_SCORE;//Constness mismatch
	}

	if (!is_const && entry.is_const() && is_rval_ref())
		return SLANG_MIN_TYPE_SCORE;//Constness mismatch

	auto value = underlying_type_->score(entry.type()->remove_modified());
	if (value == SLANG_MIN_TYPE_SCORE)
		return value;

	return (!is_explicit() || value >= (SLANG_MAX_TYPE_SCORE - 2)) ? value : SLANG_MIN_TYPE_SCORE;
}

slang::type::object::attribute slang::type::modified::attributes() const{
	return attributes_;
}

slang::type::object::id_type slang::type::modified::id() const{
	return underlying_type_->id();
}

bool slang::type::modified::is(id_type id) const{
	return underlying_type_->is(id);
}

bool slang::type::modified::is_dynamic() const{
	return underlying_type_->is_dynamic();
}

bool slang::type::modified::is_strong_pointer() const{
	return underlying_type_->is_strong_pointer();
}

bool slang::type::modified::is_string() const{
	return underlying_type_->is_string();
}

bool slang::type::modified::is_const_string() const{
	return underlying_type_->is_const_string();
}

bool slang::type::modified::is_wstring() const{
	return underlying_type_->is_wstring();
}

bool slang::type::modified::is_const_wstring() const{
	return underlying_type_->is_const_wstring();
}

bool slang::type::modified::is_strong_array() const{
	return underlying_type_->is_strong_array();
}

bool slang::type::modified::is_static_array() const{
	return underlying_type_->is_static_array();
}

bool slang::type::modified::is_strong_function() const{
	return underlying_type_->is_strong_function();
}

bool slang::type::modified::is_const_target() const{
	return underlying_type_->is_const_target();
}

bool slang::type::modified::is_modified() const{
	return true;
}
