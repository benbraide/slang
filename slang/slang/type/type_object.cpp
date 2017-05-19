#include "type_object.h"
#include "../address/address_table.h"

slang::type::object::object(){}

slang::type::object::object(id_type id, attribute attributes, const std::string &name, size_type size)
	: id_(id), attributes_(attributes), name_(name), size_(size){}

slang::type::object::object(id_type id, attribute attributes)
	: id_(id), attributes_(attributes){
	static_name_();
	static_size_();
}

slang::type::object::object(id_type id)
	: id_(id), attributes_(attribute::nil){
	static_name_();
	static_size_();
}

slang::type::object::ptr_type slang::type::object::reflect(){
	return shared_from_this();
}

slang::type::object *slang::type::object::underlying_type(){
	return nullptr;
}

slang::type::object *slang::type::object::remove_all(){
	return remove(attribute::pointer | attribute::array_ | attribute::modified | attribute::variadic);
}

slang::type::object *slang::type::object::remove(attribute attributes){
	if (!SLANG_IS(attributes_, attributes) && !SLANG_IS(attributes_, attribute::modified))
		return this;

	auto underlying_type = this->underlying_type();
	if (underlying_type == nullptr)
		return this;

	auto value = underlying_type->remove(attributes);
	return (value == underlying_type) ? this : value;
}

slang::type::object *slang::type::object::remove_pointer(){
	return remove(attribute::pointer);
}

slang::type::object *slang::type::object::remove_array(){
	return remove(attribute::array_);
}

slang::type::object *slang::type::object::remove_modified(){
	return remove(attribute::modified);
}

slang::type::object *slang::type::object::remove_variadic(){
	return remove(attribute::variadic);
}

const std::string &slang::type::object::name() const{
	return name_;
}

std::string slang::type::object::print() const{
	return name_;//#TODO: Add indentation
}

slang::type::object::size_type slang::type::object::size() const{
	return size_;
}

int slang::type::object::score(object *type){
	auto this_type = remove(attribute::modified);
	if (this_type == (type = type->remove(attribute::modified)))
		return SLANG_MAX_TYPE_SCORE;

	if (this_type->id_ < id_type::class_ && this_type->id_ == type->id_)
		return SLANG_MAX_TYPE_SCORE;

	if (this_type->id_ == id_type::auto_ || type->id_ == id_type::auto_)
		return (SLANG_MAX_TYPE_SCORE - 1);

	if (this_type->id_ == id_type::any || type->id_ == id_type::any)
		return (SLANG_MAX_TYPE_SCORE - 2);

	if (this_type->id_ == id_type::pointer && type->id_ == id_type::nullptr_)
		return (SLANG_MAX_TYPE_SCORE - 2);

	if (this_type->id_ == id_type::nullptr_ && type->id_ == id_type::pointer)
		return (SLANG_MAX_TYPE_SCORE - 2);

	if (this_type->id_ == id_type::pointer && type->id_ == id_type::array_)
		return (SLANG_MAX_TYPE_SCORE - 2);

	if (type->id_ == id_type::class_ && type->is_compatible(this_type))
		return (SLANG_MAX_TYPE_SCORE - 2);

	if (this_type->is_numeric() && type->is_numeric()){//Numeric conversion
		if (this_type->id_ < type->id_)//Narrowing conversion
			return ((SLANG_MAX_TYPE_SCORE - 3) - (static_cast<int>(type->id_) - static_cast<int>(this_type->id_)));
		return (SLANG_MAX_TYPE_SCORE - 3);//Widening conversion
	}

	return SLANG_MIN_TYPE_SCORE;
}

slang::type::object *slang::type::object::match(object *type, match_type criteria){
	auto score = this->score(type);
	if (score <= SLANG_MIN_TYPE_SCORE)//No match
		return nullptr;

	switch (criteria){
	case match_type::same:
		return (SLANG_MAX_TYPE_SCORE <= score) ? this : nullptr;
	case match_type::compatible:
		return (score <= SLANG_MIN_TYPE_SCORE) ? nullptr : this;
	case match_type::bully:
		if (score <= SLANG_MIN_TYPE_SCORE)
			return nullptr;

		return (size_ <= type->size_) ? type : this;
	default:
		break;
	}

	return nullptr;
}

