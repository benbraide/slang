#include "env.h"

slang::address::table slang::common::env::address_table;

slang::storage::named slang::common::env::global_storage("");

slang::storage::temp slang::common::env::local_temp_storage;

thread_local slang::storage::temp *slang::common::env::temp_storage;

slang::common::env::type_list_type slang::common::env::type_list;

slang::storage::entry *slang::common::env::indeterminate;

slang::storage::entry *slang::common::env::false_;

slang::storage::entry *slang::common::env::true_;

slang::storage::entry *slang::common::env::nullptr_;

slang::storage::entry *slang::common::env::nan;

void slang::common::env::bootstrap(){
	std::call_once(once_flag_, &env::bootstrap_);
}

slang::type::object::ptr_type slang::common::env::map_type(type::object::id_type id){
	auto entry = type_list.find(id);
	return (entry == type_list.end()) ? nullptr : entry->second;
}

void slang::common::env::bootstrap_(){
	temp_storage = nullptr;
	storage::temp::address_table = &address_table;

	for (auto id = type_id_type::auto_; id < type_id_type::enum_; SLANG_INCREMENT_ENUM2(id))
		type_list[id] = std::make_shared<type::object>(id);//Primitive types

	type_ptr_type ptr;
	auto key = type_id_type::wchar;

	type_list[type_id_type::nan] = std::make_shared<type::modified>(type_list[type_id_type::int_], type_attribute_type::nan);
	for (auto id = type_id_type::wstring_; id <= type_id_type::string_; SLANG_INCREMENT_ENUM2(id)){
		ptr = std::make_shared<type::pointer>(type_list[key]);
		type_list[id] = std::make_shared<type::modified>(ptr, type_attribute_type::const_);
		SLANG_INCREMENT_ENUM2(key);
	}

	indeterminate	= local_temp_storage.add(type::object::bool_type::indeterminate);
	false_			= local_temp_storage.add(type::object::bool_type::false_);
	true_			= local_temp_storage.add(type::object::bool_type::true_);

	nullptr_		= local_temp_storage.add(nullptr);
	nan				= local_temp_storage.nan();
}

std::once_flag slang::common::env::once_flag_;
