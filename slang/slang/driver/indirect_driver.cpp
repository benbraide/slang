#include "indirect_driver.h"

#include "../common/env.h"
#include "../address/indirect_address_dependency.h"

slang::driver::indirect::~indirect() = default;

slang::driver::object::entry_type *slang::driver::indirect::clone(entry_type &entry){
	auto target = linked_object(entry);
	if (target == nullptr)
		return common::env::error.set_and_return<nullptr_t>("Uninitialized value in expression", true);

	return get_driver(*target)->clone(*target);
}

slang::driver::object::entry_type *slang::driver::indirect::cast(entry_type &entry, type::object &type, cast_type options){
	auto target = linked_object(entry);
	if (target == nullptr)
		return nullptr;

	return get_driver(*target)->cast(*target, type, options);
}

slang::driver::object::entry_type *slang::driver::indirect::evaluate(entry_type &entry, binary_info_type &info, entry_type &operand){
	if (info.id == operator_id_type::assignment)
		return assign(entry, operand);

	auto target = linked_object(entry);
	if (target == nullptr)
		return common::env::error.set_and_return<nullptr_t>("Uninitialized value in expression", true);

	return get_driver(*target)->evaluate(*target, info, operand);
}

slang::driver::object::entry_type *slang::driver::indirect::evaluate(entry_type &entry, unary_info_type &info){
	auto target = linked_object(entry);
	if (target == nullptr)
		return common::env::error.set_and_return<nullptr_t>("Uninitialized value in expression", true);

	return get_driver(*target)->evaluate(*target, info);
}

void slang::driver::indirect::initialize(entry_type &entry){
	auto target = linked_object(entry);
	if (target != nullptr)
		get_driver(*target)->initialize(*target);
}

bool slang::driver::indirect::to_bool(entry_type &entry){
	auto target = linked_object(entry);
	if (target == nullptr)
		return common::env::error.set_and_return<bool>("Uninitialized value in expression", true);

	return get_driver(*target)->to_bool(*target);
}

std::string slang::driver::indirect::to_string(entry_type &entry){
	auto target = linked_object(entry);
	if (target == nullptr)
		return common::env::error.set_and_return<std::string>("Uninitialized value in expression", true);

	return get_driver(*target)->to_string(*target);
}

std::wstring slang::driver::indirect::to_wstring(entry_type &entry){
	auto target = linked_object(entry);
	if (target == nullptr)
		return common::env::error.set_and_return<std::wstring>("Uninitialized value in expression", true);

	return get_driver(*target)->to_wstring(*target);
}

void slang::driver::indirect::convert(entry_type &entry, type_id_type id, char *buffer){
	auto target = linked_object(entry);
	if (target == nullptr)
		common::env::error.set("Uninitialized value in expression", true);
	else
		get_driver(*target)->convert(*target, id, buffer);
}

void slang::driver::indirect::echo(entry_type &entry, writer_type &out, bool no_throw){
	auto target = linked_object(entry);
	if (target == nullptr)
		common::env::error.set("Uninitialized value in expression", true);
	else
		get_driver(*target)->echo(*target, out, no_throw);
}

slang::type::object *slang::driver::indirect::type_of(entry_type &entry){
	auto target = linked_object(entry);
	if (target == nullptr)
		return entry.type().get();

	return get_driver(*target)->type_of(*target);
}

slang::driver::object::uint_type slang::driver::indirect::size_of(entry_type &entry){
	auto target = linked_object(entry);
	if (target == nullptr)
		return entry.type()->size();

	return get_driver(*target)->size_of(*target);
}

slang::driver::object::entry_type *slang::driver::indirect::linked_object(entry_type &entry){
	auto dependency = common::env::address_table.get_dependency(value(entry));
	if (dependency == nullptr || common::env::error.has())
		return nullptr;

	return &dynamic_cast<address::indirect_dependency *>(dependency)->value();
}

slang::driver::object::uint64_type slang::driver::indirect::value(entry_type &entry){
	return common::env::address_table.read<uint64_type>(entry.address_value());
}

slang::driver::object::entry_type *slang::driver::indirect::assign_(entry_type &entry, entry_type &value){
	auto driver = get_driver(value);
	auto cloned = driver->clone(value);
	if (common::env::error.has())
		return nullptr;

	if (cloned == nullptr)
		return common::env::error.set_and_return<nullptr_t>("Failed to copy object.");

	return do_assignment_(entry, *cloned);
}

slang::driver::object::entry_type *slang::driver::indirect::do_assignment_(entry_type &entry, entry_type &value){
	auto dependency = common::env::address_table.get_dependency(this->value(entry));
	if (common::env::error.has())
		return nullptr;

	if (dependency == nullptr){//Create dependency
		auto indirect_dependency = std::make_shared<address::indirect_dependency>(value, nullptr);
		common::env::address_table.set_dependency(entry.address_value(), indirect_dependency);
	}
	else//Update value
		dynamic_cast<address::indirect_dependency *>(dependency)->set_value(value);

	auto head = entry.cached_address_head();
	common::env::address_table.copy(head->value, get_driver(value)->address_of(value), head->size);
	entry.remove_attributes(attribute_type::uninitialized);

	return &entry;
}
