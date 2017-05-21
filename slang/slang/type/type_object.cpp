#include "type_object.h"

#include "../address/address_table.h"
#include "../storage/storage_entry.h"

slang::type::object::ptr_type slang::type::object::reflect(){
	return shared_from_this();
}

slang::type::object *slang::type::object::underlying_type() const{
	return nullptr;
}

slang::type::object *slang::type::object::remove_modified() const{
	return const_cast<object *>(this);
}

slang::type::object *slang::type::object::remove_pointer() const{
	return const_cast<object *>(this);
}

slang::type::object *slang::type::object::remove_array() const{
	return const_cast<object *>(this);
}

slang::type::object *slang::type::object::remove_variadic() const{
	return const_cast<object *>(this);
}

std::string slang::type::object::print() const{
	return name();
}

int slang::type::object::score(const object *type) const{
	if (type == this)
		return SLANG_MAX_TYPE_SCORE;

	if (is_auto() || type->is_auto())
		return (SLANG_MAX_TYPE_SCORE - 1);

	if (is_any() || type->is_any() || type->is_class_compatible(this))
		return (SLANG_MAX_TYPE_SCORE - 2);

	return SLANG_MIN_TYPE_SCORE;
}

int slang::type::object::score(const storage::entry &entry) const{
	return score(entry.type().get());
}

const slang::type::object *slang::type::object::match(const object *type, match_type criteria) const{
	auto score = this->score(type);
	if (score <= SLANG_MIN_TYPE_SCORE)//No match
		return nullptr;

	switch (criteria){
	case match_type::same:
		return (SLANG_MAX_TYPE_SCORE <= score) ? this : nullptr;
	case match_type::compatible:
		return (score <= SLANG_MIN_TYPE_SCORE) ? nullptr : this;
	default:
		break;
	}

	return nullptr;
}

slang::type::object::attribute slang::type::object::attributes() const{
	return attribute::nil;
}

slang::type::object::id_type slang::type::object::id() const{
	return id_type::nil;
}

bool slang::type::object::is(attribute attributes, bool any) const{
	if (any)
		return SLANG_IS_ANY(this->attributes(), attributes);
	return SLANG_IS(this->attributes(), attributes);
}

bool slang::type::object::is(id_type id) const{
	return (this->id() == id);
}

bool slang::type::object::is_same(const object *type) const{
	return (score(type) == SLANG_MAX_TYPE_SCORE);
}

bool slang::type::object::is_compatible(const object *type) const{
	return (score(type) > SLANG_MIN_TYPE_SCORE);
}

bool slang::type::object::is_compatible(const storage::entry &entry) const{
	return (score(entry) > SLANG_MIN_TYPE_SCORE);
}

bool slang::type::object::is_class_compatible(const object *type) const{
	return false;
}

bool slang::type::object::is_variant() const{
	return false;
}

bool slang::type::object::is_void() const{
	return is(id_type::void_);
}

bool slang::type::object::is_any() const{
	return is(id_type::array_);
}

bool slang::type::object::is_auto() const{
	return is(id_type::auto_);
}

bool slang::type::object::is_variadic() const{
	return false;
}

bool slang::type::object::is_enum() const{
	return is(id_type::enum_);
}

bool slang::type::object::is_union() const{
	return is(id_type::union_);
}

bool slang::type::object::is_struct() const{
	return is(id_type::struct_);
}

bool slang::type::object::is_class() const{
	return is(id_type::class_);
}

bool slang::type::object::is_primitive() const{
	switch (id()){
	case id_type::union_:
	case id_type::struct_:
	case id_type::class_:
		return false;
	default:
		break;
	}

	return true;
}

bool slang::type::object::is_dynamic() const{
	switch (id()){
	case id_type::any:
	case id_type::pointer:
	case id_type::array_:
	case id_type::function:
		return true;
	default:
		break;
	}

	return false;
}

bool slang::type::object::is_numeric() const{
	switch (id()){
	case id_type::char_:
	case id_type::uchar:
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
		return true;
	default:
		break;
	}

	return false;
}

bool slang::type::object::is_integral() const{
	switch (id()){
	case id_type::char_:
	case id_type::uchar:
	case id_type::short_:
	case id_type::ushort:
	case id_type::int_:
	case id_type::uint:
	case id_type::long_:
	case id_type::ulong:
	case id_type::llong:
	case id_type::ullong:
		return true;
	default:
		break;
	}

	return false;
}

bool slang::type::object::is_unsigned_integral() const{
	switch (id()){
	case id_type::uchar:
	case id_type::ushort:
	case id_type::uint:
	case id_type::ulong:
	case id_type::ullong:
		return true;
	default:
		break;
	}

	return false;
}

bool slang::type::object::is_floating_point() const{
	switch (id()){
	case id_type::float_:
	case id_type::double_:
	case id_type::ldouble:
		return true;
	default:
		break;
	}

	return false;
}

bool slang::type::object::is_pointer() const{
	return is(id_type::pointer);
}

bool slang::type::object::is_strong_pointer() const{
	return (is_pointer() && !is_dynamic());
}

bool slang::type::object::is_string() const{
	return (is_strong_pointer() && remove_pointer()->is(id_type::char_));
}

bool slang::type::object::is_const_string() const{
	return (is_const() && is_string());
}

bool slang::type::object::is_wstring() const{
	return (is_strong_pointer() && remove_pointer()->is(id_type::wchar));
}

bool slang::type::object::is_const_wstring() const{
	return (is_const() && is_wstring());
}

bool slang::type::object::is_array() const{
	return is(id_type::array_);
}

bool slang::type::object::is_strong_array() const{
	return false;
}

bool slang::type::object::is_static_array() const{
	return (is_array() && !is_dynamic());
}

bool slang::type::object::is_function() const{
	return is(id_type::function);
}

bool slang::type::object::is_strong_function() const{
	return (is_function() && !is_dynamic());
}

bool slang::type::object::is_nullptr() const{
	return is(id_type::nullptr_);
}

bool slang::type::object::is_nan() const{
	return is(id_type::nan);
}

bool slang::type::object::is_ref() const{
	return SLANG_IS(attributes(), attribute::ref);
}

bool slang::type::object::is_rval_ref() const{
	return SLANG_IS(attributes(), attribute::rval);
}

bool slang::type::object::is_const() const{
	return SLANG_IS(attributes(), attribute::const_);
}

bool slang::type::object::is_specific() const{
	return SLANG_IS(attributes(), attribute::ref | attribute::rval | attribute::const_);
}

bool slang::type::object::is_final() const{
	return SLANG_IS(attributes(), attribute::final_);
}

bool slang::type::object::is_static() const{
	return SLANG_IS(attributes(), attribute::static_);
}

bool slang::type::object::is_thread_local() const{
	return SLANG_IS(attributes(), attribute::tls);
}

bool slang::type::object::is_private() const{
	return SLANG_IS(attributes(), attribute::private_);
}

bool slang::type::object::is_protected() const{
	return SLANG_IS(attributes(), attribute::protected_);
}

bool slang::type::object::is_public() const{
	return !SLANG_IS_ANY(attributes(), attribute::private_ | attribute::protected_);
}

bool slang::type::object::is_explicit() const{
	return SLANG_IS(attributes(), attribute::explicit_);
}

bool slang::type::object::is_modified() const{
	return (attributes() != attribute::nil);
}
