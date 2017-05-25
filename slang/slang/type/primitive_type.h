#pragma once

#ifndef SLANG_PRIMITIVE_TYPE_H
#define SLANG_PRIMITIVE_TYPE_H

#include "type_object.h"

namespace slang{
	namespace type{
		class primitive : public object{
		public:
			explicit primitive(id_type id);

			virtual ~primitive();

			virtual driver_object_type *driver() const override;

			virtual const std::string &name() const override;

			virtual size_type size() const override;

			virtual int score(const object *type, bool is_entry = false, bool check_const = false) const override;

			virtual id_type id() const override;

			virtual bool is_primitive() const override;

		protected:
			std::string name_;
			size_type size_;
			id_type id_;
		};
	}
}

#endif /* !SLANG_PRIMITIVE_TYPE_H */
