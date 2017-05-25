#pragma once

#ifndef SLANG_POINTER_DRIVER_H
#define SLANG_POINTER_DRIVER_H

#include "driver_object.h"

namespace slang{
	namespace driver{
		class pointer : public object{
		public:
			virtual ~pointer();

			virtual entry_type *cast(entry_type &entry, type::object &type, cast_type options = cast_type::nil) override;

			virtual uint_type size_of(entry_type &entry) override;

			virtual uint64_type pointer_target(entry_type &entry) override;

			virtual uint64_type value(entry_type &entry);

			virtual address_head_type target(entry_type &entry);

			virtual entry_type *dereference(entry_type &entry, long long offset = 0);

		protected:
			virtual entry_type *evaluate_(entry_type &entry, binary_info_type &info, entry_type &operand) override;

			virtual entry_type *evaluate_(entry_type &entry, unary_info_type &info) override;

			virtual entry_type *assign_(entry_type &entry, entry_type &value) override;

			virtual void convert_(entry_type &entry, type_id_type id, char *buffer) override;

			virtual void echo_(entry_type &entry, writer_type &out, bool no_throw) override;

			virtual entry_type *cast_(entry_type &entry, type_id_type id);

			virtual entry_type *cast_(const char *value, bool is_wide, type_id_type id);

			virtual entry_type *static_cast_(entry_type &entry, type::object &type, cast_type options);

			virtual entry_type *reinterpret_cast_(entry_type &entry, type::object &type, cast_type options);

			virtual entry_type *evaluate_increment_(entry_type &entry, bool increment, bool lval, uint64_type mult = 1u);

			virtual entry_type *evaluate_string_(entry_type &entry, binary_info_type &info, entry_type &operand);

			virtual entry_type *evaluate_wstring_(entry_type &entry, binary_info_type &info, entry_type &operand);

			virtual char *get_string_ptr_(entry_type &entry);
		};
	}
}

#endif /* !SLANG_POINTER_DRIVER_H */
