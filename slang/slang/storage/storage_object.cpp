#include "storage_object.h"
#include "../common/env.h"

slang::storage::object::object()
	: parent_(nullptr){}

slang::storage::object::object(object &parent)
	: parent_(&parent){}

slang::storage::object::~object(){
	value_type *value;
	while (!order_list_.empty()){
		if ((value = order_list_.begin()->second)->is_storage())
			value->storage(false)->parent_ = parent_;//Move parent
		remove(*order_list_.begin()->first);
	}
}

slang::storage::object *slang::storage::object::parent() const{
	return parent_;
}

slang::storage::object *slang::storage::object::match(const std::string &name){
	return (parent_ == nullptr) ? nullptr : parent_->match(name);
}

slang::storage::object::value_type *slang::storage::object::find(const std::string &key, bool recursive){
	{//Scoped lock
		shared_lock_type guard(lock_);

		auto entry = value_list_.find(key);
		if (entry != value_list_.end())
			return &entry->second;
	}

	return (recursive && (parent_ != nullptr)) ? parent_->find(key, true) : nullptr;
}

bool slang::storage::object::remove(const std::string &key){
	exclusive_lock_type guard(lock_);
	auto entry = value_list_.find(key);
	if (entry == value_list_.end())
		return nullptr;

	clean_(entry->second);
	for (auto order_entry = order_list_.begin(); order_entry != order_list_.end(); ++order_entry){
		if (order_entry->first == &entry->first){//Remove from list
			order_list_.erase(order_entry);
			break;
		}
	}

	value_list_.erase(entry);
	return true;
}

bool slang::storage::object::remove(value_type &value){
	return remove(*value.key_);
}

slang::storage::object &slang::storage::object::use(const std::string &key, value_type &value){
	exclusive_lock_type guard(lock_);
	use_(key, value);
	return *this;
}

slang::storage::object &slang::storage::object::use(object &storage){
	exclusive_lock_type guard(lock_);
	for (auto &entry : storage.value_list_)
		use_(entry.first, entry.second);

	return *this;
}

void slang::storage::object::clean_(value_type &value){
	if (!value.is_object())
		return;

	auto head = value.object()->address_head();
	if (head != nullptr)//Free memory
		common::env::address_table.deallocate(head->value);
}

void slang::storage::object::use_(const std::string &key, value_type &value){

}
