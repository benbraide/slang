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

	if (info.id != operator_id_type::comma)
		return common::env::error.set_and_return<nullptr_t>("Operator does not take specified operands", true);

	auto value = operand.type()->driver()->cast(operand, *typeof(entry), cast_type::ref);
	if (value == nullptr)//Mismatched types
		return common::env::error.set_and_return<nullptr_t>("Operator does not take specified operands", true);

	return value;
}

slang::driver::object::entry_type *slang::driver::object::evaluate(entry_type &entry, unary_info_type &info){
	if (entry.is_void())
		return common::env::error.set_and_return<nullptr_t>("Void value in expression", true);

	if (!info.is_left)
		return common::env::error.set_and_return<nullptr_t>("Operator does not take specified operand", true);

	switch (info.id){
	case lexer::operator_id::bitwise_and:
		if (!entry.is_lval())
			return common::env::error.set_and_return<nullptr_t>("Cannot get reference an rvalue");
		return common::env::temp_storage->add_pointer(entry, typeof(entry)->reflect());
	case lexer::operator_id::sizeof_:
		return common::env::temp_storage->add(typeof(entry)->size());
	case lexer::operator_id::typeof:
		return common::env::temp_storage->add(*typeof(entry));
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
	case lexer::operator_id::call://(this)
		return &entry;
	default:
		break;
	}

	return common::env::error.set_and_return<nullptr_t>("Operator does not take specified operand", true);
}

slang::driver::object::entry_type *slang::driver::object::assign(entry_type &entry, entry_type &value){
	if (entry.is_void() || value.is(attribute_type::void_))
		return common::env::error.set_and_return<nullptr_t>("Void value in expression", true);

	if (!entry.is_any_of(attribute_type::lval | attribute_type::ref_))
		return common::env::error.set_and_return<nullptr_t>("Cannot assign to an rvalue");

	if (!entry.is_uninitialized()){
		if (entry.is_const())
			return common::env::error.set_and_return<nullptr_t>("Cannot modify a constant object");

		if (entry.is(attribute_type::rval_ref))
			return common::env::error.set_and_return<nullptr_t>("Cannot modify a rvalue-ref object");
	}

	return assign_(entry, value);
}

void slang::driver::object::initialize(entry_type &entry){
	if (!entry.is_uninitialized())
		return;//Already initialized

	if (!entry.is_ref() || typeof(entry)->is_static_array()){
		auto head = entry.address_head();
		common::env::address_table.set(head->value, '\0', head->actual_size);
		entry.remove_attributes(attribute_type::uninitialized);
	}
	else//Raise exception
		common::env::error.set("Missing initialization for reference object", true);
}

bool slang::driver::object::to_bool(entry_type &entry){
	if (entry.is_void())
		return common::env::error.set_and_return<nullptr_t>("Void value in expression", true);

	if (entry.is_uninitialized())
		return common::env::error.set_and_return<nullptr_t>("Uninitialized value in expression", true);

	return common::env::error.set_and_return<bool>("Cannot convert object to boolean value", true);
}

int slang::driver::object::to_int(entry_type &entry){
	if (entry.is_void())
		return common::env::error.set_and_return<int>("Void value in expression", true);

	if (entry.is_uninitialized())
		return common::env::error.set_and_return<int>("Uninitialized value in expression", true);

	return common::env::error.set_and_return<int>("Cannot convert object to integer value", true);
}

std::string slang::driver::object::to_string(entry_type &entry){
	if (entry.is_void())
		return common::env::error.set_and_return<std::string>("Void value in expression", true);

	if (entry.is_uninitialized())
		return common::env::error.set_and_return<std::string>("Uninitialized value in expression", true);

	return common::env::error.set_and_return<std::string>("Cannot convert object to string value", true);
}

void slang::driver::object::echo(entry_type &entry, writer_type &out, bool no_throw){
	if (!no_throw){
		if (entry.is_void())
			common::env::error.set("Void value in expression", true);
		else if (entry.is_uninitialized())
			common::env::error.set("Uninitialized value in expression", true);
		else//Raise exception
			common::env::error.set("Cannot echo object", true);
	}
	else//No exceptions
		out.write(("<Object: " + typeof(entry)->print() + ">\n").c_str());
}

slang::type::object *slang::driver::object::typeof(entry_type &entry){
	return entry.type().get();
}

slang::driver::object::entry_type *slang::driver::object::linked_object(entry_type &entry){
	auto dependency = common::env::address_table.get_dependency(entry.address_head()->value);
	if (dependency == nullptr)
		return nullptr;

	auto indirect_dependency = dynamic_cast<address::indirect_dependency *>(dependency);
	return (indirect_dependency == nullptr) ? nullptr : &indirect_dependency->value();
}

bool slang::driver::object::is_indirect(entry_type &entry){
	return SLANG_IS_ANY(entry.address_head()->attributes, address_attribute_type::is_string | address_attribute_type::indirect);
}

slang::driver::object::entry_type *slang::driver::object::assign_(entry_type &entry, entry_type &value){
	if (!entry.type()->is_same(typeof(value)))
		return common::env::error.set_and_return<nullptr_t>("Object is not compatible with target type");

	auto head = entry.address_head();
	common::env::address_table.copy(head->value, value.address_head()->value, head->size);

	return &entry;
}
