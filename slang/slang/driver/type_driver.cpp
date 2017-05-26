#include "type_driver.h"
#include "../common/env.h"

slang::driver::type_driver::~type_driver() = default;

slang::driver::type_driver::type_object_type *slang::driver::type_driver::value(entry_type &entry){
	if (entry.is_uninitialized())
		return nullptr;
	return reinterpret_cast<type_object_type *>(common::env::address_table.read<uint64_type>(entry.address_value()));
}

slang::driver::object::entry_type *slang::driver::type_driver::evaluate_(entry_type &entry, binary_info_type &info, entry_type &operand){
	auto driver = get_driver(operand);
	if (driver->type_of(operand)->id() != type_id_type::type_)
		return object::evaluate_(entry, info, operand);

	switch (info.id){
	case operator_id_type::bitwise_or:
		break;
	case operator_id_type::equality:
		return common::env::temp_storage->add(value(entry)->is_same(operand_value_(*driver, operand)));
	case operator_id_type::inverse_equality:
		return common::env::temp_storage->add(!value(entry)->is_same(operand_value_(*driver, operand)));
	case operator_id_type::like:
		return common::env::temp_storage->add(value(entry)->score(operand_value_(*driver, operand), true) != SLANG_MIN_TYPE_SCORE);
	default:
		return object::evaluate_(entry, info, operand);
	}

	auto variant = std::make_shared<type::variant>(value(entry)->reflect(), operand_value_(*driver, operand)->reflect());
	return common::env::temp_storage->add(*static_cast<type::object *>(variant.get()));
}

void slang::driver::type_driver::convert_(entry_type &entry, type_id_type id, char *buffer){
	if (id == type_id_type::type_)
		*reinterpret_cast<type_object_type **>(buffer) = value(entry);
	else
		object::convert_(entry, id, buffer);
}

void slang::driver::type_driver::echo_(entry_type &entry, writer_type &out, bool no_throw){
	auto value = this->value(entry);
	if (value == nullptr)
		common::env::error.set("Uninitialized value in expression", true);
	else
		out.write(value->print().c_str());
}

slang::driver::type_driver::type_object_type *slang::driver::type_driver::operand_value_(object &driver, entry_type &operand){
	type_object_type *value = nullptr;
	driver.convert(operand, type_id_type::type_, reinterpret_cast<char *>(&value));
	return value;
}
