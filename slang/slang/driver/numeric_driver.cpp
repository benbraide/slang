#include "numeric_driver.h"
#include "../common/env.h"

slang::driver::numeric::~numeric() = default;

slang::driver::object::entry_type *slang::driver::numeric::cast(entry_type &entry, type::object &type, cast_type options){
	auto value = object::cast(entry, type, options);
	if (value != nullptr || SLANG_IS(options, cast_type::ref) || type.is_ref() || common::env::error.has())
		return value;

	return SLANG_IS(options, cast_type::reinterpret) ? reinterpret_cast_(entry, type, options) : static_cast_(entry, type, options);
}

std::string slang::driver::numeric::to_string(entry_type &entry){
	if (entry.is(attribute_type::nan_))
		return "nan";

	switch (type_of(entry)->id()){
	case type_id_type::char_:
		return std::to_string(common::env::address_table.read<char>(entry.address_value()));
	case type_id_type::uchar:
		return std::to_string(common::env::address_table.read<unsigned char>(entry.address_value()));
	case type_id_type::short_:
		return std::to_string(common::env::address_table.read<short>(entry.address_value()));
	case type_id_type::ushort:
		return std::to_string(common::env::address_table.read<unsigned short>(entry.address_value()));
	case type_id_type::int_:
		return std::to_string(common::env::address_table.read<int>(entry.address_value()));
	case type_id_type::uint:
		return std::to_string(common::env::address_table.read<unsigned int>(entry.address_value()));
	case type_id_type::long_:
		return std::to_string(common::env::address_table.read<long>(entry.address_value()));
	case type_id_type::ulong:
		return std::to_string(common::env::address_table.read<unsigned long>(entry.address_value()));
	case type_id_type::llong:
		return std::to_string(common::env::address_table.read<long long>(entry.address_value()));
	case type_id_type::ullong:
		return std::to_string(common::env::address_table.read<unsigned long long>(entry.address_value()));
	case type_id_type::float_:
		return common::env::real_to_string(common::env::address_table.read<float>(entry.address_value()));
	case type_id_type::double_:
		return common::env::real_to_string(common::env::address_table.read<double>(entry.address_value()));
	case type_id_type::ldouble:
		return common::env::real_to_string(common::env::address_table.read<long double>(entry.address_value()));
	default:
		break;
	}

	return object::to_string(entry);
}

std::wstring slang::driver::numeric::to_wstring(entry_type &entry){
	if (entry.is(attribute_type::nan_))
		return L"nan";

	switch (type_of(entry)->id()){
	case type_id_type::char_:
		return std::to_wstring(common::env::address_table.read<char>(entry.address_value()));
	case type_id_type::uchar:
		return std::to_wstring(common::env::address_table.read<unsigned char>(entry.address_value()));
	case type_id_type::short_:
		return std::to_wstring(common::env::address_table.read<short>(entry.address_value()));
	case type_id_type::ushort:
		return std::to_wstring(common::env::address_table.read<unsigned short>(entry.address_value()));
	case type_id_type::int_:
		return std::to_wstring(common::env::address_table.read<int>(entry.address_value()));
	case type_id_type::uint:
		return std::to_wstring(common::env::address_table.read<unsigned int>(entry.address_value()));
	case type_id_type::long_:
		return std::to_wstring(common::env::address_table.read<long>(entry.address_value()));
	case type_id_type::ulong:
		return std::to_wstring(common::env::address_table.read<unsigned long>(entry.address_value()));
	case type_id_type::llong:
		return std::to_wstring(common::env::address_table.read<long long>(entry.address_value()));
	case type_id_type::ullong:
		return std::to_wstring(common::env::address_table.read<unsigned long long>(entry.address_value()));
	case type_id_type::float_:
		return common::env::real_to_wstring(common::env::address_table.read<float>(entry.address_value()));
	case type_id_type::double_:
		return common::env::real_to_wstring(common::env::address_table.read<double>(entry.address_value()));
	case type_id_type::ldouble:
		return common::env::real_to_wstring(common::env::address_table.read<long double>(entry.address_value()));
	default:
		break;
	}

	return object::to_wstring(entry);
}

