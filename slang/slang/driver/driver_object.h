#pragma once

#ifndef SLANG_DRIVER_OBJECT_H
#define SLANG_DRIVER_OBJECT_H

#include "../common/operator_info.h"
#include "../common/output_writer_interface.h"

#include "../address/indirect_address_dependency.h"
#include "../storage/storage_entry.h"
#include "../type/type_mapper.h"

namespace slang{
	namespace address{
		class table;
	}

	namespace storage{
		class temp;
	}

	namespace driver{
		class object{
		public:
			typedef lexer::operator_id operator_id_type;

			typedef type::bool_type bool_type;
			typedef type::id type_id_type;
			typedef type::object::attribute type_attribute_type;

			typedef storage::entry entry_type;
			typedef entry_type::attribute_type attribute_type;

			typedef address::head address_head_type;
			typedef address::head::attribute_type address_attribute_type;
			typedef address::head::uint64_type uint64_type;
			typedef address::head::uint_type uint_type;

			typedef common::output_writer_interface writer_type;
			typedef common::binary_operator_info binary_info_type;
			typedef common::unary_operator_info unary_info_type;

			enum class cast_type : unsigned int{
				nil				= (0 << 0x0000),
				reinterpret		= (1 << 0x0000),
				is_explicit		= (1 << 0x0001),
				ref				= (1 << 0x0002),
			};

			virtual ~object();

			virtual entry_type *clone(entry_type &entry);

			virtual entry_type *cast(entry_type &entry, type::object &type, cast_type options = cast_type::nil);

			virtual entry_type *evaluate(entry_type &entry, binary_info_type &info, entry_type &operand);

			virtual entry_type *evaluate(entry_type &entry, unary_info_type &info);

			virtual entry_type *assign(entry_type &entry, entry_type &value);

			virtual void initialize(entry_type &entry);

			virtual bool to_bool(entry_type &entry);

			virtual bool_type to_boolean(entry_type &entry);

			virtual std::string to_string(entry_type &entry);

			virtual std::wstring to_wstring(entry_type &entry);

			virtual void convert(entry_type &entry, type_id_type id, char *buffer);

			template <typename value_type>
			value_type convert(entry_type &entry){
				return convert_<value_type>(entry, std::bool_constant<std::is_same_v<value_type, bool>>());
			}

			virtual void echo(entry_type &entry);

			virtual void echo(entry_type &entry, writer_type &out, bool no_throw = false);

			virtual type::object *type_of(entry_type &entry);

			virtual uint_type size_of(entry_type &entry);

			virtual entry_type *linked_object(entry_type &entry);

			virtual uint64_type address_of(entry_type &entry);

			virtual address_head_type *address_head_of(entry_type &entry);

			virtual uint64_type pointer_target(entry_type &entry);

			virtual uint64_type enum_value(entry_type &entry);

			virtual bool is_indirect(entry_type &entry);

			static object *get_driver(entry_type &entry);

			static address::table &get_address_table();

			static storage::temp *get_temp_storage();

			static bool has_error();

		protected:
			virtual entry_type *evaluate_(entry_type &entry, binary_info_type &info, entry_type &operand);

			virtual entry_type *evaluate_(entry_type &entry, unary_info_type &info);

			virtual entry_type *assign_(entry_type &entry, entry_type &value);

			virtual void convert_(entry_type &entry, type_id_type id, char *buffer);

			template <typename value_type>
			value_type convert_(entry_type &entry, std::false_type){
				auto value = std::remove_const_t<value_type>();
				convert(entry, type::mapper<value_type>::id, (char *)(&value));
				return value;
			}

			template <typename value_type>
			value_type convert_(entry_type &entry, std::true_type){
				auto value = bool_type::indeterminate;
				convert(entry, type_id_type::bool_, reinterpret_cast<char *>(&value));
				return (value == bool_type::true_);
			}

			virtual void echo_(entry_type &entry, writer_type &out, bool no_throw);
		};

		SLANG_MAKE_OPERATORS(object::cast_type);
	}
}

#endif /* !SLANG_DRIVER_OBJECT_H */
