#pragma once

#ifndef SLANG_TYPE_OBJECT_H
#define SLANG_TYPE_OBJECT_H

#include <string>
#include <memory>

#include "../common/preprocessor.h"

#include "type_id.h"
#include "bool_type.h"

#define SLANG_MIN_TYPE_SCORE 0
#define SLANG_MAX_TYPE_SCORE 20

namespace slang{
	namespace storage{
		class entry;
	}

	namespace driver{
		class object;
	}

	namespace type{
		class object : public std::enable_shared_from_this<object>{
		public:
			typedef driver::object driver_object_type;

			typedef type::bool_type bool_type;
			typedef type::id id_type;

			typedef std::size_t size_type;
			typedef std::shared_ptr<object> ptr_type;

			enum class attribute : unsigned int{
				nil				= (0 << 0x0000),
				ref				= (1 << 0x0000),
				rval			= (1 << 0x0001),
				const_			= (1 << 0x0002),
				final_			= (1 << 0x0003),
				static_			= (1 << 0x0004),
				tls				= (1 << 0x0005),
				private_		= (1 << 0x0006),
				protected_		= (1 << 0x0007),
				explicit_		= (1 << 0x0008),
			};

			enum class match_type{
				same,
				compatible,
				bully,
			};

			virtual ~object() = default;

			virtual ptr_type reflect();

			virtual driver_object_type *driver() const;

			virtual object *underlying_type() const;

			virtual object *remove_modified() const;

			virtual object *remove_pointer() const;

			virtual object *remove_array() const;

			virtual object *remove_variadic() const;

			virtual const std::string &name() const = 0;

			virtual std::string print() const;

			virtual size_type size() const = 0;

			virtual int score(const object *type) const;

			virtual int score(const storage::entry &entry) const;

			virtual const object *match(const object *type, match_type criteria) const;

			virtual attribute attributes() const;

			virtual id_type id() const;

			virtual bool is(attribute attributes, bool any = false) const;

			virtual bool is(id_type id) const;

			virtual bool is_same(const object *type) const;

			virtual bool is_compatible(const object *type) const;

			virtual bool is_compatible(const storage::entry &entry) const;

			virtual bool is_class_compatible(const object *type) const;

			virtual bool is_variant() const;

			virtual bool is_void() const;

			virtual bool is_any() const;

			virtual bool is_auto() const;

			virtual bool is_variadic() const;

			virtual bool is_enum() const;

			virtual bool is_union() const;

			virtual bool is_struct() const;

			virtual bool is_class() const;

			virtual bool is_primitive() const;

			virtual bool is_dynamic() const;

			virtual bool is_numeric() const;

			virtual bool is_integral() const;

			virtual bool is_unsigned_integral() const;

			virtual bool is_floating_point() const;

			virtual bool is_pointer() const;

			virtual bool is_strong_pointer() const;

			virtual bool is_string() const;

			virtual bool is_const_string() const;

			virtual bool is_wstring() const;

			virtual bool is_const_wstring() const;

			virtual bool is_array() const;

			virtual bool is_strong_array() const;

			virtual bool is_static_array() const;

			virtual bool is_function() const;

			virtual bool is_strong_function() const;

			virtual bool is_nullptr() const;

			virtual bool is_nan() const;

			virtual bool is_ref() const;

			virtual bool is_rval_ref() const;

			virtual bool is_const() const;

			virtual bool is_specific() const;

			virtual bool is_final() const;

			virtual bool is_static() const;

			virtual bool is_thread_local() const;

			virtual bool is_private() const;

			virtual bool is_protected() const;

			virtual bool is_public() const;

			virtual bool is_explicit() const;

			virtual bool is_modified() const;
		};

		SLANG_MAKE_OPERATORS(object::attribute);
	}
}

#endif /* !SLANG_TYPE_OBJECT_H */