slang::driver::object::entry_type *slang::driver::numeric::evaluate_(entry_type &entry, binary_info_type &info, entry_type &operand){
	auto target_operand = &operand;
	auto type = type_of(entry), operand_type = get_driver(operand)->type_of(operand);

	auto operand_is_nan = target_operand->is(attribute_type::nan_);
	if (!operand_type->is_numeric() && !operand_is_nan){
		if (operand_type->is_string()){
			switch (info.id){
			case operator_id_type::plus:
				return common::env::temp_storage->add((to_string(entry) + get_driver(operand)->to_string(operand)).c_str());
			case operator_id_type::less:
				return common::env::temp_storage->add(to_string(entry) < get_driver(operand)->to_string(operand));
			case operator_id_type::less_or_equal:
				return common::env::temp_storage->add(to_string(entry) <= get_driver(operand)->to_string(operand));
			case operator_id_type::equality:
				return common::env::temp_storage->add(to_string(entry) == get_driver(operand)->to_string(operand));
			case operator_id_type::inverse_equality:
				return common::env::temp_storage->add(to_string(entry) != get_driver(operand)->to_string(operand));
			case operator_id_type::more_or_equal:
				return common::env::temp_storage->add(to_string(entry) >= get_driver(operand)->to_string(operand));
			case operator_id_type::more:
				return common::env::temp_storage->add(to_string(entry) > get_driver(operand)->to_string(operand));
			default:
				break;
			}

			return object::evaluate_(entry, info, operand);
		}

		if (operand_type->is_wstring()){
			switch (info.id){
			case operator_id_type::plus:
				return common::env::temp_storage->add((to_wstring(entry) + get_driver(operand)->to_wstring(operand)).c_str());
			case operator_id_type::less:
				return common::env::temp_storage->add(to_wstring(entry) < get_driver(operand)->to_wstring(operand));
			case operator_id_type::less_or_equal:
				return common::env::temp_storage->add(to_wstring(entry) <= get_driver(operand)->to_wstring(operand));
			case operator_id_type::equality:
				return common::env::temp_storage->add(to_wstring(entry) == get_driver(operand)->to_wstring(operand));
			case operator_id_type::inverse_equality:
				return common::env::temp_storage->add(to_wstring(entry) != get_driver(operand)->to_wstring(operand));
			case operator_id_type::more_or_equal:
				return common::env::temp_storage->add(to_wstring(entry) >= get_driver(operand)->to_wstring(operand));
			case operator_id_type::more:
				return common::env::temp_storage->add(to_wstring(entry) > get_driver(operand)->to_wstring(operand));
			default:
				break;
			}

			return object::evaluate_(entry, info, operand);
		}

		if ((target_operand = get_driver(operand)->cast(operand, *type)) == nullptr)
			return object::evaluate_(entry, info, operand);
	}

	auto is_nan = entry.is(attribute_type::nan_);
	if (is_nan || operand_is_nan){
		switch (info.id){
		case operator_id_type::compound_plus:
		case operator_id_type::compound_minus:
		case operator_id_type::compound_times:
		case operator_id_type::compound_divide:
			return is_nan ? &entry : write_nan_(entry, true);
		case operator_id_type::plus:
		case operator_id_type::minus:
		case operator_id_type::times:
		case operator_id_type::divide:
			return common::env::temp_storage->nan();
		case operator_id_type::less:
			return get_temp_storage()->add(false);
		case operator_id_type::less_or_equal:
			return get_temp_storage()->add(is_nan == operand_is_nan);
		case operator_id_type::equality:
			return get_temp_storage()->add(is_nan == operand_is_nan);
		case operator_id_type::inverse_equality:
			return get_temp_storage()->add(is_nan != operand_is_nan);
		case operator_id_type::more_or_equal:
			return get_temp_storage()->add(is_nan == operand_is_nan);
		case operator_id_type::more:
			return get_temp_storage()->add(false);
		default:
			break;
		}

		if (!type->is_integral() || !operand_type->is_integral())
			return object::evaluate_(entry, info, operand);

		switch (info.id){
		case operator_id_type::compound_modulus:
		case operator_id_type::compound_left_shift:
		case operator_id_type::compound_right_shift:
		case operator_id_type::compound_bitwise_and:
		case operator_id_type::compound_bitwise_or:
		case operator_id_type::compound_bitwise_xor:
			return is_nan ? &entry : write_nan_(entry, true);
		case operator_id_type::modulus:
		case operator_id_type::left_shift:
		case operator_id_type::right_shift:
		case operator_id_type::bitwise_and:
		case operator_id_type::bitwise_or:
		case operator_id_type::bitwise_xor:
			return common::env::temp_storage->nan();
		default:
			break;
		}

		return object::evaluate_(entry, info, operand);
	}

	type::object *target_type;
	if (info.id < operator_id_type::compound_bitwise_or || info.id > operator_id_type::compound_modulus){
		if (type->is_floating_point() == operand_type->is_floating_point())
			target_type = (type->size() < operand_type->size()) ? operand_type : type;
		else//Floating point mismatch
			target_type = type->is_floating_point() ? type : operand_type;
	}
	else//Compound operator
		target_type = type;

	switch (target_type->id()){
	case type_id_type::char_:
		return evaluate_integral_<char>(entry, info, operand);
	case type_id_type::uchar:
		return evaluate_integral_<unsigned char>(entry, info, operand);
	case type_id_type::short_:
		return evaluate_integral_<short>(entry, info, operand);
	case type_id_type::ushort:
		return evaluate_integral_<unsigned short>(entry, info, operand);
	case type_id_type::int_:
		return evaluate_integral_<int>(entry, info, operand);
	case type_id_type::uint:
		return evaluate_integral_<unsigned int>(entry, info, operand);
	case type_id_type::long_:
		return evaluate_integral_<long>(entry, info, operand);
	case type_id_type::ulong:
		return evaluate_integral_<unsigned long>(entry, info, operand);
	case type_id_type::llong:
		return evaluate_integral_<long long>(entry, info, operand);
	case type_id_type::ullong:
		return evaluate_integral_<unsigned long long>(entry, info, operand);
	case type_id_type::float_:
		return evaluate_binary_<float>(entry, info, operand);
	case type_id_type::double_:
		return evaluate_binary_<double>(entry, info, operand);
	case type_id_type::ldouble:
		return evaluate_binary_<long double>(entry, info, operand);
	default:
		break;
	}

	return object::evaluate_(entry, info, operand);
}

