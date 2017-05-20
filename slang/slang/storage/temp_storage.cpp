#include "temp_storage.h"
#include "../common/env.h"

slang::storage::temp::temp()
	: previous_temp_(common::env::temp_storage){
	common::env::temp_storage = this;
	nullptr_ = address_head_type{
		1u,
		0ull,
		0u,
	};;
}

slang::storage::temp::~temp(){
	if (common::env::temp_storage == this)//Restore previous
		common::env::temp_storage = previous_temp_;

	for (auto &entry : value_list_)
		clean_(entry);
}

slang::storage::entry *slang::storage::temp::add(size_type size, type_ptr_type type){
	auto head = address_table->allocate(size);
	if (head == nullptr)//Failed to allocate memory
		return nullptr;

	return add(*head, type);
}

slang::storage::entry *slang::storage::temp::add(address_head_type &head, type_ptr_type type){
	return value_list_.emplace(value_list_.end(), entry(nullptr, &head, type))->object();
}

slang::storage::entry *slang::storage::temp::add(const entry &entry){
	return value_list_.emplace(value_list_.end(), entry)->object();
}

slang::storage::entry *slang::storage::temp::add(const char *value, size_type size){
	auto head = address_table->allocate_scalar_cstr(value, size);
	if (head == nullptr)//Failed to allocate memory
		return nullptr;

	auto ptr_head = address_table->allocate_scalar(head->value);
	if (ptr_head == nullptr){//Failed to allocate memory
		address_table->deallocate(head->value);
		return nullptr;
	}

	SLANG_SET(ptr_head->attributes, address::table::attribute_type::is_string);
	return add(*ptr_head, common::env::type_list[type::object::id_type::string_]);
}

slang::storage::entry *slang::storage::temp::add(const wchar_t *value, size_type size){
	auto head = address_table->allocate_scalar_wcstr(value, size);
	if (head == nullptr)//Failed to allocate memory
		return nullptr;

	auto ptr_head = address_table->allocate_scalar(head->value);
	if (ptr_head == nullptr){//Failed to allocate memory
		address_table->deallocate(head->value);
		return nullptr;
	}

	SLANG_SET(ptr_head->attributes, address::table::attribute_type::is_string);
	return add(*ptr_head, common::env::type_list[type::object::id_type::wstring_]);
}

slang::storage::entry *slang::storage::temp::add(std::nullptr_t){
	return add(nullptr_, common::env::type_list[type::object::id_type::nullptr_]);
}

slang::storage::entry *slang::storage::temp::nan(){
	auto head = address_table->allocate(0u);
	if (head == nullptr)//Failed to allocate memory
		return nullptr;

	SLANG_REMOVE(head->attributes, attribute_type::uninitialized);
	SLANG_SET(head->attributes, attribute_type::is_nan | attribute_type::write_protect);

	return add(*head, common::env::type_list[type::object::id_type::int_]);
}

slang::address::table *slang::storage::temp::address_table = nullptr;

void slang::storage::temp::clean_(value_type &value){
	if (!value.is_object())
		return;

	auto head = value.object()->address_head();
	if (head != nullptr)//Free memory
		common::env::address_table.deallocate(head->value);
}
