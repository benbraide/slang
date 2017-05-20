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

			virtual object *underlying_type() const override;

			virtual object *remove_modified() const override;

			virtual object *remove_pointer() const override;

			virtual object *remove_array() const override;

			virtual object *remove_variadic() const override;

			virtual const std::string &name() const override;

			virtual std::string print() const override;

			virtual size_type size() const override;

			virtual int score(const object *type) const override;

			virtual int score(const storage::entry &entry) const override;

			virtual attribute attributes() const override;

			virtual id_type id() const override;

			virtual bool is_modified() const override;

		protected:
			ptr_type underlying_type_;
			attribute attributes_;
		};
	}
}

#endif /* !SLANG_MODIFIED_TYPE_H */