slang::driver::object::entry_type *slang::driver::numeric::evaluate_(entry_type &entry, unary_info_type &info){
	switch (type_of(entry)->id()){
	case type_id_type::char_:
		return evaluate_signed_integral_<char>(entry, info);
	case type_id_type::uchar:
		return evaluate_unsigned_integral_<unsigned char>(entry, info);
	case type_id_type::short_:
		return evaluate_signed_integral_<short>(entry, info);
	case type_id_type::ushort:
		return evaluate_unsigned_integral_<unsigned short>(entry, info);
	case type_id_type::int_:
		return evaluate_signed_integral_<int>(entry, info);
	case type_id_type::uint:
		return evaluate_unsigned_integral_<unsigned int>(entry, info);
	case type_id_type::long_:
		return evaluate_signed_integral_<long>(entry, info);
	case type_id_type::ulong:
		return evaluate_unsigned_integral_<unsigned long>(entry, info);
	case type_id_type::llong:
		return evaluate_signed_integral_<long long>(entry, info);
	case type_id_type::ullong:
		return evaluate_unsigned_integral_<unsigned long long>(entry, info);
	case type_id_type::float_:
		return evaluate_signed_<float>(entry, info);
	case type_id_type::double_:
		return evaluate_signed_<double>(entry, info);
	case type_id_type::ldouble:
		return evaluate_signed_<long double>(entry, info);
	default:
		break;
	}

	return object::evaluate_(entry, info);
}

