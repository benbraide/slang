#include "address_table.h"
#include "../common/env.h"

#pragma warning(disable: 4996)

slang::address::table::table(uint64_type protected_range)
	: next_(protected_range + 1ull), protected_(protected_range), thread_id_(std::this_thread::get_id()){}

slang::address::table::~table(){
	if (!common::env::exiting){//Perform clean up
		for (auto &entry : head_list_){//Free memory
			if (entry.second.ptr != nullptr)
				delete[] entry.second.ptr;
		}

		for (auto &entry : tls_captures_){
			if (entry.second.ptr != nullptr)
				delete[] entry.second.ptr;
		}

		for (auto &entry : tls_){//Free thread local storage
			if (entry.second.ptr != nullptr)
				delete[] entry.second.ptr;
		}
	}
}

void slang::address::table::on_thread_entry(){
	if (!is_locked_()){
		SLANG_SET(common::env::runtime.state, common::env::runtime_state::address_table_locked);
		exclusive_lock_type guard(lock_);

		on_thread_entry_();
		SLANG_REMOVE(common::env::runtime.state, common::env::runtime_state::address_table_locked);
	}
	else//Locked
		on_thread_entry_();
}

void slang::address::table::on_thread_exit(){
	if (!is_locked_()){
		SLANG_SET(common::env::runtime.state, common::env::runtime_state::address_table_locked);
		exclusive_lock_type guard(lock_);

		on_thread_exit_();
		SLANG_REMOVE(common::env::runtime.state, common::env::runtime_state::address_table_locked);
	}
	else//Locked
		on_thread_exit_();
}

void slang::address::table::capture_tls(uint64_type value){
	if (!is_locked_()){
		SLANG_SET(common::env::runtime.state, common::env::runtime_state::address_table_locked);
		exclusive_lock_type guard(lock_);

		capture_tls_(value);
		SLANG_REMOVE(common::env::runtime.state, common::env::runtime_state::address_table_locked);
	}
	else//Locked
		capture_tls_(value);
}

void slang::address::table::protect(){
	protected_ = (next_ - 1);
}

bool slang::address::table::is_protected(uint64_type value) const{
	return (value <= protected_);
}

void slang::address::table::set_dependency(uint64_type value, dependency_ptr_type dependency){
	if (!is_locked_()){
		SLANG_SET(common::env::runtime.state, common::env::runtime_state::address_table_locked);
		exclusive_lock_type guard(lock_);
		dependencies_[value] = dependency;
		SLANG_REMOVE(common::env::runtime.state, common::env::runtime_state::address_table_locked);
	}
}

slang::address::watcher *slang::address::table::watch(uint64_type value, watcher_ptr_type watcher){
	if (!is_locked_()){
		SLANG_SET(common::env::runtime.state, common::env::runtime_state::address_table_locked);
		exclusive_lock_type guard(lock_);

		auto result = watch_(value, watcher);
		SLANG_REMOVE(common::env::runtime.state, common::env::runtime_state::address_table_locked);

		return result;
	}

	return watch_(value, watcher);
}

slang::address::watcher *slang::address::table::find_watcher(uint64_type value) const{
	if (!is_locked_()){
		SLANG_SET(common::env::runtime.state, common::env::runtime_state::address_table_locked);
		shared_lock_type guard(lock_);

		auto result = find_watcher_(value);
		SLANG_REMOVE(common::env::runtime.state, common::env::runtime_state::address_table_locked);

		return result;
	}

	return find_watcher_(value);
}

slang::address::dependency *slang::address::table::get_dependency(uint64_type value) const{
	if (!is_locked_()){
		SLANG_SET(common::env::runtime.state, common::env::runtime_state::address_table_locked);
		shared_lock_type guard(lock_);

		auto result = get_dependency_(value);
		SLANG_REMOVE(common::env::runtime.state, common::env::runtime_state::address_table_locked);

		return result;
	}
	
	return get_dependency_(value);
}

bool slang::address::table::deallocate(uint64_type value){
	if (!is_locked_()){
		SLANG_SET(common::env::runtime.state, common::env::runtime_state::address_table_locked);
		exclusive_lock_type guard(lock_);

		auto result = deallocate_(value);
		SLANG_REMOVE(common::env::runtime.state, common::env::runtime_state::address_table_locked);

		return result;
	}

	return deallocate_(value);
}

