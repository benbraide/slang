#pragma once

#ifndef SLANG_MODIFIED_TYPE_H
#define SLANG_MODIFIED_TYPE_H

#include "type_object.h"

namespace slang{
	namespace type{
		class modified : public object{
		public:
			modified(ptr_type underlying_type, attribute attributes);

			virtual ~modified();

			virtual object *underlying_type() override;

			virtual std::string print() const override;

		protected:
			ptr_type underlying_type_;
		};
	}
}

#endif /* !SLANG_MODIFIED_TYPE_H */
