#include "pointer_driver.h"
#include "../common/env.h"

slang::driver::pointer::~pointer() = default;

slang::driver::object::entry_type *slang::driver::pointer::cast(entry_type &entry, type::object &type, cast_type options){
	auto value = object::cast(entry, type, options);
	if (value != nullptr || SLANG_IS(options, cast_type::ref) || type.is_ref() || common::env::error.has())
		return value;

	return SLANG_IS(options, cast_type::reinterpret) ? reinterpret_cast_(entry, type, options) : static_cast_(entry, type, options);
}

slang::driver::object::uint_type slang::driver::pointer::size_of(entry_type &entry){
	auto type = type_of(entry);
	if (type->is_string())//Return count
		return static_cast<uint_type>(std::strlen(get_string_ptr_(entry)));

	if (type->is_wstring())//Return count
		return static_cast<uint_type>(std::wcslen(reinterpret_cast<const wchar_t *>(get_string_ptr_(entry))));

	return type->size();
}

slang::driver::object::uint64_type slang::driver::pointer::pointer_target(entry_type &entry){
	return value(entry);
}

slang::driver::object::uint64_type slang::driver::pointer::value(entry_type &entry){
	return common::env::address_table.read<uint64_type>(entry.address_value());
}

slang::driver::object::address_head_type slang::driver::pointer::target(entry_type &entry){
	auto target_address = value(entry);
	if (common::env::error.has())
		return address_head_type{};

	auto target_head = common::env::address_table.get_head(target_address);
	if (common::env::error.has())
		return address_head_type{};

	if (target_head == nullptr)
		return common::env::error.set_and_return<address_head_type>("Memory access violation.");

	if (target_head->value == target_address)
		return *target_head;

	auto offset = (target_address - target_head->value);
	auto target_head_copy = *target_head;
	{//Adjust value
		target_head_copy.value += offset;
		target_head_copy.ptr += offset;
		target_head_copy.size -= offset;
	}

	return target_head_copy;
}

slang::driver::object::entry_type *slang::driver::pointer::dereference(entry_type &entry, long long offset){
	auto target_address = value(entry);
	if (common::env::error.has())
		return nullptr;

	auto type = type_of(entry)->remove_pointer()->reflect();
	if (offset < 0ll)
		target_address -= (type->size() * static_cast<uint64_type>(-offset));
	else//Forward
		target_address += (type->size() * static_cast<uint64_type>(offset));

	if (common::env::address_table.is_protected(target_address))
		return common::env::error.set_and_return<nullptr_t>("Memory access violation.", true);

	if (entry.is_const_pointer() && !type->is_const())//Create constant type
		type = std::make_shared<type::modified>(type->remove_modified()->reflect(), type_attribute_type::const_);

	return common::env::temp_storage->wrap(target_address, type, attribute_type::lval);
}

slang::driver::object::entry_type *slang::driver::pointer::evaluate_(entry_type &entry, binary_info_type &info, entry_type &operand){
	auto type = type_of(entry);
	if (type->is_string())
		return evaluate_string_(entry, info, operand);

	if (type->is_wstring())
		return evaluate_wstring_(entry, info, operand);

	auto operand_driver = get_driver(operand);
	auto operand_type = operand_driver->type_of(operand);
	if (operand_type->is_integral()){
		auto value = this->value(entry);
		if (common::env::error.has())
			return nullptr;

		switch (info.id){
		case operator_id_type::index:
			return dereference(entry, operand_driver->convert<long long>(operand));
		case operator_id_type::compound_plus:
			return evaluate_increment_(entry, true, true, operand_driver->convert<uint64_type>(operand));
		case operator_id_type::plus:
			return common::env::temp_storage->add_pointer(this->value(entry) + (type->remove_pointer()->size() *
				operand_driver->convert<uint64_type>(operand)), type->reflect());
		case operator_id_type::compound_minus:
			return evaluate_increment_(entry, false, true, operand_driver->convert<uint64_type>(operand));
		case operator_id_type::minus:
			return common::env::temp_storage->add_pointer(this->value(entry) - (type->remove_pointer()->size() *
				operand_driver->convert<uint64_type>(operand)), type->reflect());
		default:
			break;
		}

		return object::evaluate_(entry, info, operand);
	}

	auto linked_entry = operand_driver->linked_object(operand);
	if (common::env::error.has())
		return nullptr;

	if (linked_entry != nullptr){
		operand_driver = get_driver(*linked_entry);
		operand_type = operand_driver->type_of(operand);
	}
	else//No linked object
		linked_entry = &operand;

	if (!operand_type->is_strong_pointer())
		return object::evaluate_(entry, info, operand);

	if (type->remove_pointer()->score(operand_type->remove_pointer(), true, false) != SLANG_MAX_TYPE_SCORE)
		return object::evaluate_(entry, info, operand);

	switch (info.id){
	case operator_id_type::less:
		return common::env::temp_storage->add(value(entry) < operand_driver->pointer_target(operand));
	case operator_id_type::less_or_equal:
		return common::env::temp_storage->add(value(entry) <= operand_driver->pointer_target(operand));
	case operator_id_type::equality:
		return common::env::temp_storage->add(value(entry) == operand_driver->pointer_target(operand));
	case operator_id_type::inverse_equality:
		return common::env::temp_storage->add(value(entry) != operand_driver->pointer_target(operand));
	case operator_id_type::more_or_equal:
		return common::env::temp_storage->add(value(entry) >= operand_driver->pointer_target(operand));
	case operator_id_type::more:
		return common::env::temp_storage->add(value(entry) > operand_driver->pointer_target(operand));
	default:
		break;
	}

	return object::evaluate_(entry, info, operand);
}

