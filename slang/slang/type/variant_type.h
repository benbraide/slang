#pragma once

#ifndef SLANG_VARIANT_TYPE_H
#define SLANG_VARIANT_TYPE_H

#include "type_object.h"

namespace slang{
	namespace type{
		class variant : public object{
		public:
			variant(ptr_type left, ptr_type right);

			virtual ~variant();

			virtual driver_object_type *driver() const override;

			virtual const std::string &name() const override;

			virtual std::string print() const override;

			virtual size_type size() const override;

			virtual int score(const object *type, bool is_entry = false, bool check_const = false) const override;

			virtual int score(const storage::entry &entry) const override;

			virtual const object *match(const object *type, match_type criteria) const;

			virtual id_type id() const override;

			virtual bool is_compatible(const object *type) const;

			virtual bool is_compatible(const storage::entry &entry) const;

			virtual bool is_variant() const override;

			virtual ptr_type left() const;

			virtual ptr_type right() const;

		protected:
			ptr_type left_;
			ptr_type right_;
			std::string name_;
		};
	}
}

#endif /* !SLANG_VARIANT_TYPE_H */
