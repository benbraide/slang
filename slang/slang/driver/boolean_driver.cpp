#include "boolean_driver.h"
#include "../common/env.h"

slang::driver::boolean::~boolean() = default;

bool slang::driver::boolean::to_bool(entry_type &entry){
	return (value(entry) == bool_type::true_);
}

slang::driver::object::bool_type slang::driver::boolean::to_boolean(entry_type &entry){
	return value(entry);
}

slang::type::bool_type slang::driver::boolean::value(entry_type &entry){
	return common::env::address_table.read<bool_type>(entry.address_value());
}

slang::driver::object::entry_type *slang::driver::boolean::evaluate_(entry_type &entry, binary_info_type &info, entry_type &operand){
	auto driver = get_driver(operand);
	if (driver->type_of(operand)->id() != type_id_type::bool_)
		return object::evaluate_(entry, info, operand);

	switch (info.id){
	case operator_id_type::equality:
		return common::env::temp_storage->add(value(entry) == driver->to_boolean(operand));
	case operator_id_type::inverse_equality:
		return common::env::temp_storage->add(value(entry) != driver->to_boolean(operand));
	default:
		break;
	}

	return object::evaluate_(entry, info, operand);
}

slang::driver::object::entry_type *slang::driver::boolean::evaluate_(entry_type &entry, unary_info_type &info){
	if (info.is_left && info.id == operator_id_type::relational_not)
		return common::env::temp_storage->add(value(entry) == bool_type::false_);

	return object::evaluate_(entry, info);
}

void slang::driver::boolean::convert_(entry_type &entry, type_id_type id, char *buffer){
	if (id == type_id_type::bool_)//Convert to boolean value
		*reinterpret_cast<bool_type *>(buffer) = value(entry);
	else
		object::convert_(entry, id, buffer);
}

void slang::driver::boolean::echo_(entry_type &entry, writer_type &out, bool no_throw){
	switch (value(entry)){
	case bool_type::false_:
		out.write("false");
		break;
	case bool_type::true_:
		out.write("true");
		break;
	default:
		out.write("indeterminate");
		break;
	}
}