slang::driver::object::entry_type *slang::driver::numeric::assign_(entry_type &entry, entry_type &value){
	auto driver = get_driver(value);
	auto type = type_of(entry), value_type = driver->type_of(value);
	if (type->is_nan() || value.is(attribute_type::nan_) || (value_type->is_numeric() && value_type->id() == type->id())){
		auto head = entry.cached_address_head();
		auto value_size = value_type->size();

		if (!value.is(attribute_type::nan_)){
			common::env::address_table.copy(head->value, driver->address_of(value), (head->size < value_size) ? head->size : value_size);
			entry.remove_attributes(attribute_type::nan_);
		}
		else//Set nan flag if applicable
			entry.add_attributes(attribute_type::nan_);

		return &entry;
	}

	auto compatible_value = driver->cast(value, *type);
	if (compatible_value == nullptr || common::env::error.has())
		return common::env::error.set_and_return<nullptr_t>("Object is not compatible with target type", true);

	auto head = entry.cached_address_head();
	common::env::address_table.copy(head->value, driver->address_of(value), head->size);
	entry.remove_attributes(attribute_type::nan_);

	return &entry;
}

void slang::driver::numeric::convert_(entry_type &entry, type_id_type id, char *buffer){
	switch (id){
	case type_id_type::char_:
		common::env::address_table.convert_numeric(entry.address_value(), entry.type()->is_floating_point(),
			*reinterpret_cast<char *>(buffer));
		break;
	case type_id_type::uchar:
		common::env::address_table.convert_numeric(entry.address_value(), entry.type()->is_floating_point(),
			*reinterpret_cast<unsigned char *>(buffer));
		break;
	case type_id_type::short_:
		common::env::address_table.convert_numeric(entry.address_value(), entry.type()->is_floating_point(),
			*reinterpret_cast<short *>(buffer));
		break;
	case type_id_type::ushort:
		common::env::address_table.convert_numeric(entry.address_value(), entry.type()->is_floating_point(),
			*reinterpret_cast<unsigned short *>(buffer));
		break;
	case type_id_type::int_:
		common::env::address_table.convert_numeric(entry.address_value(), entry.type()->is_floating_point(),
			*reinterpret_cast<int *>(buffer));
		break;
	case type_id_type::uint:
		common::env::address_table.convert_numeric(entry.address_value(), entry.type()->is_floating_point(),
			*reinterpret_cast<unsigned int *>(buffer));
		break;
	case type_id_type::long_:
		common::env::address_table.convert_numeric(entry.address_value(), entry.type()->is_floating_point(),
			*reinterpret_cast<long *>(buffer));
		break;
	case type_id_type::ulong:
		common::env::address_table.convert_numeric(entry.address_value(), entry.type()->is_floating_point(),
			*reinterpret_cast<unsigned long *>(buffer));
		break;
	case type_id_type::llong:
		common::env::address_table.convert_numeric(entry.address_value(), entry.type()->is_floating_point(),
			*reinterpret_cast<long long *>(buffer));
		break;
	case type_id_type::ullong:
		common::env::address_table.convert_numeric(entry.address_value(), entry.type()->is_floating_point(),
			*reinterpret_cast<unsigned long long *>(buffer));
		break;
	case type_id_type::float_:
		common::env::address_table.convert_numeric(entry.address_value(), entry.type()->is_floating_point(),
			*reinterpret_cast<float *>(buffer));
		break;
	case type_id_type::double_:
		common::env::address_table.convert_numeric(entry.address_value(), entry.type()->is_floating_point(),
			*reinterpret_cast<double *>(buffer));
		break;
	case type_id_type::ldouble:
		common::env::address_table.convert_numeric(entry.address_value(), entry.type()->is_floating_point(),
			*reinterpret_cast<long double *>(buffer));
		break;
	default:
		object::convert_(entry, id, buffer);
		break;
	}
}

void slang::driver::numeric::echo_(entry_type &entry, writer_type &out, bool no_throw){
	auto value = (prefix_(entry) + to_string(entry) + suffix_(entry));
	out.write(value.c_str(), value.size());
}