slang::driver::object::entry_type *slang::driver::pointer::evaluate_(entry_type &entry, unary_info_type &info){
	switch (info.id){
	case operator_id_type::decrement:
		return evaluate_increment_(entry, false, info.is_left);
	case operator_id_type::increment:
		return evaluate_increment_(entry, true, info.is_left);
	default:
		break;
	}

	if (info.is_left && info.id == lexer::operator_id::times)
		return dereference(entry);

	return object::evaluate_(entry, info);
}

slang::driver::object::entry_type *slang::driver::pointer::assign_(entry_type &entry, entry_type &value){
	auto driver = get_driver(value);
	auto type = type_of(entry), value_type = driver->type_of(value);
	if (entry.is(attribute_type::block_aligned) && value.is(attribute_type::block_aligned)){
		if (type->is_const_string() && value_type->is_const_string()){
			if (assign_str_(entry, value, *driver) != nullptr)
				return &entry;
		}
		else if (type->is_const_wstring() && value_type->is_const_wstring()){
			if (assign_str_(entry, value, *driver) != nullptr)
				return &entry;
		}
	}

	auto compatible_value = driver->cast(value, *type);
	if (compatible_value == nullptr || common::env::error.has())
		return common::env::error.set_and_return<nullptr_t>("Object is not compatible with target type", true);

	auto head = entry.cached_address_head();
	common::env::address_table.copy(head->value, driver->address_of(value), head->size);
	SLANG_REMOVE(head->attributes, address_attribute_type::is_string);

	return &entry;
}

void slang::driver::pointer::convert_(entry_type &entry, type_id_type id, char *buffer){
	if (id == type_id_type::string_ && type_of(entry)->is_string())
		*reinterpret_cast<char **>(buffer) = get_string_ptr_(entry);
	else if (id == type_id_type::wstring_ && type_of(entry)->is_wstring())
		*reinterpret_cast<wchar_t **>(buffer) = reinterpret_cast<wchar_t *>(get_string_ptr_(entry));
	else//No conversion
		object::convert_(entry, id, buffer);
}

void slang::driver::pointer::echo_(entry_type &entry, writer_type &out, bool no_throw){
	auto type = type_of(entry);
	if (type->is_string()){//Echo string
		out.write(get_string_ptr_(entry));
		return;
	}

	if (type->is_wstring()){//Echo narrow string
		out.write(reinterpret_cast<wchar_t *>(get_string_ptr_(entry)));
		return;
	}

	auto value = this->value(entry);
	if (value == 0u)
		return object::echo_(entry, out, no_throw);

	entry_type target_entry(nullptr, value, type->remove_pointer()->reflect());
	if (common::env::error.has())
		return;

	out.begin();
	out.write("<");
	out.write(common::env::to_hex(value).c_str());
	out.write(": ");
	get_driver(target_entry)->echo(target_entry, out, no_throw);
	out.write(">");
	out.end(true);
}

