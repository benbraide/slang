#pragma once

#ifndef SLANG_INDIRECT_DRIVER_H
#define SLANG_INDIRECT_DRIVER_H

#include "driver_object.h"

namespace slang{
	namespace driver{
		class indirect : public object{
		public:
			using object::echo;

			virtual ~indirect();

			virtual entry_type *clone(entry_type &entry) override;

			virtual entry_type *cast(entry_type &entry, type::object &type, cast_type options = cast_type::nil) override;

			virtual entry_type *evaluate(entry_type &entry, binary_info_type &info, entry_type &operand) override;

			virtual entry_type *evaluate(entry_type &entry, unary_info_type &info) override;

			virtual void initialize(entry_type &entry) override;

			virtual bool to_bool(entry_type &entry) override;

			virtual std::string to_string(entry_type &entry) override;

			virtual std::wstring to_wstring(entry_type &entry) override;

			virtual void convert(entry_type &entry, type_id_type id, char *buffer) override;

			virtual void echo(entry_type &entry, writer_type &out, bool no_throw) override;

			virtual type::object *type_of(entry_type &entry) override;

			virtual uint_type size_of(entry_type &entry) override;

			virtual entry_type *linked_object(entry_type &entry) override;

			virtual uint64_type value(entry_type &entry);

		protected:
			virtual entry_type *assign_(entry_type &entry, entry_type &value) override;

			virtual entry_type *do_assignment_(entry_type &entry, entry_type &value);
		};
	}
}

#endif /* !SLANG_INDIRECT_DRIVER_H */
