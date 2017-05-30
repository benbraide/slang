#include "enum_type.h"
#include "../common/env.h"

slang::type::enum_type::enum_type(size_type count, const std::string &name, storage_type *parent)
	: named(name, *parent), size_(sizeof(uint64_type)), insert_count_(0u), address_(0u){
	is_linear_ = (count > static_cast<size_type>(0x40));
	if (count > 0u && (address_ = common::env::address_table.reserve(size_ * count)) == 0u)
		common::env::error.set("Could not allocate memory for enum");
}

slang::type::enum_type::~enum_type(){
	if (!common::env::exiting && address_ != 0u)
		common::env::address_table.deallocate(address_);
}

slang::type::object::driver_object_type *slang::type::enum_type::driver() const{
	return nullptr;
}

const std::string &slang::type::enum_type::name() const{
	return storage_type::name();
}

std::string slang::type::enum_type::print() const{
	return storage_type::print();
}

slang::type::object::size_type slang::type::enum_type::size() const{
	return size_;
}

slang::type::object::id_type slang::type::enum_type::id() const{
	return id_type::enum_;
}

bool slang::type::enum_type::is_enum() const{
	return true;
}

bool slang::type::enum_type::is_linear() const{
	return is_linear_;
}

bool slang::type::enum_type::insert(const std::string &item){
	auto head = common::env::address_table.allocate_reserved(address_ + (insert_count_ * size_), size_);
	if (head == nullptr)//Failed to allocate memory
		return false;

	auto value = storage_type::add<storage::entry>(item);
	if (value == nullptr){//Failed to add item
		common::env::address_table.deallocate(head->value);
		return false;
	}

	*value->object() = storage::entry(nullptr, head->value, reflect(), storage::entry::attribute_type::block_aligned);
	if (is_linear_)
		common::env::address_table.write(head->value, static_cast<uint64_type>(insert_count_));
	else//Composite
		common::env::address_table.write(head->value, (1ull << static_cast<uint64_type>(insert_count_)));

	SLANG_SET(head->attributes, address::head::attribute_type::write_protect);
	++insert_count_;

	return true;
}

bool slang::type::enum_type::print_item(uint64_type value, writer_type &out) const{
	if (!is_linear_){//Composite
		std::string composed;
		auto item = storage_type::order_list_.begin();

		for (uint64_type i = 0u; i < storage_type::order_list_.size(); ++i, ++item){
			if (SLANG_IS(value, (static_cast<uint64_type>(1) << i))){
				if (composed.empty())
					composed = (print() + "::" + *item->first);
				else//Append
					composed = (" | " + print() + "::" + *item->first);
			}
		}

		out.write(composed.c_str());
	}
	else if (value < storage_type::order_list_.size())//Value is index
		out.write((print() + "::" + *std::next(storage_type::order_list_.begin(), value)->first).c_str());
	else//Out of bounds
		return false;

	return true;
}

unsigned long long slang::type::enum_type::compute_value(size_type index) const{
	if (is_linear_)
		return static_cast<unsigned long long>(index);
	return (1ull << static_cast<unsigned long long>(index));
}
