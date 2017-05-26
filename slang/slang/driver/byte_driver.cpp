#include "byte_driver.h"
#include "../common/env.h"

slang::driver::byte::~byte() = default;

slang::driver::object::entry_type *slang::driver::byte::cast(entry_type &entry, type::object &type, cast_type options){
	auto value = object::cast(entry, type, options);
	if (value != nullptr || SLANG_IS(options, cast_type::ref) || type.is_ref() || common::env::error.has())
		return value;

	if (!SLANG_IS(options, cast_type::reinterpret))
		return nullptr;

	switch (type.id()){
	case type_id_type::char_:
		return common::env::temp_storage->add(static_cast<char>(this->value(entry)));
	case type_id_type::uchar:
		return common::env::temp_storage->add(static_cast<unsigned char>(this->value(entry)));
	case type_id_type::short_:
		return common::env::temp_storage->add(static_cast<short>(this->value(entry)));
	case type_id_type::ushort:
		return common::env::temp_storage->add(static_cast<unsigned short>(this->value(entry)));
	case type_id_type::int_:
		return common::env::temp_storage->add(static_cast<int>(this->value(entry)));
	case type_id_type::uint:
		return common::env::temp_storage->add(static_cast<unsigned int>(this->value(entry)));
	case type_id_type::long_:
		return common::env::temp_storage->add(static_cast<long>(this->value(entry)));
	case type_id_type::ulong:
		return common::env::temp_storage->add(static_cast<unsigned long>(this->value(entry)));
	case type_id_type::llong:
		return common::env::temp_storage->add(static_cast<long long>(this->value(entry)));
	case type_id_type::ullong:
		return common::env::temp_storage->add(static_cast<unsigned long long>(this->value(entry)));
	case type_id_type::float_:
		return common::env::temp_storage->add(static_cast<float>(this->value(entry)));
	case type_id_type::double_:
		return common::env::temp_storage->add(static_cast<double>(this->value(entry)));
	case type_id_type::ldouble:
		return common::env::temp_storage->add(static_cast<long double>(this->value(entry)));
	default:
		break;
	}

	return nullptr;
}

slang::driver::byte::uchar_type slang::driver::byte::value(entry_type &entry){
	return common::env::address_table.read<uchar_type>(entry.address_value());
}

slang::driver::object::entry_type *slang::driver::byte::evaluate_(entry_type &entry, binary_info_type &info, entry_type &operand){
	auto driver = get_driver(operand);
	if (driver->type_of(operand)->id() != type_id_type::byte)
		return object::evaluate_(entry, info, operand);

	switch (info.id){
	case operator_id_type::compound_left_shift:
		return !is_non_const_lval_(entry) ? nullptr :
			write_(entry, static_cast<uchar_type>(value(entry) << operand_value_(*driver, operand)));
	case operator_id_type::left_shift:
		return common::env::temp_storage->add_typed(static_cast<uchar_type>(value(entry) <<
			operand_value_(*driver, operand)), type_of(entry)->reflect());
	case operator_id_type::compound_right_shift:
		return !is_non_const_lval_(entry) ? nullptr :
			write_(entry, static_cast<uchar_type>(value(entry) >> operand_value_(*driver, operand)));
	case operator_id_type::right_shift:
		return common::env::temp_storage->add_typed(static_cast<uchar_type>(value(entry) >>
			operand_value_(*driver, operand)), type_of(entry)->reflect());
	case operator_id_type::compound_bitwise_and:
		return !is_non_const_lval_(entry) ? nullptr :
			write_(entry, static_cast<uchar_type>(value(entry) & operand_value_(*driver, operand)));
	case operator_id_type::bitwise_and:
		return common::env::temp_storage->add_typed(static_cast<uchar_type>(value(entry) &
			operand_value_(*driver, operand)), type_of(entry)->reflect());
	case operator_id_type::compound_bitwise_or:
		return !is_non_const_lval_(entry) ? nullptr :
			write_(entry, static_cast<uchar_type>(value(entry) | operand_value_(*driver, operand)));
	case operator_id_type::bitwise_or:
		return common::env::temp_storage->add_typed(static_cast<uchar_type>(value(entry) |
			operand_value_(*driver, operand)), type_of(entry)->reflect());
	case operator_id_type::compound_bitwise_xor:
		return !is_non_const_lval_(entry) ? nullptr :
			write_(entry, static_cast<uchar_type>(value(entry) ^ operand_value_(*driver, operand)));
	case operator_id_type::bitwise_xor:
		return common::env::temp_storage->add_typed(static_cast<uchar_type>(value(entry) ^
			operand_value_(*driver, operand)), type_of(entry)->reflect());
	case operator_id_type::less:
		return common::env::temp_storage->add(value(entry) < operand_value_(*driver, operand));
	case operator_id_type::less_or_equal:
		return common::env::temp_storage->add(value(entry) <= operand_value_(*driver, operand));
	case operator_id_type::equality:
		return common::env::temp_storage->add(value(entry) == operand_value_(*driver, operand));
	case operator_id_type::inverse_equality:
		return common::env::temp_storage->add(value(entry) != operand_value_(*driver, operand));
	case operator_id_type::more_or_equal:
		return common::env::temp_storage->add(value(entry) >= operand_value_(*driver, operand));
	case operator_id_type::more:
		return common::env::temp_storage->add(value(entry) > operand_value_(*driver, operand));
	default:
		break;
	}

	return object::evaluate_(entry, info, operand);
}

slang::driver::object::entry_type *slang::driver::byte::evaluate_(entry_type &entry, unary_info_type &info){
	switch (info.id){
	case operator_id_type::bitwise_inverse:
		return common::env::temp_storage->add_typed(static_cast<uchar_type>(~value(entry)), type_of(entry)->reflect());
	case operator_id_type::increment:
		return evaluate_increment_(entry, true, info.is_left);
	case operator_id_type::decrement:
		return evaluate_increment_(entry, false, info.is_left);
	default:
		break;
	}

	return object::evaluate_(entry, info);
}

void slang::driver::byte::convert_(entry_type &entry, type_id_type id, char *buffer){
	if (id == type_id_type::byte)
		*reinterpret_cast<uchar_type *>(buffer) = value(entry);
	else
		object::convert_(entry, id, buffer);
}

void slang::driver::byte::echo_(entry_type &entry, writer_type &out, bool no_throw){
	out.write(common::env::to_hex(value(entry)).c_str());
}

slang::driver::object::entry_type *slang::driver::byte::evaluate_increment_(entry_type &entry, bool increment, bool lval){
	if (!is_non_const_lval_(entry))
		return nullptr;

	auto type = type_of(entry);
	auto value = this->value(entry), previous_value = value;
	if (increment)
		++value;
	else//Decrement
		--value;

	common::env::address_table.write(entry.address_value(), value);
	return lval ? &entry : common::env::temp_storage->add_typed(previous_value, type->reflect());
}

slang::driver::object::entry_type *slang::driver::byte::write_(entry_type &entry, uchar_type value){
	common::env::address_table.write(entry.address_value(), value);
	return &entry;
}

slang::driver::byte::uchar_type slang::driver::byte::operand_value_(object &driver, entry_type &operand){
	auto value = uchar_type();
	driver.convert(operand, type_id_type::byte, reinterpret_cast<char *>(&value));
	return value;
}

bool slang::driver::byte::is_non_const_lval_(entry_type &entry){
	if (!entry.is_lval()){
		common::env::error.set("Operator requires an lvalue.", true);
		return false;
	}

	if (entry.is_const()){
		common::env::error.set("Cannot modify a constant object.", true);
		return false;
	}

	return true;
}
