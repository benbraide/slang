#include "error.h"
#include "../common/env.h"

slang::common::error::~error(){
	clear();
}

void slang::common::error::set(storage_entry_type *object){
	clear();
	auto head = object->address_head();
	if (head != nullptr)//Increment reference count
		++head->ref_count;

	object_ = *object;
}

void slang::common::error::set(type type, storage_entry_type *object){
	if (object != nullptr)
		set(object);

	type_ = type;
}

void slang::common::error::set(const char *value, bool is_runtime){
	auto entry = common::env::temp_storage->add(value);
	if (entry == nullptr)
		return;

	auto type = common::env::map_type(is_runtime ? type_id_type::runtime_t : type_id_type::compile_t);
	storage_entry_type entry_copy(entry->owner(), entry->address_head(), type);
	set(&entry_copy);
}

void slang::common::error::set(const char *value, size_type size){
	set(common::env::temp_storage->add(value, size));
}

void slang::common::error::set(const std::string &value, const index_type &index){
	auto index_string = ("line " + std::to_string(index.line) + ", col " + std::to_string(index.column));
	set(std::string("Exception thrown at " + index_string + ": " + value).c_str());
}

void slang::common::error::warn(const char *value){
	if (common::env::error_writer != nullptr)
		common::env::error_writer->write(value);
}

void slang::common::error::suppress(){
	if (object_.address_head() != nullptr && type_ == type::nil)
		type_ = type::suppressed;
}

void slang::common::error::unsuppress(){
	if (type_ == type::suppressed)
		type_ = type::nil;
}

void slang::common::error::clear(){
	auto head = object_.address_head();
	if (head != nullptr){//Decrement reference count
		common::env::address_table.deallocate(head->value);
		object_ = {};
	}

	type_ = type::nil;
}

void slang::common::error::dump(){
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

	auto head = object_.address_head();
	if (head == nullptr)
		return;

	++head->ref_count;
	//#TODO: Echo object
	common::env::address_table.deallocate(head->value);
}

slang::common::error::storage_entry_type *slang::common::error::get(){
	return &object_;
}

bool slang::common::error::has() const{
	return (type_ != type::suppressed && (object_.address_head() != nullptr || type_ != type::nil));
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
