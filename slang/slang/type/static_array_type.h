#pragma once

#ifndef SLANG_STATIC_ARRAY_TYPE_H
#define SLANG_STATIC_ARRAY_TYPE_H

#include "array_type.h"

namespace slang{
	namespace type{
		class static_array : public array_type{
		public:
			static_array(ptr_type underlying_type, size_type count);

			virtual ~static_array();

			virtual std::string print() const override;

			virtual size_type size() const override;

			virtual bool is_dynamic() const override;

			virtual bool is_static_array() const override;

			virtual size_type count() const;

		protected:
			size_type count_;
		};
	}
}

#endif /* !SLANG_STATIC_ARRAY_TYPE_H */
