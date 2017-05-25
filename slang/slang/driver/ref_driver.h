#pragma once

#ifndef SLANG_REF_DRIVER_H
#define SLANG_REF_DRIVER_H

#include "indirect_driver.h"

namespace slang{
	namespace driver{
		class ref : public indirect{
		public:
			virtual ~ref();

			virtual entry_type *assign(entry_type &entry, entry_type &value) override;

		protected:
			virtual entry_type *assign_(entry_type &entry, entry_type &value) override;
		};
	}
}

#endif /* !SLANG_REF_DRIVER_H */
