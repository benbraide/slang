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
			typedef address::table::attribute_type attribute_type;

			typedef entry::address_head_type address_head_type;
			typedef entry::type_ptr_type type_ptr_type;

			typedef std::size_t size_type;
			typedef std::list<value_type> value_list_type;

			temp();

			virtual ~temp();

			entry *add(size_type size, type_ptr_type type);

			entry *add(address_head_type &head, type_ptr_type type);

			entry *add(const entry &entry);

			template <typename value_type>
			entry *add(value_type value){
				auto head = address_table->allocate_scalar(value);
				if (head == nullptr)//Failed to allocate memory
					return nullptr;

				return add(*head, type::static_mapper::map(type::mapper<value_type>::id));
			}

			entry *add(const char *value, size_type size = 0u);

			entry *add(const wchar_t *value, size_type size = 0u);

			entry *add(std::nullptr_t);

			entry *nan();

			static address::table *address_table;

		protected:
			void clean_(value_type &value);

			temp *previous_temp_;
			value_list_type value_list_;
			address_head_type nullptr_;
		};
	}
}

#endif /* !SLANG_TEMP_STORAGE_H */