slang::address::head *slang::address::table::allocate(uint_type size){
	if (!is_locked_()){
		SLANG_SET(common::env::runtime.state, common::env::runtime_state::address_table_locked);
		exclusive_lock_type guard(lock_);

		auto result = allocate_(size);
		SLANG_REMOVE(common::env::runtime.state, common::env::runtime_state::address_table_locked);

		return result;
	}

	return allocate_(size);
}

slang::address::head *slang::address::table::allocate(uint_type count, uint_type size){
	if (!is_locked_()){
		SLANG_SET(common::env::runtime.state, common::env::runtime_state::address_table_locked);
		exclusive_lock_type guard(lock_);

		auto result = allocate_contiguous_(count, size);
		SLANG_REMOVE(common::env::runtime.state, common::env::runtime_state::address_table_locked);

		return result;
	}

	return allocate_contiguous_(count, size);
}

slang::address::head *slang::address::table::allocate_scalar(bool value){
	return allocate_scalar(value ? type::bool_type::true_ : type::bool_type::false_);
}

slang::address::head *slang::address::table::allocate_scalar(const char *value){
	return allocate_scalar_cstr(value);
}

slang::address::head *slang::address::table::allocate_scalar(const wchar_t *value){
	return allocate_scalar_wcstr(value);
}

slang::address::head *slang::address::table::allocate_scalar_cstr(const char *value, uint_type size){
	return allocate_scalar(value, (size == 0u) ? static_cast<uint_type>(std::strlen(value) + 1) : size);
}

slang::address::head *slang::address::table::allocate_scalar_wcstr(const wchar_t *value, uint_type size){
	return allocate_scalar(value, (size == 0u) ? static_cast<uint_type>(std::wcslen(value) + 1) : size);
}

slang::address::head *slang::address::table::reallocate(uint64_type value, uint_type size){
	if (!is_locked_()){
		SLANG_SET(common::env::runtime.state, common::env::runtime_state::address_table_locked);
		exclusive_lock_type guard(lock_);

		auto result = reallocate_(value, size);
		SLANG_REMOVE(common::env::runtime.state, common::env::runtime_state::address_table_locked);

		return result;
	}
	
	return reallocate_(value, size);
}

slang::address::head *slang::address::table::allocate_reserved(uint64_type value, uint_type size){
	if (!is_locked_()){
		SLANG_SET(common::env::runtime.state, common::env::runtime_state::address_table_locked);
		exclusive_lock_type guard(lock_);

		auto result = allocate_(size, value);
		SLANG_REMOVE(common::env::runtime.state, common::env::runtime_state::address_table_locked);

		return result;
	}

	return allocate_(size, value);
}

slang::address::table::uint64_type slang::address::table::reserve(uint_type size){
	if (!is_locked_()){
		SLANG_SET(common::env::runtime.state, common::env::runtime_state::address_table_locked);
		exclusive_lock_type guard(lock_);

		auto result = reserve_(size);
		SLANG_REMOVE(common::env::runtime.state, common::env::runtime_state::address_table_locked);

		return result;
	}

	return reserve_(size);
}

slang::address::head *slang::address::table::find(uint64_type value) const{
	if (!is_locked_()){
		SLANG_SET(common::env::runtime.state, common::env::runtime_state::address_table_locked);
		shared_lock_type guard(lock_);

		auto result = find_(value);
		SLANG_REMOVE(common::env::runtime.state, common::env::runtime_state::address_table_locked);

		return result;
	}

	return find_(value);
}

slang::address::head *slang::address::table::get_head(uint64_type value) const{
	if (!is_locked_()){
		SLANG_SET(common::env::runtime.state, common::env::runtime_state::address_table_locked);
		shared_lock_type guard(lock_);

		auto result = get_head_(value);
		SLANG_REMOVE(common::env::runtime.state, common::env::runtime_state::address_table_locked);

		return result;
	}

	return get_head_(value);
}

bool slang::address::table::contains(uint64_type value) const{
	if (!is_locked_()){
		SLANG_SET(common::env::runtime.state, common::env::runtime_state::address_table_locked);
		shared_lock_type guard(lock_);

		auto result = (head_list_.find(value) != head_list_.end());
		SLANG_REMOVE(common::env::runtime.state, common::env::runtime_state::address_table_locked);

		return result;
	}

	return (head_list_.find(value) != head_list_.end());
}

