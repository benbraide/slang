#include "enum_driver.h"
#include "../common/env.h"

slang::driver::enum_driver::~enum_driver() = default;

slang::driver::object::uint64_type slang::driver::enum_driver::enum_value(entry_type &entry){
	return value(entry);
}

slang::driver::object::uint64_type slang::driver::enum_driver::value(entry_type &entry){
	return common::env::address_table.read<uint_type>(entry.address_value());
}

slang::driver::object::entry_type *slang::driver::enum_driver::evaluate_(entry_type &entry, binary_info_type &info, entry_type &operand){
	auto type = type_of(entry);
	auto driver = get_driver(operand);
	if (driver->type_of(operand)->score(type) != SLANG_MAX_TYPE_SCORE)
		return object::evaluate_(entry, info, operand);

	auto enum_type = dynamic_cast<type::enum_type *>(type);
	if (enum_type == nullptr)
		return object::evaluate_(entry, info, operand);

	switch (info.id){
	case operator_id_type::bitwise_or:
		if (enum_type->is_linear())
			return object::evaluate_(entry, info, operand);
		return common::env::temp_storage->add_typed(value(entry) | driver->enum_value(operand), type->reflect());
	case operator_id_type::equality:
		if (enum_type->is_linear())
			return common::env::temp_storage->add(value(entry) == driver->enum_value(operand));
		return common::env::temp_storage->add(SLANG_IS_ANY(value(entry), driver->enum_value(operand)));
	case operator_id_type::inverse_equality:
		if (enum_type->is_linear())
			return common::env::temp_storage->add(value(entry) != driver->enum_value(operand));
		return common::env::temp_storage->add(!SLANG_IS_ANY(value(entry), driver->enum_value(operand)));
	default:
		break;
	}

	return object::evaluate_(entry, info, operand);
}

void slang::driver::enum_driver::echo_(entry_type &entry, writer_type &out, bool no_throw){
	auto enum_type = dynamic_cast<type::enum_type *>(type_of(entry));
	if (enum_type == nullptr || !enum_type->print_item(value(entry), out))
		object::echo_(entry, out, no_throw);
}
