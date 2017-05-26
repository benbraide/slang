#pragma once

#ifndef SLANG_TEMP_STORAGE_H
#define SLANG_TEMP_STORAGE_H

#include "storage_object.h"

#include "../address/address_table.h"
#include "../type/type_mapper.h"

namespace slang{
	namespace storage{
		class temp{
		public:
			typedef object::value_type value_type;
			typedef address::table::uint64_type uint64_type;

			typedef type::object type_object_type;
			typedef type::object::ptr_type type_ptr_type;
			typedef type::object::attribute type_attribute_type;
			typedef type::id type_id_type;

			typedef entry::attribute_type attribute_type;
			typedef entry::address_head_type address_head_type;
			typedef entry::type_ptr_type type_ptr_type;

			typedef std::size_t size_type;
			typedef std::list<value_type> value_list_type;

			temp();

			virtual ~temp();

			entry *add(size_type size, type_ptr_type type, attribute_type attributes = attribute_type::nil);

			entry *wrap(uint64_type value, type_ptr_type type, attribute_type attributes = attribute_type::nil);

			entry *wrap(const entry &entry);

			template <typename value_type>
			entry *add(value_type value){
				return add_typed(value, type::static_mapper::map(type::mapper<value_type>::id));
			}

			template <typename value_type>
			entry *add_typed(value_type value, type_ptr_type type){
				auto head = address_table->allocate_scalar(value);
				if (head == nullptr)//Failed to allocate memory
					return nullptr;

				return wrap(head->value, type, attribute_type::block_aligned);
			}

			entry *add(const char *value, size_type size = 0u);

			entry *add(const wchar_t *value, size_type size = 0u);

			entry *add(type_object_type &value);

			entry *add(std::nullptr_t);

			entry *add_pointer(entry &value);

			entry *add_pointer(uint64_type value, type_ptr_type type);

			entry *nan();

			static address::table *address_table;

		protected:
			entry *add_string_(address_head_type &head, type_id_type id);

			void clean_(value_type &value);

			temp *previous_temp_;
			value_list_type value_list_;
		};
	}
}

#endif /* !SLANG_TEMP_STORAGE_H */
