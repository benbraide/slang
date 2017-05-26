#pragma once

#ifndef SLANG_TYPE_DRIVER_H
#define SLANG_TYPE_DRIVER_H

#include "driver_object.h"
#include "../type/variant_type.h"

namespace slang{
	namespace driver{
		class type_driver : public object{
		public:
			typedef type::object type_object_type;

			virtual ~type_driver();

			virtual type_object_type *value(entry_type &entry);

		protected:
			virtual entry_type *evaluate_(entry_type &entry, binary_info_type &info, entry_type &operand) override;

			virtual void convert_(entry_type &entry, type_id_type id, char *buffer) override;

			virtual void echo_(entry_type &entry, writer_type &out, bool no_throw) override;

			virtual type_object_type *operand_value_(object &driver, entry_type &operand);
		};
	}
}

#endif /* !SLANG_TYPE_DRIVER_H */
