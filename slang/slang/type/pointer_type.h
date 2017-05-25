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

			virtual driver_object_type *driver() const override;

			virtual object *underlying_type() const override;

			virtual object *remove_pointer() const override;

			virtual const std::string &name() const override;

			virtual std::string print() const override;

			virtual size_type size() const override;

			virtual int score(const object *type) const override;

			virtual int score(const storage::entry &entry) const override;

			virtual id_type id() const override;

			virtual bool is_dynamic() const override;

			virtual bool is_pointer() const override;

		protected:
			ptr_type underlying_type_;
			std::string name_;
		};
	}
}

#endif /* !SLANG_POINTER_TYPE_H */
