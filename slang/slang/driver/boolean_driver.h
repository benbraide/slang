#pragma once

#ifndef SLANG_BOOLEAN_DRIVER_H
#define SLANG_BOOLEAN_DRIVER_H

#include "driver_object.h"

namespace slang{
	namespace driver{
		class boolean : public object{
		public:
			virtual ~boolean();

			virtual bool to_bool(entry_type &entry) override;

			virtual bool_type to_boolean(entry_type &entry) override;

			virtual bool_type value(entry_type &entry);

		protected:
			virtual entry_type *evaluate_(entry_type &entry, binary_info_type &info, entry_type &operand) override;

			virtual entry_type *evaluate_(entry_type &entry, unary_info_type &info) override;

			virtual void convert_(entry_type &entry, type_id_type id, char *buffer) override;

			virtual void echo_(entry_type &entry, writer_type &out, bool no_throw) override;
		};
	}
}

#endif /* !SLANG_BOOLEAN_DRIVER_H */
