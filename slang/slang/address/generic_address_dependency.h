#pragma once

#ifndef SLANG_GENERIC_ADDRESS_DEPENDENCY_H
#define SLANG_GENERIC_ADDRESS_DEPENDENCY_H

#include "address_dependency.h"

namespace slang{
	namespace address{
		template <class value_type>
		class generic_dependency : public dependency{
		public:
			typedef value_type value_type;

			explicit generic_dependency(const value_type &value)
				: value_(value){}

			virtual ~generic_dependency() = default;

			virtual const value_type &value() const{
				return value_;
			}

		protected:
			value_type value_;
		};
	}
}

#endif /* !SLANG_GENERIC_ADDRESS_DEPENDENCY_H */