slang::driver::object::entry_type *slang::driver::numeric::cast_(entry_type &entry, type_id_type id){
	switch (id){
	case type_id_type::char_:
		return common::env::temp_storage->add(common::env::address_table.convert_numeric<char>(entry.address_value(),
			type_of(entry)->is_floating_point()));
	case type_id_type::uchar:
		return common::env::temp_storage->add(common::env::address_table.convert_numeric<unsigned char>(entry.address_value(),
			type_of(entry)->is_floating_point()));
	case type_id_type::short_:
		return common::env::temp_storage->add(common::env::address_table.convert_numeric<short>(entry.address_value(),
			type_of(entry)->is_floating_point()));
	case type_id_type::ushort:
		return common::env::temp_storage->add(common::env::address_table.convert_numeric<unsigned short>(entry.address_value(),
			type_of(entry)->is_floating_point()));
	case type_id_type::int_:
		return common::env::temp_storage->add(common::env::address_table.convert_numeric<int>(entry.address_value(),
			type_of(entry)->is_floating_point()));
	case type_id_type::uint:
		return common::env::temp_storage->add(common::env::address_table.convert_numeric<unsigned int>(entry.address_value(),
			type_of(entry)->is_floating_point()));
	case type_id_type::long_:
		return common::env::temp_storage->add(common::env::address_table.convert_numeric<long>(entry.address_value(),
			type_of(entry)->is_floating_point()));
	case type_id_type::ulong:
		return common::env::temp_storage->add(common::env::address_table.convert_numeric<unsigned long>(entry.address_value(),
			type_of(entry)->is_floating_point()));
	case type_id_type::llong:
		return common::env::temp_storage->add(common::env::address_table.convert_numeric<long long>(entry.address_value(),
			type_of(entry)->is_floating_point()));
	case type_id_type::ullong:
		return common::env::temp_storage->add(common::env::address_table.convert_numeric<unsigned long long>(entry.address_value(),
			type_of(entry)->is_floating_point()));
	case type_id_type::float_:
		return common::env::temp_storage->add(common::env::address_table.convert_numeric<float>(entry.address_value(),
			type_of(entry)->is_floating_point()));
	case type_id_type::double_:
		return common::env::temp_storage->add(common::env::address_table.convert_numeric<double>(entry.address_value(),
			type_of(entry)->is_floating_point()));
	case type_id_type::ldouble:
		return common::env::temp_storage->add(common::env::address_table.convert_numeric<long double>(entry.address_value(),
			type_of(entry)->is_floating_point()));
	default:
		break;
	}

	return nullptr;
}

slang::driver::object::entry_type *slang::driver::numeric::static_cast_(entry_type &entry, type::object &type, cast_type options){
	auto value = cast_(entry, type.id());
	if (value != nullptr)
		return value;

	if (type.id() == type_id_type::byte){
		if (!SLANG_IS(options, cast_type::is_explicit) || !type.is_numeric())
			return nullptr;
		return nullptr;
		//return common::env::create(const_cast<e::type::base *>(type)->get_ptr(), get_value_<unsigned char, common::env>(entry));
	}

	if (type.is_const_string()){
		auto value = to_string(entry);
		if (common::env::error.has())
			return nullptr;

		return common::env::temp_storage->add(value.c_str(), value.size() + 1u);
	}

	return nullptr;
}

slang::driver::object::entry_type *slang::driver::numeric::reinterpret_cast_(entry_type &entry, type::object &type, cast_type options){
	if (!type.is_numeric())
		return nullptr;

	if (type.is_strong_pointer())//Cast converted value
		return common::env::temp_storage->add_pointer(convert<uint64_type>(entry), type.reflect());

	return nullptr;
}

slang::driver::object::entry_type *slang::driver::numeric::write_nan_(entry_type &entry, bool assign){
	if (assign){
		entry.add_attributes(attribute_type::nan_);
		return &entry;
	}

	return common::env::temp_storage->nan();
}

std::string slang::driver::numeric::prefix_(entry_type &entry){
	switch (type_of(entry)->id()){
	case type_id_type::char_:
	case type_id_type::uchar:
		return "'";
	default:
		break;
	}

	return "";
}

std::string slang::driver::numeric::suffix_(entry_type &entry){
	switch (type_of(entry)->id()){
	case type_id_type::char_:
		return "'";
	case type_id_type::uchar:
		return "'u";
	case type_id_type::short_:
		return "h";
	case type_id_type::ushort:
		return "uh";
	case type_id_type::uint:
		return "u";
	case type_id_type::long_:
		return "l";
	case type_id_type::ulong:
		return "ul";
	case type_id_type::llong:
		return "ll";
	case type_id_type::ullong:
		return "ull";
	case type_id_type::float_:
		return "f";
	case type_id_type::ldouble:
		return "l";
	default:
		break;
	}

	return "";
}
