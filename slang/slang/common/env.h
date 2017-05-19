#pragma once

#ifndef SLANG_ENV_H
#define SLANG_ENV_H

#include "../address/address_table.h"

#include "../type/type_mapper.h"
#include "../type/modified_type.h"
#include "../type/pointer_type.h"

#include "../storage/named_storage.h"
#include "../storage/temp_storage.h"

namespace slang{
	namespace common{
		class env{
		public:
			typedef type::object::id_type type_id_type;
			typedef type::object::ptr_type type_ptr_type;
			typedef type::object::attribute type_attribute_type;

			typedef std::unordered_map<type_id_type, type_ptr_type> type_list_type;

			static address::table address_table;
			static storage::named global_storage;

			static storage::temp local_temp_storage;
			static thread_local storage::temp *temp_storage;
			static type_list_type type_list;

			static storage::entry *indeterminate;
			static storage::entry *false_;
			static storage::entry *true_;

			static storage::entry *nullptr_;
			static storage::entry *nan;

			static void bootstrap();

			static type::object::ptr_type map_type(type::object::id_type id);

			template <typename value_type>
			static type::object::ptr_type map_type(){
				return map_type(type::mapper<value_type>::id);
			}

		private:
			static void bootstrap_();

			static std::once_flag once_flag_;
		};
	}
}

#endif /* !SLANG_ENV_H */