void slang::address::table::copy(uint64_type destination, uint64_type source, uint_type size){
	if (!is_locked_()){
		SLANG_SET(common::env::runtime.state, common::env::runtime_state::address_table_locked);
		shared_lock_type guard(lock_);

		copy_(destination, source, size);
		SLANG_REMOVE(common::env::runtime.state, common::env::runtime_state::address_table_locked);
	}
	else//Locked
		copy_(destination, source, size);
}

void slang::address::table::set(uint64_type value, char c, uint_type size){
	if (!is_locked_()){
		SLANG_SET(common::env::runtime.state, common::env::runtime_state::address_table_locked);
		shared_lock_type guard(lock_);

		write_(value, &c, size, false);
		SLANG_REMOVE(common::env::runtime.state, common::env::runtime_state::address_table_locked);
	}
	else//Locked
		write_(value, &c, size, false);
}

void slang::address::table::write(uint64_type destination, const char *source, uint_type size){
	if (!is_locked_()){
		SLANG_SET(common::env::runtime.state, common::env::runtime_state::address_table_locked);
		shared_lock_type guard(lock_);

		write_(destination, source, size, true);
		SLANG_REMOVE(common::env::runtime.state, common::env::runtime_state::address_table_locked);
	}
	else//Locked
		write_(destination, source, size, true);
}

void slang::address::table::write(uint64_type destination, const wchar_t *source, uint_type size){
	write(destination, reinterpret_cast<const char *>(&source), size * sizeof(wchar_t));
}

void slang::address::table::write(uint64_type destination, const char *source){
	return write(destination, source, static_cast<uint_type>(std::strlen(source) + 1));
}

void slang::address::table::write(uint64_type destination, const wchar_t *source){
	return write(destination, source, static_cast<uint_type>(std::wcslen(source) + 1));
}

void slang::address::table::read(uint64_type value, char *buffer, uint_type size) const{
	if (!is_locked_()){
		SLANG_SET(common::env::runtime.state, common::env::runtime_state::address_table_locked);
		shared_lock_type guard(lock_);

		read_(value, buffer, size);
		SLANG_REMOVE(common::env::runtime.state, common::env::runtime_state::address_table_locked);
	}
	else//Locked
		read_(value, buffer, size);
}

void slang::address::table::on_thread_entry_(){
	common::env::runtime.state = common::env::runtime_state::error_enabled;
	for (auto &entry : tls_captures_){//Initialize thread local storage
		auto &tls_entry = tls_[entry.first];

		tls_entry = entry.second;
		if (tls_entry.ptr != nullptr){//Duplicate bytes
			tls_entry.ptr = new char[tls_entry.actual_size];
			std::strncpy(tls_entry.ptr, entry.second.ptr, tls_entry.actual_size);
		}

		if (SLANG_IS_ANY(entry.second.attributes, attribute_type::is_string | attribute_type::indirect)){
			auto linked_entry = find_(entry.first);
			if (linked_entry != nullptr)
				++linked_entry->ref_count;
		}
	}
}

void slang::address::table::on_thread_exit_(){
	for (auto &entry : tls_){//Free thread local storage
		if (entry.second.ptr != nullptr){
			if (SLANG_IS_ANY(entry.second.attributes, attribute_type::is_string | attribute_type::indirect))
				deallocate_(*reinterpret_cast<uint64_type *>(entry.second.ptr), true, true);//Deallocate linked memory
			delete[] entry.second.ptr;
		}
	}
}

void slang::address::table::capture_tls_(uint64_type value){
	auto entry = find_(value);
	if (entry == nullptr)
		return;

	SLANG_SET(entry->attributes, attribute_type::tls);
	auto &capture = tls_captures_[value];

	capture = *entry;
	if (capture.ptr != nullptr){//Duplicate bytes
		capture.ptr = new char[capture.actual_size];
		std::strncpy(capture.ptr, entry->ptr, capture.actual_size);
	}
}