slang::driver::object::entry_type *slang::driver::pointer::cast_(const char *value, bool is_wide, type_id_type id){
	try{
		switch (id){
		case type_id_type::char_:
			if (!is_wide)
				return common::env::temp_storage->add(static_cast<char>(std::stoi(value)));
			return common::env::temp_storage->add(static_cast<char>(std::stoi(reinterpret_cast<const wchar_t *>(value))));
		case type_id_type::uchar:
			if (!is_wide)
				return common::env::temp_storage->add(static_cast<unsigned char>(std::stoi(value)));
			return common::env::temp_storage->add(static_cast<unsigned char>(std::stoi(reinterpret_cast<const wchar_t *>(value))));
		case type_id_type::short_:
			if (!is_wide)
				return common::env::temp_storage->add(static_cast<short>(std::stoi(value)));
			return common::env::temp_storage->add(static_cast<short>(std::stoi(reinterpret_cast<const wchar_t *>(value))));
		case type_id_type::ushort:
			if (!is_wide)
				return common::env::temp_storage->add(static_cast<unsigned short>(std::stoi(value)));
			return common::env::temp_storage->add(static_cast<unsigned short>(std::stoi(reinterpret_cast<const wchar_t *>(value))));
		case type_id_type::int_:
			if (!is_wide)
				return common::env::temp_storage->add(std::stoi(value));
			return common::env::temp_storage->add(std::stoi(reinterpret_cast<const wchar_t *>(value)));
		case type_id_type::uint:
			if (!is_wide)
				return common::env::temp_storage->add(static_cast<unsigned int>(std::stoi(value)));
			return common::env::temp_storage->add(static_cast<unsigned int>(std::stoi(reinterpret_cast<const wchar_t *>(value))));
		case type_id_type::long_:
			if (!is_wide)
				return common::env::temp_storage->add(std::stol(value));
			return common::env::temp_storage->add(std::stol(reinterpret_cast<const wchar_t *>(value)));
		case type_id_type::ulong:
			if (!is_wide)
				return common::env::temp_storage->add(std::stoul(value));
			return common::env::temp_storage->add(std::stoul(reinterpret_cast<const wchar_t *>(value)));
		case type_id_type::llong:
			if (!is_wide)
				return common::env::temp_storage->add(std::stoll(value));
			return common::env::temp_storage->add(std::stoll(reinterpret_cast<const wchar_t *>(value)));
		case type_id_type::ullong:
			if (!is_wide)
				return common::env::temp_storage->add(std::stoull(value));
			return common::env::temp_storage->add(std::stoull(reinterpret_cast<const wchar_t *>(value)));
		case type_id_type::float_:
			if (!is_wide)
				return common::env::temp_storage->add(std::stof(value));
			return common::env::temp_storage->add(std::stof(reinterpret_cast<const wchar_t *>(value)));
		case type_id_type::double_:
			if (!is_wide)
				return common::env::temp_storage->add(std::stod(value));
			return common::env::temp_storage->add(std::stod(reinterpret_cast<const wchar_t *>(value)));
		case type_id_type::ldouble:
			if (!is_wide)
				return common::env::temp_storage->add(std::stold(value));
			return common::env::temp_storage->add(std::stold(reinterpret_cast<const wchar_t *>(value)));
		default:
			break;
		}
	}
	catch (...){
		return nullptr;
	}

	return nullptr;
}

slang::driver::object::entry_type *slang::driver::pointer::cast_(entry_type &entry, type_id_type id){
	switch (id){
	case type_id_type::char_:
		return common::env::temp_storage->add(static_cast<char>(value(entry)));
	case type_id_type::uchar:
		return common::env::temp_storage->add(static_cast<unsigned char>(value(entry)));
	case type_id_type::short_:
		return common::env::temp_storage->add(static_cast<short>(value(entry)));
	case type_id_type::ushort:
		return common::env::temp_storage->add(static_cast<unsigned short>(value(entry)));
	case type_id_type::int_:
		return common::env::temp_storage->add(static_cast<int>(value(entry)));
	case type_id_type::uint:
		return common::env::temp_storage->add(static_cast<unsigned int>(value(entry)));
	case type_id_type::long_:
		return common::env::temp_storage->add(static_cast<long>(value(entry)));
	case type_id_type::ulong:
		return common::env::temp_storage->add(static_cast<unsigned long>(value(entry)));
	case type_id_type::llong:
		return common::env::temp_storage->add(static_cast<long long>(value(entry)));
	case type_id_type::ullong:
		return common::env::temp_storage->add(static_cast<unsigned long long>(value(entry)));
	case type_id_type::float_:
		return common::env::temp_storage->add(static_cast<float>(value(entry)));
	case type_id_type::double_:
		return common::env::temp_storage->add(static_cast<double>(value(entry)));
	case type_id_type::ldouble:
		return common::env::temp_storage->add(static_cast<long double>(value(entry)));
	default:
		break;
	}

	return nullptr;
}

slang::driver::object::entry_type *slang::driver::pointer::static_cast_(entry_type &entry, type::object &type, cast_type options){
	if (type_of(entry)->is_string() && type.is_numeric())
		return cast_(get_string_ptr_(entry), false, type.id());

	if (type_of(entry)->is_wstring() && type.is_numeric())
		return cast_(get_string_ptr_(entry), true, type.id());

	return nullptr;
}

slang::driver::object::entry_type *slang::driver::pointer::reinterpret_cast_(entry_type &entry, type::object &type, cast_type options){
	if (type.is_strong_pointer()){//Convert to a different pointer type
		if ((entry.is_const() || type_of(entry)->is_const()) && !type.is_const())
			return nullptr;//Cannot cast away constness

		return common::env::temp_storage->add_pointer(value(entry), type.reflect());
	}

	return type.is_numeric() ? cast_(entry, type.id()) : nullptr;
}

