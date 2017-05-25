#include "driver_object.h"
#include "../common/env.h"

#pragma warning(disable: 4996)

slang::driver::object::~object() = default;

slang::driver::object::entry_type *slang::driver::object::clone(entry_type &entry){
	if (entry.is_uninitialized())
		return nullptr;

	if (entry.type()->is_static_array())
		return &entry;

	auto head = entry.address_head();
	if (head == nullptr && !common::env::error.has())
		return common::env::error.set_and_return<nullptr_t>("Memory access violation.", true);

	auto is_string = SLANG_IS(head->attributes, address_attribute_type::is_string);
	if (is_string && !entry.is_lval())
		return &entry;

	auto value = common::env::temp_storage->add(head->size, entry.type()->remove_modified()->reflect());
	if (value == nullptr)
		return nullptr;

	auto value_head = value->address_head();
	{//Copy attributes and value
		value_head->attributes = head->attributes;
		std::strncpy(value_head->ptr, head->ptr, (head->actual_size < value_head->actual_size) ? head->actual_size :
			value_head->actual_size);
	}

	if (is_string){//Increment underlying reference count
		auto underlying_head = common::env::address_table.find(*reinterpret_cast<uint64_type *>(head->ptr));
		if (underlying_head != nullptr)
			++underlying_head->ref_count;
	}

	return value;
}

slang::driver::object::entry_type *slang::driver::object::cast(entry_type &entry, type::object &type, cast_type options){
	auto this_type = entry.type();
	if (SLANG_IS(options, cast_type::ref))
		return ((type.is_any() || type.is_same(this_type.get())) ? &entry : nullptr);

	if (entry.is_void() || (!entry.is_lval() && entry.is_uninitialized()))
		return nullptr;

	if (type.is_ref()){//ref
		if (type.is_const())//const ref
			return (type.is_same(this_type.get()) ? &entry : nullptr);

		return (!entry.is_const() && type.is_same(this_type.get()) && entry.is_lval()) ?
			&entry : nullptr;
	}

	if (SLANG_IS(options, cast_type::reinterpret))
		return nullptr;//Cannot perform cast

	if (type.is_rval_ref())
		return ((!entry.is_const() || type.is_const()) && type.is_same(this_type.get()) ? &entry : nullptr);

	return ((type.is_any() || type.is_same(this_type.get())) ? &entry : nullptr);
}

slang::driver::object::entry_type *slang::driver::object::evaluate(entry_type &entry, binary_info_type &info, entry_type &operand){
	if (info.id == operator_id_type::assignment)
		return assign(entry, operand);

	if (entry.is_void() || operand.is_void())
		return common::env::error.set_and_return<nullptr_t>("Void value in expression", true);

	if (entry.is_uninitialized() || operand.is_uninitialized())
		return common::env::error.set_and_return<nullptr_t>("Uninitialized value in expression", true);

	return evaluate_(entry, info, operand);
}

slang::driver::object::entry_type *slang::driver::object::evaluate(entry_type &entry, unary_info_type &info){
	if (entry.is_void())
		return common::env::error.set_and_return<nullptr_t>("Void value in expression", true);
	return evaluate_(entry, info);
}

slang::driver::object::entry_type *slang::driver::object::assign(entry_type &entry, entry_type &value){
	if (entry.is_void() || value.is(attribute_type::void_))
		return common::env::error.set_and_return<nullptr_t>("Void value in expression", true);

	if (!entry.is_any_of(attribute_type::lval | attribute_type::ref_))
		return common::env::error.set_and_return<nullptr_t>("Cannot assign to an rvalue", true);

	if (!entry.is_uninitialized()){
		if (entry.is_const())
			return common::env::error.set_and_return<nullptr_t>("Cannot modify a constant object", true);

		if (entry.is(attribute_type::rval_ref))
			return common::env::error.set_and_return<nullptr_t>("Cannot modify a rvalue-ref object", true);
	}

	if (entry.address_head() == nullptr && !common::env::error.has())
		return common::env::error.set_and_return<nullptr_t>("Memory access violation.", true);

	return assign_(entry, value);
}

void slang::driver::object::initialize(entry_type &entry){
	if (!entry.is_uninitialized())
		return;//Already initialized

	if (!entry.is_ref() || type_of(entry)->is_static_array()){
		auto head = entry.address_head();
		if (head == nullptr && !common::env::error.has()){
			common::env::error.set("Memory access violation.");
			return;
		}

		common::env::address_table.set(head->value, '\0', head->actual_size);
		entry.remove_attributes(attribute_type::uninitialized);
	}
	else//Raise exception
		common::env::error.set("Missing initialization for reference object", true);
}

bool slang::driver::object::to_bool(entry_type &entry){
	return (convert<bool_type>(entry) == bool_type::true_);
}

std::string slang::driver::object::to_string(entry_type &entry){
	auto value = convert<const char *>(entry);
	return (value == nullptr) ? std::string() : std::string(value);
}

