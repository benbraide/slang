#pragma once

#ifndef SLANG_INDIRECT_ADDRESS_DEPENDENCY_H
#define SLANG_INDIRECT_ADDRESS_DEPENDENCY_H

#include <memory>

#include "address_dependency.h"
#include "../storage/storage_entry.h"

namespace slang{
	namespace address{
		class indirect_dependency : public dependency{
		public:
			typedef storage::entry entry_type;
			typedef std::shared_ptr<dependency> dependency_type;

			indirect_dependency(const entry_type &value, dependency_type previous);

			virtual ~indirect_dependency();

			virtual void set_value(const entry_type &value);

			virtual entry_type &value();

			virtual dependency_type previous() const;

		protected:
			entry_type value_;
			dependency_type previous_;
		};
	}
}

#endif /* !SLANG_INDIRECT_ADDRESS_DEPENDENCY_H */
