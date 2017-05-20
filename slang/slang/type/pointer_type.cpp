#include "pointer_type.h"

#include "../address/address_table.h"
#include "../storage/storage_entry.h"

slang::type::pointer::pointer(ptr_type underlying_type)
	: underlying_type_(underlying_type), name_("pointer_t<" + underlying_type->name() + ">"){}

slang::type::pointer::~pointer() = default;

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

int slang::type::pointer::score(const object *type) const{
	auto value = object::score(type);
	if (value != SLANG_MIN_TYPE_SCORE)
		return value;

	if (!type->is_pointer())
		return type->is_nullptr() ? (SLANG_MAX_TYPE_SCORE - 2) : SLANG_MIN_TYPE_SCORE;

	if (type->is_dynamic())
		return (SLANG_MAX_TYPE_SCORE - 2);

	auto underlying_type_value = underlying_type_->score(type->remove_pointer());
	return (underlying_type_value >= (SLANG_MAX_TYPE_SCORE - 2)) ? underlying_type_value : SLANG_MIN_TYPE_SCORE;
}

int slang::type::pointer::score(const storage::entry &entry) const{
	auto head = entry.address_head();
	if (head == nullptr)
		return score(entry.type().get());

	auto type = entry.type().get();
	auto value = object::score(type);

	if (value != SLANG_MIN_TYPE_SCORE)
		return value;

	if (!type->is_pointer()){
		if (type->is_nullptr())
			return (SLANG_MAX_TYPE_SCORE - 2);

		if (!type->is_array())
			return SLANG_MIN_TYPE_SCORE;

		if (type->is_dynamic())
			return (SLANG_MAX_TYPE_SCORE - 2);

		auto underlying_type_value = underlying_type_->score(type->remove_array());
		return (underlying_type_value >= (SLANG_MAX_TYPE_SCORE - 2)) ? underlying_type_value : SLANG_MIN_TYPE_SCORE;
	}

	if (type->is_dynamic())
		return (SLANG_MAX_TYPE_SCORE - 2);

	auto underlying_type_value = underlying_type_->score(type->remove_pointer());
	return (underlying_type_value >= (SLANG_MAX_TYPE_SCORE - 2)) ? underlying_type_value : SLANG_MIN_TYPE_SCORE;
}

slang::type::object::id_type slang::type::pointer::id() const{
	return id_type::pointer;
}

bool slang::type::pointer::is_pointer() const{
	return true;
}
