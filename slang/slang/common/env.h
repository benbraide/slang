#pragma once

#ifndef SLANG_ENV_H
#define SLANG_ENV_H

#include "error.h"
#include "output_writer_interface.h"

#include "../address/address_table.h"
#include "../address/generic_address_dependency.h"

#include "../type/type_mapper.h"
#include "../type/primitive_type.h"
#include "../type/modified_type.h"
#include "../type/pointer_type.h"
#include "../type/static_array_type.h"

#include "../storage/named_storage.h"
#include "../storage/temp_storage.h"

#include "../utilities/thread_pool.h"

namespace slang{
	namespace common{
		class env{
		public:
			typedef type::object::id_type type_id_type;
			typedef type::object::ptr_type type_ptr_type;
			typedef type::object::attribute type_attribute_type;

			typedef std::unordered_map<type_id_type, type_ptr_type> type_list_type;

			enum class runtime_state : unsigned int{
				nil							= (0 << 0x0000),
				error_enabled				= (1 << 0x0000),
				address_table_locked		= (1 << 0x0001),
			};

			struct runtime_info{
				runtime_state state;
			};

			static thread_local runtime_info runtime;
			static bool exiting;

			static utilities::thread_pool thread_pool;
			static thread_local error error;

			static address::table address_table;
			static storage::named global_storage;

			static storage::temp local_temp_storage;
			static thread_local storage::temp *temp_storage;
			static type_list_type type_list;

			static output_writer_interface *out_writer;
			static output_writer_interface *error_writer;

			static storage::entry *indeterminate;
			static storage::entry *false_;
			static storage::entry *true_;

			static storage::entry *nullptr_;
			static storage::entry *nan;

			static void bootstrap();

			static void tear_down();

			static type::object::ptr_type map_type(type::object::id_type id);

			template <typename value_type>
			static type::object::ptr_type map_type(){
				return map_type(type::mapper<value_type>::id);
			}

		private:
			static void bootstrap_();

			static std::once_flag once_flag_;
		};

		SLANG_MAKE_OPERATORS(env::runtime_state);
	}
}

#endif /* !SLANG_ENV_H */
