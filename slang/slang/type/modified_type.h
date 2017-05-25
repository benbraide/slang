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

			virtual driver_object_type *driver() const override;

			virtual object *underlying_type() const override;

			virtual object *remove_modified() const override;

			virtual object *remove_pointer() const override;

			virtual object *remove_array() const override;

			virtual object *remove_variadic() const override;

			virtual const std::string &name() const override;

			virtual std::string print() const override;

			virtual size_type size() const override;

			virtual int score(const object *type, bool is_entry = false, bool check_const = false) const override;

			virtual int score(const storage::entry &entry) const override;

			virtual attribute attributes() const override;

			virtual id_type id() const override;

			virtual bool is(id_type id) const override;

			virtual bool is_dynamic() const override;

			virtual bool is_strong_pointer() const override;

			virtual bool is_string() const override;

			virtual bool is_const_string() const override;

			virtual bool is_wstring() const override;

			virtual bool is_const_wstring() const override;

			virtual bool is_strong_array() const override;

			virtual bool is_static_array() const override;

			virtual bool is_strong_function() const override;

			virtual bool is_const_target() const override;

			virtual bool is_modified() const override;

		protected:
			ptr_type underlying_type_;
			attribute attributes_;
		};
	}
}

#endif /* !SLANG_MODIFIED_TYPE_H */
