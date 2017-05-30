#pragma once

#ifndef SLANG_ENUM_DRIVER_H
#define SLANG_ENUM_DRIVER_H

#include "driver_object.h"
#include "../type/enum_type.h"

namespace slang{
	namespace driver{
		class enum_driver : public object{
		public:
			virtual ~enum_driver();

			virtual uint64_type enum_value(entry_type &entry) override;

			virtual uint64_type value(entry_type &entry);

		protected:
			virtual entry_type *evaluate_(entry_type &entry, binary_info_type &info, entry_type &operand) override;

			virtual void echo_(entry_type &entry, writer_type &out, bool no_throw) override;
		};
	}
}

#endif /* !SLANG_ENUM_DRIVER_H */