slang::address::watcher *slang::address::table::watch_(uint64_type value, watcher_ptr_type watcher){
	auto entry = find_(value);
	if (entry == nullptr)
		return throw_and_return_<nullptr_t>("Failed to watch memory - invalid address.");

	if (SLANG_IS(entry->attributes, attribute_type::tls))
		watchers_[value][std::this_thread::get_id()] = watcher;
	else//No thread local storage
		watchers_[value][thread_id_] = watcher;

	return watcher.get();
}

slang::address::dependency *slang::address::table::get_dependency_(uint64_type value) const{
	auto entry = dependencies_.find(value);
	return (entry == dependencies_.end()) ? nullptr : entry->second.get();
}

bool slang::address::table::deallocate_(uint64_type value, bool merge, bool ignore_tls){
	if (value <= protected_)
		return throw_and_return_<bool>("Memory access violation.");

	auto entry = head_list_.find(value);
	if (entry == head_list_.end())
		return false;

	if (SLANG_IS(entry->second.attributes, attribute_type::tls))
		return ignore_tls ? nullptr : throw_and_return_<bool>("Memory access violation.");

	if (entry->second.ref_count > 0u && --entry->second.ref_count > 0u)
		return true;//Referenced by some other object

	if (merge)//Add to available list
		merge_available_(value, entry->second.actual_size);

	if (entry->second.ptr != nullptr){
		if (SLANG_IS_ANY(entry->second.attributes, attribute_type::is_string | attribute_type::indirect))
			deallocate_(*reinterpret_cast<uint64_type *>(entry->second.ptr));//Deallocate linked memory
		delete[] entry->second.ptr;
	}
	
	head_list_.erase(entry);
	watchers_.erase(value);
	dependencies_.erase(value);

	return true;
}

slang::address::head *slang::address::table::allocate_(uint_type size, uint64_type value){
	if (head_list_.max_size() <= head_list_.size())
		return nullptr;//Out of address space

	auto merge = false;
	auto actual_size = ((size == 0u) ? 1u : size);

	if (value == 0ull){//Determine value
		merge = true;
		if ((value = get_available_(actual_size)) == 0ull){//Use next value
			if ((std::numeric_limits<uint64_type>::max() - actual_size) < value)
				return nullptr;//Out of address space

			value = next_;
			next_ += actual_size;
		}
		else//Remove from list
			available_list_.erase(value);
	}
	else if (value <= protected_ || get_head_(value) != nullptr)
		return throw_and_return_<nullptr_t>("Memory access violation.");

	auto ptr = new char[actual_size];
	if (ptr == nullptr){//Failed to allocate buffer
		if (merge)
			merge_available_(value, actual_size);
		return nullptr;
	}

	return &(head_list_[value] = head{ 1u, value, size, actual_size, attribute_type::nil, ptr });
}

slang::address::head *slang::address::table::allocate_contiguous_(uint_type count, uint_type size){
	if (count == 0u)
		return nullptr;

	auto actual_size = ((size == 0u) ? 1u : size);
	auto value = reserve_(actual_size * count);
	if (value == 0ull)//Failed to reserve contiguous memory
		return nullptr;

	auto start_value = value;
	std::vector<head *> entries(size);

	for (auto i = 0u; i < count; ++i, value += actual_size){
		if ((entries[i] = allocate_(size, value)) == nullptr){//Allocation failed
			for (auto j = 0u; j < i; ++j)//Rollback
				deallocate_(entries[j]->value, false);

			merge_available_(start_value, actual_size * count);
			return nullptr;
		}
	}

	return *entries.begin();
}

slang::address::head *slang::address::table::reallocate_(uint64_type value, uint_type size){
	auto entry = find_(value);
	if (entry == nullptr || entry->size == size)
		return entry;

	return ((entry->size < size) ? expand_(*entry, size - entry->size) : shrink_(*entry, entry->size - size));
}

slang::address::head *slang::address::table::shrink_(head &head, uint_type size){
	head.size -= size;
	return &head;
}