slang::driver::object::entry_type *slang::driver::pointer::evaluate_increment_(entry_type &entry, bool increment, bool lval, uint64_type mult){
	if (!entry.is_lval())
		return common::env::error.set_and_return<nullptr_t>("Operator requires an lvalue.", true);

	if (entry.is_const())
		return common::env::error.set_and_return<nullptr_t>("Cannot modify a constant object.", true);

	auto type = type_of(entry);
	auto value = this->value(entry), previous_value = value;
	if (increment){
		if ((value += (type->remove_pointer()->size() * mult)) == previous_value)
			++value;
	}
	else if ((value -= (type->remove_pointer()->size() * mult)) == previous_value)//Decrement
		--value;

	common::env::address_table.write(entry.address_value(), value);
	return lval ? &entry : common::env::temp_storage->add_pointer(previous_value, type->reflect());
}

slang::driver::object::entry_type *slang::driver::pointer::evaluate_string_(entry_type &entry, binary_info_type &info, entry_type &operand){
	auto rhs = get_driver(operand)->to_string(operand);
	if (common::env::error.has())
		return nullptr;

	switch (info.id){
	case operator_id_type::plus:
		return common::env::temp_storage->add((get_string_ptr_(entry) + rhs).c_str());
	case operator_id_type::less:
		return common::env::temp_storage->add(std::string_view(get_string_ptr_(entry)) < rhs);
	case operator_id_type::less_or_equal:
		return common::env::temp_storage->add(std::string_view(get_string_ptr_(entry)) <= rhs);
	case operator_id_type::equality:
		return common::env::temp_storage->add(std::string_view(get_string_ptr_(entry)) == rhs);
	case operator_id_type::inverse_equality:
		return common::env::temp_storage->add(std::string_view(get_string_ptr_(entry)) != rhs);
	case operator_id_type::more_or_equal:
		return common::env::temp_storage->add(std::string_view(get_string_ptr_(entry)) >= rhs);
	case operator_id_type::more:
		return common::env::temp_storage->add(std::string_view(get_string_ptr_(entry)) > rhs);
	default:
		break;
	}

	return object::evaluate_(entry, info, operand);
}

slang::driver::object::entry_type *slang::driver::pointer::evaluate_wstring_(entry_type &entry, binary_info_type &info, entry_type &operand){
	auto rhs = get_driver(operand)->to_wstring(operand);
	if (common::env::error.has())
		return nullptr;

	switch (info.id){
	case operator_id_type::plus:
		return common::env::temp_storage->add((reinterpret_cast<wchar_t *>(get_string_ptr_(entry)) + rhs).c_str());
	case operator_id_type::less:
		return common::env::temp_storage->add(std::wstring_view(reinterpret_cast<wchar_t *>(get_string_ptr_(entry))) < rhs);
	case operator_id_type::less_or_equal:
		return common::env::temp_storage->add(std::wstring_view(reinterpret_cast<wchar_t *>(get_string_ptr_(entry))) <= rhs);
	case operator_id_type::equality:
		return common::env::temp_storage->add(std::wstring_view(reinterpret_cast<wchar_t *>(get_string_ptr_(entry))) == rhs);
	case operator_id_type::inverse_equality:
		return common::env::temp_storage->add(std::wstring_view(reinterpret_cast<wchar_t *>(get_string_ptr_(entry))) != rhs);
	case operator_id_type::more_or_equal:
		return common::env::temp_storage->add(std::wstring_view(reinterpret_cast<wchar_t *>(get_string_ptr_(entry))) >= rhs);
	case operator_id_type::more:
		return common::env::temp_storage->add(std::wstring_view(reinterpret_cast<wchar_t *>(get_string_ptr_(entry))) > rhs);
	default:
		break;
	}

	return object::evaluate_(entry, info, operand);
}

slang::driver::object::entry_type *slang::driver::pointer::assign_str_(entry_type &entry, entry_type &value, object &driver){
	if (common::env::error.has())
		return &entry;

	auto value_head = driver.address_head_of(value);
	if (value_head == nullptr || !SLANG_IS(value_head->attributes, address_attribute_type::is_string))
		return nullptr;

	auto string_head = common::env::address_table.find(common::env::address_table.read<uint64_type>(value_head->value));
	if (string_head == nullptr){
		common::env::error.clear();//Clear any errors raised
		return nullptr;
	}

	auto head = entry.cached_address_head();
	common::env::address_table.copy(head->value, value_head->value, head->size);

	++string_head->ref_count;
	SLANG_SET(head->attributes, address_attribute_type::is_string);

	return &entry;
}

char *slang::driver::pointer::get_string_ptr_(entry_type &entry){
	auto target = this->target(entry);
	return (common::env::error.has() ? nullptr : target.ptr);
}