slang::type::object::attribute slang::type::object::attributes() const{
	return attribute::nil;
}

slang::type::object::id_type slang::type::object::id() const{
	return id_;
}

bool slang::type::object::is(attribute attributes){
	if (attributes_ == attributes)
		return true;

	if (!SLANG_IS(attributes_, attributes) && !SLANG_IS(attributes_, attribute::modified))
		return false;

	auto underlying_type = this->underlying_type();
	return (underlying_type == nullptr) ? false : underlying_type->is(attributes);
}

bool slang::type::object::is(id_type id){
	return (id_ == id);
}

bool slang::type::object::is_same(object *type){
	return (score(type) == SLANG_MAX_TYPE_SCORE);
}

bool slang::type::object::is_compatible(object *type){
	return (score(type) > SLANG_MIN_TYPE_SCORE);
}

bool slang::type::object::is_variant(){
	return is(attribute::variant);
}

bool slang::type::object::is_void(){
	return is(id_type::void_);
}

bool slang::type::object::is_any(){
	return is(id_type::array_);
}

bool slang::type::object::is_auto(){
	return is(id_type::auto_);
}

bool slang::type::object::is_variadic(){
	return is(attribute::variadic);
}

bool slang::type::object::is_enum(){
	return is(id_type::enum_);
}

bool slang::type::object::is_union(){
	return is(id_type::union_);
}

bool slang::type::object::is_struct(){
	return is(id_type::struct_);
}

bool slang::type::object::is_class(){
	return is(id_type::class_);
}

bool slang::type::object::is_primitive(){
	switch (id_){
	case id_type::union_:
	case id_type::struct_:
	case id_type::class_:
		return false;
	default:
		break;
	}

	return true;
}

