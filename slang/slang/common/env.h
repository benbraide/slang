#pragma once

#ifndef SLANG_ENV_H
#define SLANG_ENV_H

#include <iostream>

#include "error.h"
#include "output_writer.h"

#include "../address/address_table.h"
#include "../address/generic_address_dependency.h"

#include "../type/type_mapper.h"
#include "../type/primitive_type.h"
#include "../type/modified_type.h"
#include "../type/pointer_type.h"
#include "../type/static_array_type.h"

#include "../storage/named_storage.h"
#include "../storage/temp_storage.h"

#include "../driver/numeric_driver.h"
#include "../driver/pointer_driver.h"
#include "../driver/ref_driver.h"
#include "../driver/boolean_driver.h"
#include "../driver/byte_driver.h"

#include "../utilities/thread_pool.h"

namespace slang{
	namespace common{
		class env{
		public:
			typedef output_writer<std::ostream, std::wostream> output_writer_type;

			typedef type::object::id_type type_id_type;
			typedef type::object::ptr_type type_ptr_type;
			typedef type::object::attribute type_attribute_type;

			typedef std::unordered_map<type_id_type, type_ptr_type> type_list_type;

			enum class runtime_state : unsigned int{
				nil							= (0 << 0x0000),
				error_enabled				= (1 << 0x0000),
				address_table_locked		= (1 << 0x0001),
				writer_append_mode			= (1 << 0x0002),
				output_writer_locked		= (1 << 0x0003),
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

			static output_writer_type default_output_writer;
			static output_writer_type default_error_writer;

			static output_writer_interface *out_writer;
			static output_writer_interface *error_writer;

			static storage::entry *indeterminate;
			static storage::entry *false_;
			static storage::entry *true_;

			static storage::entry *nullptr_;
			static storage::entry *nan;

			static driver::numeric numeric_driver;
			static driver::pointer pointer_driver;
			static driver::indirect indirect_driver;
			static driver::ref ref_driver;
			static driver::boolean boolean_driver;
			static driver::byte byte_driver;

			static void bootstrap();

			static void tear_down();

			static type::object::ptr_type map_type(type::object::id_type id);

			template <typename value_type>
			static type::object::ptr_type map_type(){
				return map_type(type::mapper<value_type>::id);
			}

			template <typename value_type>
			static std::string to_hex(value_type value, std::size_t width = sizeof(value_type) << 1) {
				static const char *digits = "0123456789abcdef";
				std::string rc(width, '0');
				for (size_t i = 0, j = (width - 1) * 4; i < width; ++i, j -= 4)
					rc[i] = digits[(value >> j) & 0x0f];

				return ("0x" + rc);
			}

			template <typename value_type>
			static std::string real_to_string(value_type value){
				auto string_value = std::to_string(value);
				auto index = string_value.size();
				for (; index > 1u; --index){
					if (string_value[index - 2] == '.' || string_value[index - 1] != '0')
						break;
				}

				if (index < string_value.size())
					string_value.erase(index);

				return string_value;
			}

			template <typename value_type>
			static std::wstring real_to_wstring(value_type value){
				auto string_value = std::to_wstring(value);
				auto index = string_value.size();
				for (; index > 1u; --index){
					if (string_value[index - 2] == L'.' || string_value[index - 1] != L'0')
						break;
				}

				if (index < string_value.size())
					string_value.erase(index);

				return string_value;
			}

		private:
			static void bootstrap_();

			static std::once_flag once_flag_;
		};

		SLANG_MAKE_OPERATORS(env::runtime_state);
	}
}

#endif /* !SLANG_ENV_H */
