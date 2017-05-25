#include "ref_driver.h"
#include "../common/env.h"

slang::driver::ref::~ref() = default;

slang::driver::object::entry_type *slang::driver::ref::assign(entry_type &entry, entry_type &value){
	if (entry.is_uninitialized())//First assignment
		return assign_(entry, value);

	auto target = linked_object(entry);
	if (target == nullptr)
		return common::env::error.set_and_return<nullptr_t>("Uninitialized value in expression", true);

	return get_driver(*target)->assign(*target, value);
}

slang::driver::object::entry_type *slang::driver::ref::assign_(entry_type &entry, entry_type &value){
	if (!value.is_lval())
		return common::env::error.set_and_return<nullptr_t>("Cannot assign an rvalue to a reference.");

	if (value.is_const() && !type_of(entry)->is_const())
		return common::env::error.set_and_return<nullptr_t>("Cannot assign a constant object to a non-constant reference.");

	return do_assignment_(entry, value);
}
