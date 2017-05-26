#include "variant_type.h"
#include "../common/env.h"

slang::type::variant::variant(ptr_type left, ptr_type right)
	: left_(left), right_(right), name_(left->name() + " | " + right->name()){}

slang::type::variant::~variant() = default;

slang::type::object::driver_object_type *slang::type::variant::driver() const{
	return &common::env::indirect_driver;
}

const std::string &slang::type::variant::name() const{
	return name_;
}

std::string slang::type::variant::print() const{
	return (left_->print() + " | " + right_->print());
}

slang::type::object::size_type slang::type::variant::size() const{
	return static_cast<size_type>(sizeof(address::table::uint64_type));
}

int slang::type::variant::score(const object *type, bool is_entry, bool check_const) const{
	auto value = right_->score(type, is_entry, check_const);
	if (value >= (SLANG_MAX_TYPE_SCORE - 2))
		return value;

	auto other_value = left_->score(type, is_entry, check_const);
	return (other_value >= (SLANG_MAX_TYPE_SCORE - 2)) ? other_value : SLANG_MIN_TYPE_SCORE;
}

int slang::type::variant::score(const storage::entry &entry) const{
	auto value = right_->score(entry);
	if (value >= (SLANG_MAX_TYPE_SCORE - 2))
		return value;

	auto other_value = left_->score(entry);
	return (other_value >= (SLANG_MAX_TYPE_SCORE - 2)) ? other_value : SLANG_MIN_TYPE_SCORE;
}

const slang::type::object *slang::type::variant::match(const object *type, match_type criteria) const{
	auto matched = right_->match(type, match_type::same);
	if (matched != nullptr)
		return matched;

	return left_->match(type, match_type::same);
}

slang::type::object::id_type slang::type::variant::id() const{
	return id_type::variant;
}

bool slang::type::variant::is_compatible(const object *type) const{
	return (score(type) >= (SLANG_MAX_TYPE_SCORE - 2));
}

bool slang::type::variant::is_compatible(const storage::entry &entry) const{
	return (score(entry) >= (SLANG_MAX_TYPE_SCORE - 2));
}

bool slang::type::variant::is_variant() const{
	return true;
}

slang::type::object::ptr_type slang::type::variant::left() const{
	return left_;
}

slang::type::object::ptr_type slang::type::variant::right() const{
	return right_;
}
