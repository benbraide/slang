#include "address_watcher.h"

slang::address::watcher::~watcher() = default;

slang::address::watcher::uint64_type slang::address::watcher::add(callback_type callback, uint64_type id){
	exclusive_lock_type guard(lock_);
	if (id == 0ull)
		callback_list_[id = *reinterpret_cast<uint64_type *>(&callback)] = callback;
	else//Use id
		callback_list_[id] = callback;

	return id;
}

void slang::address::watcher::remove(uint64_type id){
	exclusive_lock_type guard(lock_);
	callback_list_.erase(id);
}

void slang::address::watcher::on_change(uint64_type value){
	shared_lock_type guard(lock_);
	for (auto &entry : callback_list_)
		entry.second(value);//#TODO: Execute in a different thread
}