slang::address::head *slang::address::table::expand_(head &head, uint_type size){
	if ((head.size + size) <= head.actual_size){//Increase size -- same address
		head.size += size;
		return &head;
	}

	{//Adjust
		size -= (head.actual_size - head.size);
		head.size = head.actual_size;
	}

	auto actual_size = head.actual_size;
	auto available = get_available_(size, head.value + head.size);

	if (available == 0ull && head_list_.find(head.value + head.size) != head_list_.end()){//Allocate new
		auto allocated = allocate_(head.size + size);
		if (allocated == nullptr)//Failed to allocate
			return &head;

		allocated->attributes = head.attributes;
		if (head.ptr != nullptr)//Copy data
			std::strncpy(allocated->ptr, head.ptr, actual_size);
		deallocate_(head.value);

		return allocated;
	}

	auto old_value = head.value;
	head.actual_size = (head.size += size);
	if (available != 0ull){
		head.value = available;
		available_list_.erase(available);
	}
	else//Update next
		next_ += size;

	auto old_ptr = head.ptr;
	if ((head.ptr = new char[head.actual_size]) == nullptr)
		return &head;
	
	if (old_ptr != nullptr){//Move data
		std::strncpy(head.ptr, old_ptr, actual_size);
		delete[] old_ptr;
	}

	if (old_value != head.value)//Deallocate previous
		deallocate_(old_value);

	return &head;
}

slang::address::table::uint64_type slang::address::table::reserve_(uint_type size){
	if (size == 0u)
		return 0ull;

	auto value = get_available_(size);
	if (value == 0ull){//Use next value
		if ((std::numeric_limits<uint64_type>::max() - size) < value)
			return 0ull;//Out of address space

		value = next_;
		next_ += size;
	}
	else//Remove from list
		available_list_.erase(value);

	return value;
}

slang::address::head *slang::address::table::find_(uint64_type value) const{
	if (value <= protected_)
		return throw_and_return_<nullptr_t>("Memory access violation.");

	auto entry = head_list_.find(value);
	if (entry == head_list_.end())//Not found
		return nullptr;

	if (SLANG_IS(entry->second.attributes, attribute_type::tls) && std::this_thread::get_id() != thread_id_)
		return &tls_[value];//Get corresponding head in current thread

	return const_cast<head *>(&entry->second);
}

slang::address::head *slang::address::table::get_head_(uint64_type value) const{
	if (value <= protected_)
		return throw_and_return_<nullptr_t>("Memory access violation.");

	head *entry_head = nullptr;
	for (auto &entry : head_list_){
		if (entry.first == value || (entry.first < value && value < (entry.first + entry.second.actual_size))){
			entry_head = const_cast<head *>(&entry.second);
			break;
		}
	}

	if (entry_head == nullptr)
		return nullptr;

	if (entry_head->value <= protected_)
		return throw_and_return_<nullptr_t>("Memory access violation.");

	if (SLANG_IS(entry_head->attributes, attribute_type::tls) && std::this_thread::get_id() != thread_id_)
		return &tls_[value];//Get corresponding head in current thread

	return entry_head;
}

slang::address::watcher *slang::address::table::find_watcher_(uint64_type value) const{
	auto entry = find_(value);
	if (entry == nullptr)
		return nullptr;

	auto watcher_entry = watchers_.find(value);
	if (watcher_entry == watchers_.end())
		return nullptr;

	thread_id_type id;
	if (SLANG_IS(entry->attributes, attribute_type::tls))
		id = std::this_thread::get_id();
	else//No thread local storage
		id = thread_id_;

	auto tls_entry = watcher_entry->second.find(id);
	return (tls_entry == watcher_entry->second.end()) ? nullptr : tls_entry->second.get();
}

void slang::address::table::copy_(uint64_type destination, uint64_type source, uint_type size){
	if (size == 0u)
		return;

	head *entry = nullptr;
	uint_type available_size = 0u, target_size = 0u, ptr_index = 0u;

	while (size > 0u){
		if ((entry = (entry == nullptr) ? get_head_(source) : find_(source)) != nullptr){
			ptr_index = static_cast<uint_type>(source - entry->value);
			available_size = (entry->actual_size - ptr_index);
		}
		else//No next block
			break;

		target_size = (available_size < size) ? available_size : size;
		write_(destination, entry->ptr + ptr_index, target_size, true);

		source += target_size;
		destination += target_size;
		size -= target_size;
	}

	if (size > 0u && !common::env::error.has())
		throw_("Memory access violation.");
}

