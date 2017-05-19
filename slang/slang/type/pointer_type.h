#pragma once

#ifndef SLANG_POINTER_TYPE_H
#define SLANG_POINTER_TYPE_H

#include "type_object.h"

namespace slang{
	namespace type{
		class pointer : public object{
		public:
			explicit pointer(ptr_type underlying_type);

			virtual ~pointer();

			virtual object *underlying_type() override;

			virtual std::string print() const override;

		protected:
			ptr_type underlying_type_;
		};
	}
}

#endif /* !SLANG_POINTER_TYPE_H */
