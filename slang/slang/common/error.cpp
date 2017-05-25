#include "error.h"
#include "../common/env.h"

slang::common::error::~error(){
	if (!common::env::exiting)
		clear();
}

void slang::common::error::set(storage_entry_type *object){
	if (!SLANG_IS(common::env::runtime.state, common::env::runtime_state::error_enabled))
		return;

	clear();
	auto head = object->address_head();
	if (head != nullptr)//Increment reference count
		++head->ref_count;

	object_ = *object;
}

void slang::common::error::set(type type, storage_entry_type *object){
	if (!SLANG_IS(common::env::runtime.state, common::env::runtime_state::error_enabled))
		return;

	if (object != nullptr)
		set(object);

	type_ = type;
}

void slang::common::error::set(const char *value, bool is_runtime){
	if (!SLANG_IS(common::env::runtime.state, common::env::runtime_state::error_enabled) || has())
		return;

	auto entry = common::env::temp_storage->add(value);
	if (entry == nullptr)
		return;

	if (!is_runtime){//Return string object
		set(entry);
		return;
	}

	storage_entry_type entry_copy(entry->owner(), entry->address_value(), common::env::map_type(type_id_type::runtime_t));
	set(&entry_copy);
}

void slang::common::error::set(const char *value, size_type size){
	set(common::env::temp_storage->add(value, size));
}

void slang::common::error::set(const std::string &value, const index_type &index, bool is_runtime){
	if (!SLANG_IS(common::env::runtime.state, common::env::runtime_state::error_enabled))
		return;

	auto index_string = ("line " + std::to_string(index.line) + ", col " + std::to_string(index.column));
	set(std::string("Exception thrown at " + index_string + ": " + value).c_str(), is_runtime);
}

void slang::common::error::warn(const char *value){
	if (!SLANG_IS(common::env::runtime.state, common::env::runtime_state::error_enabled))
		return;

	if (common::env::error_writer != nullptr)
		common::env::error_writer->write(value);
}

void slang::common::error::suppress(){
	if (!SLANG_IS(common::env::runtime.state, common::env::runtime_state::error_enabled))
		return;

	if (object_.address_head() != nullptr && type_ == type::nil)
		type_ = type::suppressed;
}

void slang::common::error::unsuppress(){
	if (type_ == type::suppressed)
		type_ = type::nil;
}

void slang::common::error::clear(){
	if (!SLANG_IS(common::env::runtime.state, common::env::runtime_state::error_enabled))
		return;

	SLANG_REMOVE(common::env::runtime.state, common::env::runtime_state::error_enabled);
	auto head = object_.address_head();
	if (head != nullptr){//Decrement reference count
		common::env::address_table.deallocate(head->value);
		object_ = {};
	}

	type_ = type::nil;
	SLANG_SET(common::env::runtime.state, common::env::runtime_state::error_enabled);
}

void slang::common::error::dump(){
	if (!SLANG_IS(common::env::runtime.state, common::env::runtime_state::error_enabled))
		return;

	if (common::env::error_writer == nullptr)
		return;

	switch (type_){
	case type::return_:
		common::env::error_writer->write("Error: Invalid 'return' statement.");
		clear();
		return;
	case type::break_:
		common::env::error_writer->write("Error: Invalid 'break' statement.");
		clear();
		return;
	case type::continue_:
		common::env::error_writer->write("Error: Invalid 'continue' statement.");
		clear();
		return;
	default:
		break;
	}

	SLANG_REMOVE(common::env::runtime.state, common::env::runtime_state::error_enabled);
	auto head = object_.address_head();
	if (head != nullptr){//Echo object
		driver::object::get_driver(object_)->echo(object_, *common::env::error_writer, true);
		common::env::address_table.deallocate(head->value);
	}

	SLANG_SET(common::env::runtime.state, common::env::runtime_state::error_enabled);
}

slang::common::error::storage_entry_type *slang::common::error::get(){
	return &object_;
}

bool slang::common::error::has() const{
	return (type_ != type::suppressed && (object_.address_value() != 0u || type_ != type::nil));
}

bool slang::common::error::is_error() const{
	return (type_ == type::nil && object_.address_head() != nullptr);
}

bool slang::common::error::is_return() const{
	return (type_ == type::return_);
}

bool slang::common::error::is_break() const{
	return (type_ == type::break_);
}

bool slang::common::error::is_continue() const{
	return (type_ == type::continue_);
}

bool slang::common::error::is_suppressed() const{
	return (type_ == type::suppressed);
}
