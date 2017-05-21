#pragma once

#ifndef SLANG_ARRAY_TYPE_H
#define SLANG_ARRAY_TYPE_H

#include "type_object.h"

namespace slang{
	namespace type{
		class array_type : public object{
		public:
			explicit array_type(ptr_type underlying_type);

			virtual ~array_type();

			virtual object *underlying_type() const override;

			virtual object *remove_array() const override;

			virtual const std::string &name() const override;

			virtual std::string print() const override;

			virtual size_type size() const override;

			virtual int score(const object *type) const override;

			virtual id_type id() const override;

			virtual bool is_array() const override;

			virtual bool is_strong_array() const override;

		protected:
			ptr_type underlying_type_;
			std::string name_;
		};
	}
}

#endif /* !SLANG_ARRAY_TYPE_H */
