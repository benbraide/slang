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
			typedef type::object::ptr_type type_ptr_type;

			typedef address::head address_head_type;
			typedef address_head_type::uint64_type uint64_type;
			typedef address_head_type::uint_type uint_type;

			enum class attribute_type : unsigned int{
				nil				= (0 << 0x0000),
				lval			= (1 << 0x0000),
				ref_			= (1 << 0x0001),
				rval_ref		= (1 << 0x0002),
				uninitialized	= (1 << 0x0003),
				const_			= (1 << 0x0004),
				nan_			= (1 << 0x0005),
				void_			= (1 << 0x0006),
				static_			= (1 << 0x0007),
				private_		= (1 << 0x0008),
				protected_		= (1 << 0x0009),
				public_			= (1 << 0x000A),
				tls				= (1 << 0x000B),
				block_aligned	= (1 << 0x000C),
				const_pointer	= (1 << 0x000D),
			};

			entry();

			entry(object *owner, uint64_type address_value, type_ptr_type type, attribute_type attributes = attribute_type::nil);

			object *owner() const;

			void add_attributes(attribute_type value, bool replace = false);

			void remove_attributes(attribute_type value);

			uint64_type address_value() const;

			address_head_type *address_head() const;

			address_head_type *cached_address_head() const;

			void set_type(type_ptr_type value);

			type_ptr_type type() const;

			attribute_type attributes() const;

			bool is(attribute_type attribute) const;

			bool is_any_of(attribute_type attributes) const;

			bool is_lval() const;

			bool is_ref() const;

			bool is_rval_ref() const;

			bool is_const() const;

			bool is_uninitialized() const;

			bool is_void() const;

			bool is_const_pointer() const;

		private:
			object *owner_;
			uint64_type address_value_;
			type_ptr_type type_;
			attribute_type attributes_;
			mutable address_head_type address_head_;
		};

		SLANG_MAKE_OPERATORS(entry::attribute_type);
	}
}

#endif /* !SLANG_STORAGE_ENTRY_H */