std::wstring slang::driver::object::to_wstring(entry_type &entry){
	auto value = convert<const wchar_t *>(entry);
	return (value == nullptr) ? std::wstring() : std::wstring(value);
}

void slang::driver::object::convert(entry_type &entry, type_id_type id, char *buffer){
	if (entry.is_void())
		common::env::error.set("Void value in expression", true);
	else if (entry.is_uninitialized())
		common::env::error.set("Uninitialized value in expression", true);
	else
		convert_(entry, id, buffer);
}

void slang::driver::object::echo(entry_type &entry){
	return echo(entry, *common::env::out_writer, false);
}

void slang::driver::object::echo(entry_type &entry, writer_type &out, bool no_throw){
	if (!no_throw){
		if (entry.is_void())
			common::env::error.set("Void value in expression", true);
		else if (entry.is_uninitialized())
			common::env::error.set("Uninitialized value in expression", true);
		else
			echo_(entry, out, no_throw);
	}
	else
		echo_(entry, out, no_throw);
}

slang::type::object *slang::driver::object::type_of(entry_type &entry){
	return entry.type().get();
}

slang::driver::object *slang::driver::object::get_driver(entry_type &entry){
	return entry.type()->driver();
}

slang::address::table &slang::driver::object::get_address_table(){
	return common::env::address_table;
}

slang::storage::temp *slang::driver::object::get_temp_storage(){
	return common::env::temp_storage;
}

bool slang::driver::object::has_error(){
	return common::env::error.has();
}

slang::driver::object::entry_type *slang::driver::object::linked_object(entry_type &entry){
	auto dependency = common::env::address_table.get_dependency(entry.address_head()->value);
	if (dependency == nullptr)
		return nullptr;

	auto indirect_dependency = dynamic_cast<address::indirect_dependency *>(dependency);
	return (indirect_dependency == nullptr) ? nullptr : &indirect_dependency->value();
}

slang::driver::object::uint64_type slang::driver::object::address_of(entry_type &entry){
	return entry.address_value();
}

bool slang::driver::object::is_indirect(entry_type &entry){
	return SLANG_IS_ANY(entry.address_head()->attributes, address_attribute_type::is_string | address_attribute_type::indirect);
}

slang::driver::object::entry_type *slang::driver::object::evaluate_(entry_type &entry, binary_info_type &info, entry_type &operand){
	if (info.id != operator_id_type::comma)
		return common::env::error.set_and_return<nullptr_t>("Operator does not take specified operands", true);

	auto value = get_driver(operand)->cast(operand, *type_of(entry), cast_type::ref);
	if (value == nullptr && !common::env::error.has())//Mismatched types
		return common::env::error.set_and_return<nullptr_t>("Operator does not take specified operands", true);

	return value;
}

slang::driver::object::entry_type *slang::driver::object::evaluate_(entry_type &entry, unary_info_type &info){
	if (!info.is_left)
		return common::env::error.set_and_return<nullptr_t>("Operator does not take specified operand", true);

	switch (info.id){
	case lexer::operator_id::bitwise_and:
		if (!entry.is_lval())
			return common::env::error.set_and_return<nullptr_t>("Cannot get reference an rvalue", true);
		return common::env::temp_storage->add_pointer(entry, type_of(entry)->reflect());
	case lexer::operator_id::sizeof_:
		return common::env::temp_storage->add(type_of(entry)->size());
	case lexer::operator_id::typeof:
		return common::env::temp_storage->add(*type_of(entry));
	case lexer::operator_id::call://(this)
		return &entry;
	default:
		break;
	}

	if (entry.is_uninitialized())
		return common::env::error.set_and_return<nullptr_t>("Uninitialized value in expression", true);

	switch (info.id){
	case lexer::operator_id::relational_not:
	{
		auto value = to_bool(entry);
		return common::env::error.has() ? nullptr : common::env::temp_storage->add(value);
	}
	default:
		break;
	}

	return common::env::error.set_and_return<nullptr_t>("Operator does not take specified operand", true);
}

slang::driver::object::entry_type *slang::driver::object::assign_(entry_type &entry, entry_type &value){
	auto driver = get_driver(value);
	if (!entry.type()->is_same(driver->type_of(value)))
		return common::env::error.set_and_return<nullptr_t>("Object is not compatible with target type", true);

	auto head = entry.cached_address_head();
	common::env::address_table.copy(head->value, driver->address_of(value), head->size);

	return &entry;
}

void slang::driver::object::convert_(entry_type &entry, type_id_type id, char *buffer){
	common::env::error.set("Cannot convert object to specified type", true);
}

void slang::driver::object::echo_(entry_type &entry, writer_type &out, bool no_throw){
	if (no_throw)//No exceptions
		out.write(("<Object: " + type_of(entry)->print() + ">").c_str());
	else//Raise exception
		common::env::error.set("Cannot echo object", true);
}
