#pragma once

#ifndef SLANG_BYTE_DRIVER_H
#define SLANG_BYTE_DRIVER_H

#include "driver_object.h"

namespace slang{
	namespace driver{
		class byte : public object{
		public:
			typedef unsigned char uchar_type;

			virtual ~byte();

			virtual entry_type *cast(entry_type &entry, type::object &type, cast_type options = cast_type::nil) override;

			virtual uchar_type value(entry_type &entry);

		protected:
			virtual entry_type *evaluate_(entry_type &entry, binary_info_type &info, entry_type &operand) override;

			virtual entry_type *evaluate_(entry_type &entry, unary_info_type &info) override;

			virtual void convert_(entry_type &entry, type_id_type id, char *buffer) override;

			virtual void echo_(entry_type &entry, writer_type &out, bool no_throw) override;

			virtual entry_type *evaluate_increment_(entry_type &entry, bool increment, bool lval);

			virtual entry_type *write_(entry_type &entry, uchar_type value);

			virtual uchar_type operand_value_(object &driver, entry_type &operand);

			virtual bool is_non_const_lval_(entry_type &entry);
		};
	}
}

#endif /* !SLANG_BYTE_DRIVER_H */
