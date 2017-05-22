#pragma once

#ifndef SLANG_DRIVER_OBJECT_H
#define SLANG_DRIVER_OBJECT_H

#include "../common/operator_info.h"
#include "../common/output_writer_interface.h"

#include "../address/indirect_address_dependency.h"
#include "../storage/storage_entry.h"

namespace slang{
	namespace driver{
		class object{
		public:
			typedef lexer::operator_id operator_id_type;

			typedef type::bool_type bool_type;
			typedef storage::entry entry_type;
			typedef entry_type::attribute_type attribute_type;

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

			virtual int to_int(entry_type &entry);

			virtual std::string to_string(entry_type &entry);

			virtual void echo(entry_type &entry, writer_type &out, bool no_throw = false);

			virtual type::object *typeof(entry_type &entry);

			virtual entry_type *linked_object(entry_type &entry);

			virtual bool is_indirect(entry_type &entry);

		protected:
			virtual entry_type *assign_(entry_type &entry, entry_type &value);
		};

		SLANG_MAKE_OPERATORS(object::cast_type);
	}
}

#endif /* !SLANG_DRIVER_OBJECT_H */
