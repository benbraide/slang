#include "pointer_driver.h"
#include "../common/env.h"

slang::driver::pointer::~pointer() = default;

slang::driver::object::entry_type *slang::driver::pointer::cast(entry_type &entry, type::object &type, cast_type options){
	auto value = object::cast(entry, type, options);
	if (value != nullptr || SLANG_IS(options, cast_type::ref) || type.is_ref() || common::env::error.has())
		return value;

	return SLANG_IS(options, cast_type::reinterpret) ? reinterpret_cast_(entry, type, options) : static_cast_(entry, type, options);
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

slang::driver::object::entry_type *slang::driver::pointer::evaluate_(entry_type &entry, binary_info_type &info, entry_type &operand){
	return nullptr;
}

slang::driver::object::entry_type *slang::driver::pointer::evaluate_(entry_type &entry, unary_info_type &info){
	return nullptr;
}

slang::driver::object::entry_type *slang::driver::pointer::assign_(entry_type &entry, entry_type &value){
	return nullptr;
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
	if (type_of(entry)->is_string()){//Echo string
		out.write(get_string_ptr_(entry));
		return;
	}

	if (type_of(entry)->is_wstring()){//Echo narrow string
		out.write(reinterpret_cast<wchar_t *>(get_string_ptr_(entry)));
		return;
	}

	auto value = this->value(entry);
	if (value == 0u)
		return object::echo_(entry, out, no_throw);

	entry_type target_entry(nullptr, value, type_of(entry)->remove_pointer()->reflect());
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

char *slang::driver::pointer::get_string_ptr_(entry_type &entry){
	auto target = this->target(entry);
	return (common::env::error.has() ? nullptr : target.ptr);
}
