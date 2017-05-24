#include "storage_entry.h"
#include "storage_object.h"

#include "../common/env.h"

slang::storage::entry::entry()
	: owner_(nullptr), address_value_(static_cast<uint64_type>(0)), attributes_(attribute_type::nil){}

slang::storage::entry::entry(object *owner, uint64_type address_value, type_ptr_type type, attribute_type attributes)
	: owner_(owner), address_value_(address_value), type_(type), attributes_(attributes){}

slang::storage::object *slang::storage::entry::owner() const{
	return owner_;
}

void slang::storage::entry::add_attributes(attribute_type value, bool replace){
	if (replace)
		attributes_ = value;
	else//Combine
		SLANG_SET(attributes_, value);
}

void slang::storage::entry::remove_attributes(attribute_type value){
	SLANG_REMOVE(attributes_, value);
}

slang::storage::entry::uint64_type slang::storage::entry::address_value() const{
	return address_value_;
}

slang::storage::entry::address_head_type *slang::storage::entry::address_head() const{
	if (SLANG_IS(attributes_, attribute_type::block_aligned))
		return common::env::address_table.find(address_value_);

	auto head = common::env::address_table.get_head(address_value_);
	if (head == nullptr || head->value == address_value_)
		return nullptr;

	auto offset = (address_value_ - head->value);
	address_head_ = *head;
	{//Apply offset
		address_head_.value += offset;
		address_head_.size -= static_cast<uint_type>(offset);
		address_head_.actual_size -= static_cast<uint_type>(offset);
		address_head_.ptr += offset;
	}

	return &address_head_;
}

slang::storage::entry::address_head_type *slang::storage::entry::cached_address_head() const{
	return &address_head_;
}

slang::storage::entry::type_ptr_type slang::storage::entry::type() const{
	return type_;
}

slang::storage::entry::attribute_type slang::storage::entry::attributes() const{
	return attributes_;
}

bool slang::storage::entry::is(attribute_type attribute) const{
	return SLANG_IS(attributes_, attribute);
}

bool slang::storage::entry::is_any_of(attribute_type attributes) const{
	return SLANG_IS_ANY(attributes_, attributes);
}

bool slang::storage::entry::is_lval() const{
	return SLANG_IS(attributes_, attribute_type::lval);
}

bool slang::storage::entry::is_ref() const{
	return SLANG_IS(attributes_, attribute_type::ref_);
}

bool slang::storage::entry::is_rval_ref() const{
	return SLANG_IS(attributes_, attribute_type::rval_ref);
}

bool slang::storage::entry::is_const() const{
	return SLANG_IS(attributes_, attribute_type::const_);
}

bool slang::storage::entry::is_uninitialized() const{
	return SLANG_IS(attributes_, attribute_type::uninitialized);
}

bool slang::storage::entry::is_void() const{
	return SLANG_IS(attributes_, attribute_type::void_);
}

bool slang::storage::entry::is_const_pointer() const{
	return SLANG_IS(attributes_, attribute_type::const_pointer);
}