void slang::address::table::write_(uint64_type value, const char *source, uint_type size, bool is_array){
	if (size == 0u)
		return;

	head *entry = nullptr;
	uint_type available_size = 0u, min_size = 0u, ptr_index = 0u;
	watcher *watcher = nullptr;

	bool unlinked;
	while (size > 0u){
		unlinked = false;
		if ((entry = (entry == nullptr) ? get_head_(value) : find_(value)) != nullptr){//Get next block
			ptr_index = static_cast<uint_type>(value - entry->value);
			available_size = (entry->actual_size - ptr_index);
		}
		else//No next block
			break;

		if (SLANG_IS(entry->attributes, attribute_type::write_protect)){//Block is write protected
			common::env::error.set("Memory write access violation.", true);
			return;
		}

		if (!unlinked && SLANG_IS_ANY(entry->attributes, attribute_type::is_string | attribute_type::indirect)){
			deallocate_(*reinterpret_cast<uint64_type *>(entry->ptr), true, true);//Deallocate linked memory
			unlinked = true;
		}

		min_size = (available_size < size) ? available_size : size;
		if (is_array)
			std::strncpy(entry->ptr + ptr_index, source, min_size);
		else//Set applicable
			std::memset(entry->ptr, *source, min_size);

		source += min_size;
		value += min_size;
		size -= min_size;

		if ((watcher = find_watcher_(entry->value)) != nullptr)
			watcher->on_change(entry->value);//Alert watcher
	}

	if (size > 0u && !common::env::error.has())
		throw_("Memory access violation.");
}

void slang::address::table::read_(uint64_type value, char *buffer, uint_type size) const{
	if (size == 0u)
		return;

	head *entry = nullptr;
	uint_type available_size = 0u, min_size = 0u, ptr_index = 0u;

	while (size > 0u){
		if ((entry = (entry == nullptr) ? get_head_(value) : find_(value)) != nullptr){//Get next block
			ptr_index = static_cast<uint_type>(value - entry->value);
			available_size = (entry->actual_size - ptr_index);
		}
		else//No next block
			break;

		min_size = (available_size < size) ? available_size : size;
		std::strncpy(buffer, entry->ptr, min_size);//Read block

		buffer += min_size;
		value += min_size;
		size -= min_size;
	}

	if (size > 0u && !common::env::error.has())
		common::env::error.set("Memory write access violation.", true);
}

void slang::address::table::merge_available_(uint64_type value, uint_type size){
	if (size == 0u)
		return;

	auto entry = available_list_.begin();
	for (; entry != available_list_.end(); ++entry){
		if ((entry->first + entry->second) == value)
			break;//Previous in sequence
	}

	if (entry != available_list_.end()){//Merge with previous
		entry->second += size;
		value = entry->first;
		size = entry->second;
	}

	auto next = available_list_.find(value + size);
	if (next != available_list_.end()){//Merge with next
		entry->second += next->second;
		available_list_.erase(next);
	}
	else//Add entry
		available_list_[value] = size;

	if (!available_list_.empty()){
		auto last = std::prev(available_list_.end());
		if ((last->first + last->second) == next_){//Move next backwards
			next_ = last->first;
			available_list_.erase(last);
		}
	}
}

slang::address::table::uint64_type slang::address::table::get_available_(uint_type size, uint64_type match){
	for (auto &entry : available_list_){
		if (match != 0ull && entry.first != match){
			if (match < entry.first)
				break;
			continue;
		}

		if (size < entry.second){//Use required
			available_list_[entry.first + size] = (entry.second - size);
			return entry.first;
		}

		if (size == entry.second)
			return entry.first;
	}

	return 0ull;
}

void slang::address::table::throw_(const char *err) const{
	if (!common::env::error.has())
		common::env::error.set(err, true);
}

bool slang::address::table::is_locked_() const{
	return SLANG_IS(common::env::runtime.state, common::env::runtime_state::address_table_locked);
}

void slang::address::table::set_locked_state_(bool is_locked) const{
	if (is_locked)
		SLANG_SET(common::env::runtime.state, common::env::runtime_state::address_table_locked);
	else//Unlocked
		SLANG_REMOVE(common::env::runtime.state, common::env::runtime_state::address_table_locked);
}

thread_local slang::address::table::head_list_type slang::address::table::tls_;