bool slang::type::object::is_dynamic(){
	switch (id_){
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

bool slang::type::object::is_numeric(){
	switch (id_){
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

bool slang::type::object::is_integral(){
	switch (id_){
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

bool slang::type::object::is_unsigned_integral(){
	switch (id_){
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

bool slang::type::object::is_floating_point(){
	switch (id_){
	case id_type::float_:
	case id_type::double_:
	case id_type::ldouble:
		return true;
	default:
		break;
	}

	return false;
}

bool slang::type::object::is_pointer(){
	return is(id_type::pointer);
}

bool slang::type::object::is_strong_pointer(){
	return (is_pointer() && !is_dynamic());
}

bool slang::type::object::is_string(){
	return (is_strong_pointer() && remove_pointer()->is(id_type::char_));
}

bool slang::type::object::is_const_string(){
	return (is_const() && is_string());
}

bool slang::type::object::is_wstring(){
	return (is_strong_pointer() && remove_pointer()->is(id_type::wchar));
}

bool slang::type::object::is_const_wstring(){
	return (is_const() && is_wstring());
}

bool slang::type::object::is_array(){
	return is(id_type::array_);
}

bool slang::type::object::is_static_array(){
	return (is_array() && !is_dynamic());
}

bool slang::type::object::is_function(){
	return is(id_type::function);
}

bool slang::type::object::is_strong_function(){
	return (is_function() && !is_dynamic());
}

bool slang::type::object::is_nullptr(){
	return is(id_type::nullptr_);
}

bool slang::type::object::is_nan(){
	return is(attribute::nan);
}

bool slang::type::object::is_ref(){
	return SLANG_IS(attributes_, attribute::ref);
}

bool slang::type::object::is_rval_ref(){
	return SLANG_IS(attributes_, attribute::rval);
}

bool slang::type::object::is_const(){
	return SLANG_IS(attributes_, attribute::const_);
}

bool slang::type::object::is_final(){
	return SLANG_IS(attributes_, attribute::final_);
}

bool slang::type::object::is_static(){
	return SLANG_IS(attributes_, attribute::static_);
}

bool slang::type::object::is_thread_local(){
	return SLANG_IS(attributes_, attribute::tls);
}

bool slang::type::object::is_private(){
	return SLANG_IS(attributes_, attribute::private_);
}

bool slang::type::object::is_protected(){
	return SLANG_IS(attributes_, attribute::protected_);
}

bool slang::type::object::is_public(){
	return !SLANG_IS(attributes_, attribute::private_ | attribute::protected_);
}

bool slang::type::object::is_modified(){
	return SLANG_IS(attributes_, attribute::modified);
}

void slang::type::object::static_name_(){
	if (is_nan()){
		name_ = "nan_t";
		return;
	}

	switch (id_){
	case id_type::void_:
		name_ = "void";
		break;
	case id_type::any:
		name_ = "any";
		break;
	case id_type::auto_:
		name_ = "auto";
		break;
	case id_type::bool_:
		name_ = "bool";
		break;
	case id_type::bit:
		name_ = "bit";
		break;
	case id_type::byte:
		name_ = "byte";
		break;
	case id_type::char_:
		name_ = "char";
		break;
	case id_type::uchar:
		name_ = "unsigned char";
		break;
	case id_type::wchar:
		name_ = "wchar";
		break;
	case id_type::short_:
		name_ = "short";
		break;
	case id_type::ushort:
		name_ = "unsigned short";
		break;
	case id_type::int_:
		name_ = "int";
		break;
	case id_type::uint:
		name_ = "unsigned int";
		break;
	case id_type::long_:
		name_ = "long";
		break;
	case id_type::ulong:
		name_ = "unsigned long";
		break;
	case id_type::llong:
		name_ = "long long";
		break;
	case id_type::ullong:
		name_ = "unsigned long long";
		break;
	case id_type::float_:
		name_ = "float";
		break;
	case id_type::double_:
		name_ = "double";
		break;
	case id_type::ldouble:
		name_ = "long double";
		break;
	case id_type::nullptr_:
		name_ = "nullptr_t";
		break;
	case id_type::type_:
		name_ = "type_t";
		break;
	case id_type::pointer:
		name_ = "pointer_t";
		break;
	case id_type::array_:
		name_ = "array";
		break;
	case id_type::function:
		name_ = "function";
		break;
	case id_type::node_:
		name_ = "node_t";
		break;
	case id_type::storage_:
		name_ = "storage_t";
		break;
	default:
		break;
	}
}

void slang::type::object::static_size_(){
	if (is_nan()){
		size_ = static_cast<size_type>(1);
		return;
	}

	if (is_ref()){
		size_ = static_cast<size_type>(sizeof(address::table::uint64_type));
		return;
	}

	switch (id_){
	case id_type::bool_:
		size_ = static_cast<size_type>(sizeof(char));
		break;
	case id_type::byte:
		size_ = static_cast<size_type>(sizeof(unsigned char));
		break;
	case id_type::char_:
		size_ = static_cast<size_type>(sizeof(char));
		break;
	case id_type::uchar:
		size_ = static_cast<size_type>(sizeof(unsigned char));
		break;
	case id_type::wchar:
		size_ = static_cast<size_type>(sizeof(wchar_t));
		break;
	case id_type::short_:
		size_ = static_cast<size_type>(sizeof(short));
		break;
	case id_type::ushort:
		size_ = static_cast<size_type>(sizeof(unsigned short));
		break;
	case id_type::int_:
		size_ = static_cast<size_type>(sizeof(int));
		break;
	case id_type::uint:
		size_ = static_cast<size_type>(sizeof(unsigned int));
		break;
	case id_type::long_:
		size_ = static_cast<size_type>(sizeof(long));
		break;
	case id_type::ulong:
		size_ = static_cast<size_type>(sizeof(unsigned long));
		break;
	case id_type::llong:
		size_ = static_cast<size_type>(sizeof(long long));
		break;
	case id_type::ullong:
		size_ = static_cast<size_type>(sizeof(unsigned long long));
		break;
	case id_type::float_:
		size_ = static_cast<size_type>(sizeof(float));
		break;
	case id_type::double_:
		size_ = static_cast<size_type>(sizeof(double));
		break;
	case id_type::ldouble:
		size_ = static_cast<size_type>(sizeof(long double));
		break;
	default:
		size_ = static_cast<size_type>(sizeof(address::table::uint64_type));
		break;
	}
}
