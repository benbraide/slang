#include "address_table.h"

#pragma warning(disable: 4996)

slang::address::table::table(uint64_type protected_range)
	: next_(protected_range + 1ull), protected_(protected_range), thread_id_(std::this_thread::get_id()){}

slang::address::table::~table(){
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

void slang::address::table::on_thread_entry(){
	exclusive_lock_type guard(lock_);
	for (auto &entry : tls_captures_){//Initialize thread local storage
		auto &tls_entry = tls_[entry.first];

		tls_entry = entry.second;
		if (tls_entry.ptr != nullptr){//Duplicate bytes
			tls_entry.ptr = new char[tls_entry.actual_size];
			std::strncpy(tls_entry.ptr, entry.second.ptr, tls_entry.actual_size);
		}
	}
}

void slang::address::table::on_thread_exit(){
	exclusive_lock_type guard(lock_);
	for (auto &entry : tls_){//Free thread local storage
		if (entry.second.ptr != nullptr)
			delete[] entry.second.ptr;
	}
}

void slang::address::table::capture_tls(uint64_type value){
	exclusive_lock_type guard(lock_);

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

void slang::address::table::protect(){
	protected_ = (next_ - 1);
}

void slang::address::table::set_dependency(uint64_type value, dependency_ptr_type dependency){
	exclusive_lock_type guard(lock_);
	dependencies_[value] = dependency;
}

slang::address::watcher *slang::address::table::watch(uint64_type value, watcher_ptr_type watcher){
	exclusive_lock_type guard(lock_);
	
	auto entry = find_(value);
	if (entry == nullptr){
		//#TODO: Throw exception
		return nullptr;
	}

	if (SLANG_IS(entry->attributes, attribute_type::tls))
		watchers_[value][std::this_thread::get_id()] = watcher;
	else//No thread local storage
		watchers_[value][thread_id_] = watcher;

	return watcher.get();
}

slang::address::watcher *slang::address::table::find_watcher(uint64_type value) const{
	shared_lock_type guard(lock_);
	return find_watcher_(value);
}

slang::address::dependency *slang::address::table::get_dependency(uint64_type value) const{
	shared_lock_type guard(lock_);
	auto entry = dependencies_.find(value);
	return (entry == dependencies_.end()) ? nullptr : entry->second.get();
}

bool slang::address::table::deallocate(uint64_type value){
	exclusive_lock_type guard(lock_);
	return deallocate_(value);
}

slang::address::head *slang::address::table::allocate(uint_type size){
	exclusive_lock_type guard(lock_);
	return allocate_(size);
}

slang::address::head *slang::address::table::allocate(uint_type count, uint_type size){
	exclusive_lock_type guard(lock_);
	return allocate_contiguous_(count, size);
}

slang::address::head *slang::address::table::allocate_scalar(const char *value){
	return allocate_scalar_cstr(value);
}

slang::address::head *slang::address::table::allocate_scalar(const wchar_t *value){
	return allocate_scalar_wcstr(value);
}

slang::address::head *slang::address::table::reallocate(uint64_type value, uint_type size){
	exclusive_lock_type guard(lock_);

	auto entry = find_(value);
	if (entry == nullptr || entry->size == size)
		return entry;

	return ((entry->size < size) ? expand_(*entry, size - entry->size) : shrink_(*entry, entry->size - size));
}

slang::address::head *slang::address::table::allocate_reserved(uint64_type value, uint_type size){
	exclusive_lock_type guard(lock_);
	return allocate_(size, value);
}

slang::address::table::uint64_type slang::address::table::reserve(uint_type size){
	exclusive_lock_type guard(lock_);
	return reserve_(size);
}

slang::address::head *slang::address::table::find(uint64_type value) const{
	shared_lock_type guard(lock_);
	return find_(value);
}

slang::address::head *slang::address::table::get_head(uint64_type value) const{
	shared_lock_type guard(lock_);
	return get_head_(value);
}

bool slang::address::table::contains(uint64_type value) const{
	shared_lock_type guard(lock_);
	return (head_list_.find(value) != head_list_.end());
}

void slang::address::table::copy(uint64_type destination, uint64_type source, uint_type size){
	shared_lock_type guard(lock_);
	copy_(destination, source, size);
}

void slang::address::table::copy(uint64_type destination, uint64_type source){
	shared_lock_type guard(lock_);

	auto destination_entry = find_(destination);
	if (destination_entry == nullptr){
		//#TODO: Throw exception
		return;
	}

	auto source_entry = find_(source);
	if (source_entry == nullptr){
		//#TODO: Throw exception
		return;
	}

	if (source_entry->size < destination_entry->size){//Copy bytes and zero rest
		std::strncpy(destination_entry->ptr, source_entry->ptr, source_entry->size);
		std::memset(destination_entry->ptr + source_entry->size, 0, destination_entry->size - source_entry->size);
	}
	else//Direct copy
		std::strncpy(destination_entry->ptr, source_entry->ptr, destination_entry->size);

	auto watcher = find_watcher_(destination_entry->value);
	if (watcher != nullptr)
		watcher->on_change(destination_entry->value);//Alert watcher
}

void slang::address::table::set(uint64_type value, char c, uint_type size){
	shared_lock_type guard(lock_);
	write_(value, &c, size, false);
}

void slang::address::table::write(uint64_type destination, const char *source, uint_type size){
	shared_lock_type guard(lock_);
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
	shared_lock_type guard(lock_);
	read_(value, buffer, size);
}

void slang::address::table::convert_numeric(uint64_type destination, uint64_type source) const{
	shared_lock_type guard(lock_);
	convert_numeric_(destination, source);
}

bool slang::address::table::deallocate_(uint64_type value, bool merge){
	if (value <= protected_){
		//#TODO: Throw exception
		return false;
	}

	auto entry = head_list_.find(value);
	if (entry == head_list_.end())
		return false;

	if (SLANG_IS(entry->second.attributes, attribute_type::tls)){
		//#TODO: Throw exception
		return false;
	}

	if (entry->second.ref_count > 0u && --entry->second.ref_count > 0u)
		return true;//Referenced by some other object

	if (merge)//Add to available list
		merge_available_(value, entry->second.actual_size);

	if (entry->second.ptr != nullptr)
		delete[] entry->second.ptr;
	
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
	else if (value <= protected_ || get_head_(value) != nullptr){
		//#TODO: Throw exception
		return nullptr;//Invalid value
	}

	auto ptr = new char[actual_size];
	if (ptr == nullptr){//Failed to allocate buffer
		if (merge)
			merge_available_(value, actual_size);
		return nullptr;
	}

	return &(head_list_[value] = head{ 1u, value, size, actual_size, attribute_type::uninitialized, ptr });
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
	if (value <= protected_){
		//#TODO: Throw exception
		return nullptr;
	}

	auto entry = head_list_.find(value);
	if (entry == head_list_.end())//Not found
		return nullptr;

	if (SLANG_IS(entry->second.attributes, attribute_type::tls) && std::this_thread::get_id() != thread_id_)
		return &tls_[value];//Get corresponding head in current thread

	return const_cast<head *>(&entry->second);
}

slang::address::head *slang::address::table::get_head_(uint64_type value) const{
	if (value <= protected_){
		//#TODO: Throw exception
		return nullptr;
	}

	head *entry_head = nullptr;
	for (auto &entry : head_list_){
		if (entry.first == value || (entry.first < value && value < (entry.first + entry.second.actual_size))){
			entry_head = const_cast<head *>(&entry.second);
			break;
		}
	}

	if (entry_head == nullptr)
		return nullptr;

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

	if (size > 0u){
		//#TODO: Throw exception
	}
}

void slang::address::table::write_(uint64_type value, const char *source, uint_type size, bool is_array){
	if (size == 0u)
		return;

	head *entry = nullptr;
	uint_type available_size = 0u, ptr_index = 0u;
	watcher *watcher = nullptr;

	while (size > 0u){
		if (available_size == 0u){//Get next block
			if ((entry = (entry == nullptr) ? get_head_(value) : find_(value)) != nullptr){
				ptr_index = static_cast<uint_type>(value - entry->value);
				available_size = (entry->actual_size - ptr_index);
			}
			else//No next block
				break;
		}

		if (SLANG_IS(entry->attributes, attribute_type::write_protect)){//Block is write protected
			//#TODO: Throw exception
			break;
		}

		if (is_array){
			if (available_size < size){
				std::strncpy(entry->ptr + ptr_index, source, available_size);
				source += available_size;
				value += available_size;
				size -= available_size;
				available_size = 0u;
			}
			else{//Copy applicable
				std::strncpy(entry->ptr + ptr_index, source, size);
				size = 0u;
			}
		}
		else{//Single byte
			*(entry->ptr + ptr_index) = *source;//Write byte
			--available_size;
			--size;
			++ptr_index;
		}

		if ((watcher = find_watcher_(entry->value)) != nullptr)
			watcher->on_change(entry->value);//Alert watcher
	}

	if (size > 0u){
		//#TODO: Throw exception
	}
}

void slang::address::table::read_(uint64_type value, char *buffer, uint_type size) const{
	if (size == 0u)
		return;

	head *entry = nullptr;
	uint_type available_size = 0u, ptr_index = 0u;

	for (; size > 0u; --size, --available_size, ++value, ++ptr_index, ++buffer){
		if (available_size == 0u){//Get next block
			if ((entry = (entry == nullptr) ? get_head_(value) : find_(value)) != nullptr){
				ptr_index = static_cast<uint_type>(value - entry->value);
				available_size = (entry->actual_size - ptr_index);
			}
			else//No next block
				break;
		}

		*buffer = *(entry->ptr + ptr_index);//Read byte
	}

	if (size > 0u){
		//#TODO: Throw exception
	}
}

void slang::address::table::convert_numeric_(uint64_type destination, uint64_type source) const{
	auto destination_entry = find_(destination);
	if (destination_entry == nullptr){
		//#TODO: Throw exception
	}

	if (!SLANG_IS(destination_entry->attributes, attribute_type::is_float)){
		switch (destination_entry->size){
		case sizeof(__int8) :
			*reinterpret_cast<__int8 *>(destination_entry->ptr) = convert_numeric_<__int8>(source);
			break;
		case sizeof(__int16):
			*reinterpret_cast<__int16 *>(destination_entry->ptr) = convert_numeric_<__int16>(source);
			break;
		case sizeof(__int32):
			*reinterpret_cast<__int32 *>(destination_entry->ptr) = convert_numeric_<__int32>(source);
			break;
		case sizeof(__int64):
			*reinterpret_cast<__int64 *>(destination_entry->ptr) = convert_numeric_<__int64>(source);
			break;
		default:
			break;
		}
	}
	else{//Floating point
		switch (destination_entry->size){
		case sizeof(float):
			*reinterpret_cast<float *>(destination_entry->ptr) = convert_numeric_<float>(source);
			break;
		case sizeof(long double):
			*reinterpret_cast<long double *>(destination_entry->ptr) = convert_numeric_<long double>(source);
			break;
		default:
			break;
		}
	}

	//#TODO: Throw exception
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

thread_local slang::address::table::head_list_type slang::address::table::tls_;
