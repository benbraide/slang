#include "env.h"

thread_local slang::common::env::runtime_info slang::common::env::runtime{};

bool slang::common::env::exiting = false;

slang::utilities::thread_pool slang::common::env::thread_pool(9, 18);

thread_local slang::common::error slang::common::env::error;

slang::address::table slang::common::env::address_table;

slang::storage::named slang::common::env::global_storage("");

slang::storage::temp slang::common::env::local_temp_storage;

thread_local slang::storage::temp *slang::common::env::temp_storage;

slang::common::env::type_list_type slang::common::env::type_list;

slang::common::env::output_writer_type slang::common::env::default_output_writer(std::cout, std::wcout);

slang::common::env::output_writer_type slang::common::env::default_error_writer(std::cerr, std::wcerr);

slang::common::output_writer_interface *slang::common::env::out_writer = &default_output_writer;

slang::common::output_writer_interface *slang::common::env::error_writer = &default_error_writer;

slang::storage::entry *slang::common::env::indeterminate;

slang::storage::entry *slang::common::env::false_;

slang::storage::entry *slang::common::env::true_;

slang::storage::entry *slang::common::env::nullptr_;

slang::storage::entry *slang::common::env::nan;

slang::driver::numeric slang::common::env::numeric_driver;

slang::driver::pointer slang::common::env::pointer_driver;

void slang::common::env::bootstrap(){
	std::call_once(once_flag_, &env::bootstrap_);
}

void slang::common::env::tear_down(){
	exiting = true;
}

slang::type::object::ptr_type slang::common::env::map_type(type::object::id_type id){
	auto entry = type_list.find(id);
	return (entry == type_list.end()) ? nullptr : entry->second;
}

void slang::common::env::bootstrap_(){
	temp_storage = nullptr;
	runtime.state = runtime_state::error_enabled;

	storage::temp::address_table = &address_table;
	for (auto id = type_id_type::auto_; id < type_id_type::enum_; SLANG_INCREMENT_ENUM2(id))
		type_list[id] = std::make_shared<type::primitive>(id);//Primitive types

	type_ptr_type ptr;
	auto key = type_id_type::wchar;

	for (auto id = type_id_type::wstring_; id <= type_id_type::string_; SLANG_INCREMENT_ENUM2(id)){
		ptr = std::make_shared<type::modified>(type_list[key], type_attribute_type::const_);
		ptr = std::make_shared<type::pointer>(ptr);
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
