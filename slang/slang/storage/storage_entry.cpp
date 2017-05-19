#include "storage_entry.h"
#include "storage_object.h"

slang::storage::entry::entry()
	: owner_(nullptr), address_head_(nullptr){}

slang::storage::entry::entry(object *owner, address_head_type *address_head, type_ptr_type type)
	: owner_(owner), address_head_(address_head), type_(type){}

slang::storage::object *slang::storage::entry::owner() const{
	return owner_;
}

slang::storage::entry::address_head_type *slang::storage::entry::address_head() const{
	return address_head_;
}

slang::storage::entry::type_ptr_type slang::storage::entry::type() const{
	return type_;
}
