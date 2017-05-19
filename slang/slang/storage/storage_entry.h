#pragma once

#ifndef SLANG_STORAGE_ENTRY_H
#define SLANG_STORAGE_ENTRY_H

#include "../address/address_head.h"
#include "../type/type_object.h"

namespace slang{
	namespace storage{
		class object;

		class entry{
		public:
			typedef address::head address_head_type;
			typedef type::object::ptr_type type_ptr_type;

			entry();

			entry(object *owner, address_head_type *address_head, type_ptr_type type);

			object *owner() const;

			address_head_type *address_head() const;

			type_ptr_type type() const;

		private:
			object *owner_;
			address_head_type *address_head_;
			type_ptr_type type_;
		};
	}
}

#endif /* !SLANG_STORAGE_ENTRY_H */
