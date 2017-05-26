#include "primitive_type.h"
#include "../common/env.h"

slang::type::primitive::primitive(id_type id)
	: id_(id){
	switch (id_){
	case id_type::void_:
		name_ = "void";
		size_ = static_cast<size_type>(0);
		break;
	case id_type::any:
		name_ = "any";
		size_ = static_cast<size_type>(sizeof(address::table::uint64_type));
		break;
	case id_type::auto_:
		name_ = "auto";
		size_ = static_cast<size_type>(0);
		break;
	case id_type::bool_:
		name_ = "bool";
		size_ = static_cast<size_type>(sizeof(char));
		break;
	case id_type::bit:
		name_ = "bit";
		size_ = static_cast<size_type>(0);
		break;
	case id_type::byte:
		name_ = "byte";
		size_ = static_cast<size_type>(sizeof(unsigned char));
		break;
	case id_type::char_:
		name_ = "char";
		size_ = static_cast<size_type>(sizeof(char));
		break;
	case id_type::uchar:
		name_ = "unsigned char";
		size_ = static_cast<size_type>(sizeof(unsigned char));
		break;
	case id_type::wchar:
		name_ = "wchar";
		size_ = static_cast<size_type>(sizeof(wchar_t));
		break;
	case id_type::short_:
		name_ = "short";
		size_ = static_cast<size_type>(sizeof(short));
		break;
	case id_type::ushort:
		name_ = "unsigned short";
		size_ = static_cast<size_type>(sizeof(unsigned short));
		break;
	case id_type::int_:
		name_ = "int";
		size_ = static_cast<size_type>(sizeof(int));
		break;
	case id_type::uint:
		name_ = "unsigned int";
		size_ = static_cast<size_type>(sizeof(unsigned int));
		break;
	case id_type::long_:
		name_ = "long";
		size_ = static_cast<size_type>(sizeof(long));
		break;
	case id_type::ulong:
		name_ = "unsigned long";
		size_ = static_cast<size_type>(sizeof(unsigned long));
		break;
	case id_type::llong:
		name_ = "long long";
		size_ = static_cast<size_type>(sizeof(long long));
		break;
	case id_type::ullong:
		name_ = "unsigned long long";
		size_ = static_cast<size_type>(sizeof(unsigned long long));
		break;
	case id_type::float_:
		name_ = "float";
		size_ = static_cast<size_type>(sizeof(float));
		break;
	case id_type::double_:
		name_ = "double";
		size_ = static_cast<size_type>(sizeof(double));
		break;
	case id_type::ldouble:
		name_ = "long double";
		size_ = static_cast<size_type>(sizeof(long double));
		break;
	case id_type::nullptr_:
		name_ = "nullptr_t";
		size_ = static_cast<size_type>(sizeof(address::table::uint64_type));
		break;
	case id_type::type_:
		name_ = "type_t";
		size_ = static_cast<size_type>(sizeof(address::table::uint64_type));
		break;
	case id_type::pointer:
		name_ = "pointer_t";
		size_ = static_cast<size_type>(sizeof(address::table::uint64_type));
		break;
	case id_type::array_:
		name_ = "array";
		size_ = static_cast<size_type>(sizeof(address::table::uint64_type));
		break;
	case id_type::function:
		name_ = "function";
		size_ = static_cast<size_type>(sizeof(address::table::uint64_type));
		break;
	case id_type::node_:
		name_ = "node_t";
		size_ = static_cast<size_type>(sizeof(address::table::uint64_type));
		break;
	case id_type::storage_:
		name_ = "storage_t";
		size_ = static_cast<size_type>(sizeof(address::table::uint64_type));
		break;
	case id_type::nan:
		name_ = "nan_t";
		size_ = static_cast<size_type>(sizeof(address::table::uint64_type));
		break;
	default:
		size_ = static_cast<size_type>(0);
		break;
	}
}

slang::type::primitive::~primitive(){}

slang::type::object::driver_object_type *slang::type::primitive::driver() const{
	switch (id_){
	case id_type::bool_:
		return &common::env::boolean_driver;
	case id_type::bit:
		return nullptr;
	case id_type::byte:
		return &common::env::byte_driver;
	case id_type::char_:
	case id_type::uchar:
	case id_type::wchar:
	case id_type::short_:
	case id_type::ushort:
	case id_type::int_:
	case id_type::uint:
	case id_type::long_:
	case id_type::ulong:
	case id_type::llong:
	case id_type::ullong:
	case id_type::float_:
	case id_type::double_:
	case id_type::ldouble:
	case id_type::nan:
		return &common::env::numeric_driver;
	case id_type::nullptr_:
		return &common::env::pointer_driver;
	case id_type::any:
	case id_type::pointer:
	case id_type::array_:
	case id_type::function:
		return &common::env::indirect_driver;
	case id_type::type_:
		return nullptr;
	case id_type::node_:
		return nullptr;
	case id_type::storage_:
		return nullptr;
	default:
		break;
	}

	return nullptr;
}

const std::string &slang::type::primitive::name() const{
	return name_;
}

slang::type::object::size_type slang::type::primitive::size() const{
	return size_;
}

int slang::type::primitive::score(const object *type, bool is_entry, bool check_const) const{
	auto value = object::score(type, is_entry, check_const);
	if (value != SLANG_MIN_TYPE_SCORE)
		return value;

	if (is_entry){
		if (check_const && type->is_const())
			return SLANG_MIN_TYPE_SCORE;
	}
	else if (type->is_const() || type->is_ref())
		return SLANG_MIN_TYPE_SCORE;

	if (id_ == type->id())
		return (type->is_specific() ? SLANG_MIN_TYPE_SCORE : SLANG_MAX_TYPE_SCORE);

	if (type->is_explicit())
		return SLANG_MIN_TYPE_SCORE;

	if ((is_nan() && type->is_numeric()) || (type->is_nan() && is_numeric()))
		return (SLANG_MAX_TYPE_SCORE - 2);

	if ((is_nullptr() && type->is_pointer()) || (type->is_nullptr() && is_pointer()))
		return (SLANG_MAX_TYPE_SCORE - 2);

	if (is_pointer() && type->is_array())
		return (SLANG_MAX_TYPE_SCORE - 2);

	if (is_numeric() && type->is_numeric()){//Numeric conversion
		if (id_ < type->id())//Narrowing conversion
			return ((SLANG_MAX_TYPE_SCORE - 3) - (static_cast<int>(type->id()) - static_cast<int>(id_)));
		return (SLANG_MAX_TYPE_SCORE - 3);//Widening conversion
	}

	return SLANG_MIN_TYPE_SCORE;
}

slang::type::object::id_type slang::type::primitive::id() const{
	return id_;
}

bool slang::type::primitive::is_primitive() const{
	return true;
}
