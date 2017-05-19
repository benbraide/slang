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
	temp_storage = nullptr;
	storage::temp::address_table = &address_table;

	for (auto id = type_id_type::auto_; id < type_id_type::enum_; SLANG_INCREMENT_ENUM2(id))
		type_list[id] = std::make_shared<type::object>(id);//Primitive types

	for (auto id = type_id_type::string_; id <= type_id_type::wstring_; SLANG_INCREMENT_ENUM2(id))
		type_list[id] = std::make_shared<type::object>(id);

	indeterminate = local_temp_storage.add(type::object::bool_type::indeterminate);
	false_ = local_temp_storage.add(type::object::bool_type::false_);
	true_ = local_temp_storage.add(type::object::bool_type::true_);

	nullptr_ = local_temp_storage.add(nullptr);
	nan = local_temp_storage.nan();
}

slang::type::object::ptr_type slang::common::env::map_type(type::object::id_type id){
	auto entry = type_list.find(id);
	return (entry == type_list.end()) ? nullptr : entry->second;
}
