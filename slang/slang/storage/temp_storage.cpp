#include "temp_storage.h"
#include "../common/env.h"

slang::storage::temp::temp()
	: previous_temp_(common::env::temp_storage){
	common::env::temp_storage = this;
}

slang::storage::temp::~temp(){
	if (!common::env::exiting){//Perform clean up
		if (common::env::temp_storage == this)//Restore previous
			common::env::temp_storage = previous_temp_;

		for (auto &entry : value_list_)
			clean_(entry);
	}
}

slang::storage::entry *slang::storage::temp::add(size_type size, type_ptr_type type, attribute_type attributes){
	auto head = address_table->allocate(size);
	if (head == nullptr)//Failed to allocate memory
		return nullptr;

	return wrap(head->value, type, attributes | attribute_type::block_aligned);
}

slang::storage::entry *slang::storage::temp::wrap(uint64_type value, type_ptr_type type, attribute_type attributes){
	return value_list_.emplace(value_list_.end(), entry(nullptr, value, type, attributes))->object();
}

slang::storage::entry *slang::storage::temp::wrap(const entry &entry){
	return value_list_.emplace(value_list_.end(), entry)->object();
}

slang::storage::entry *slang::storage::temp::add(const char *value, size_type size){
	auto head = address_table->allocate_scalar_cstr(value, size);
	if (head == nullptr)//Failed to allocate memory
		return nullptr;

	return add_string_(*head, type_id_type::string_);
}

slang::storage::entry *slang::storage::temp::add(const wchar_t *value, size_type size){
	auto head = address_table->allocate_scalar_wcstr(value, size);
	if (head == nullptr)//Failed to allocate memory
		return nullptr;

	return add_string_(*head, type_id_type::wstring_);
}

slang::storage::entry *slang::storage::temp::add(type_object_type &value){
	auto head = address_table->allocate_scalar(*reinterpret_cast<uint64_type *>(&value));
	if (head == nullptr)//Failed to allocate memory
		return nullptr;

	address_table->set_dependency<address::generic_dependency<type_ptr_type>>(head->value, value.reflect());
	return wrap(head->value, common::env::type_list[type::object::id_type::type_], attribute_type::block_aligned);
}

slang::storage::entry *slang::storage::temp::add(std::nullptr_t){
	return wrap(0u, common::env::type_list[type::object::id_type::nullptr_], attribute_type::block_aligned);
}

slang::storage::entry *slang::storage::temp::add_pointer(entry &value){
	auto head = address_table->allocate_scalar(value.address_head()->value);
	if (head == nullptr)//Failed to allocate memory
		return nullptr;

	auto type = value.type();
	auto new_type = type->remove_modified()->reflect();

	auto attributes = attribute_type::nil;
	if (value.is(attribute_type::const_) || type->is_const()){//pointer to const value
		new_type = std::make_shared<type::modified>(new_type, type_attribute_type::const_);
		attributes = attribute_type::const_pointer;
	}

	return wrap(head->value, std::make_shared<type::pointer>(new_type), attributes | attribute_type::block_aligned);
}

slang::storage::entry *slang::storage::temp::add_pointer(uint64_type value, type_ptr_type type){
	auto head = address_table->allocate_scalar(value);
	if (head == nullptr)//Failed to allocate memory
		return nullptr;

	return wrap(head->value, type, attribute_type::block_aligned);
}

slang::storage::entry *slang::storage::temp::nan(){
	auto head = address_table->allocate(0u);
	if (head == nullptr)//Failed to allocate memory
		return nullptr;

	SLANG_SET(head->attributes, address::table::attribute_type::write_protect);
	return wrap(head->value, common::env::type_list[type::object::id_type::nan], attribute_type::nan_ | attribute_type::block_aligned);
}

slang::address::table *slang::storage::temp::address_table = nullptr;

slang::storage::entry *slang::storage::temp::add_string_(address_head_type &head, type_id_type id){
	auto ptr_head = address_table->allocate_scalar(head.value);
	if (ptr_head == nullptr){//Failed to allocate memory
		address_table->deallocate(head.value);
		return nullptr;
	}

	SLANG_SET(ptr_head->attributes, address::table::attribute_type::is_string);
	return wrap(ptr_head->value, common::env::type_list[id], attribute_type::const_pointer | attribute_type::block_aligned);
}

void slang::storage::temp::clean_(value_type &value){
	if (!value.is_object())
		return;

	auto head = value.object()->address_head();
	if (head != nullptr && head->value != 0u)//Free memory
		address_table->deallocate(head->value);
}
