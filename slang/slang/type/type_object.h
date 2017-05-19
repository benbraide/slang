#pragma once

#ifndef SLANG_TYPE_OBJECT_H
#define SLANG_TYPE_OBJECT_H

#include <string>
#include <memory>

#include "../common/preprocessor.h"

#define SLANG_MIN_TYPE_SCORE 0
#define SLANG_MAX_TYPE_SCORE 20

namespace slang{
	namespace type{
		class object : public std::enable_shared_from_this<object>{
		public:
			typedef std::size_t size_type;
			typedef std::shared_ptr<object> ptr_type;

			enum class attribute : unsigned int{
				nil				= (0 << 0x0000),
				ref				= (1 << 0x0000),
				const_			= (1 << 0x0001),
				pointer			= (1 << 0x0002),
				static_			= (1 << 0x0003),
				tls				= (1 << 0x0004),
				private_		= (1 << 0x0005),
				protected_		= (1 << 0x0006),
				array_			= (1 << 0x0007),
				modified		= (1 << 0x0008),
				variant			= (1 << 0x0009),
				variadic		= (1 << 0x000A),
				final_			= (1 << 0x000B),
				rval			= (1 << 0x000C),
				static_array	= (1 << 0x000D),
			};

			enum class id_type{
				nil,
				auto_,
				any,
				void_,
				bool_,
				bit,
				byte,
				wchar,
				char_,
				uchar,
				short_,
				ushort,
				int_,
				uint,
				long_,
				ulong,
				llong,
				ullong,
				float_,
				double_,
				ldouble,
				array_,
				pointer,
				function,
				nullptr_,
				type_,
				node_,
				storage_,
				enum_,
				union_,
				struct_,
				class_,
				string_,
				wstring_,
			};

			enum class match_type{
				same,
				compatible,
				bully,
			};

			enum class bool_type : char{
				indeterminate	= '\x0',
				false_			= '\x1',
				true_			= '\x2',
			};

			object();

			object(id_type id, attribute attributes, const std::string &name, size_type size);

			object(id_type id, attribute attributes);

			explicit object(id_type id);

			virtual ~object() = default;

			virtual ptr_type reflect();

			virtual object *underlying_type();

			virtual object *remove_all();

			virtual object *remove(attribute attributes);

			virtual object *remove_pointer();

			virtual object *remove_array();

			virtual object *remove_modified();

			virtual object *remove_variadic();

			virtual const std::string &name() const;

			virtual std::string print() const;

			virtual size_type size() const;

			virtual int score(object *type);

			virtual object *match(object *type, match_type criteria);

			virtual attribute attributes() const;

			virtual id_type id() const;

			virtual bool is(attribute attributes);

			virtual bool is(id_type id);

			virtual bool is_same(object *type);

			virtual bool is_compatible(object *type);

			virtual bool is_variant();

			virtual bool is_any();

			virtual bool is_auto();

			virtual bool is_variadic();

			virtual bool is_enum();

			virtual bool is_union();

			virtual bool is_struct();

			virtual bool is_class();

			virtual bool is_primitive();

			virtual bool is_dynamic();

			virtual bool is_numeric();

			virtual bool is_integral();

			virtual bool is_unsigned_integral();

			virtual bool is_floating_point();

			virtual bool is_pointer();

			virtual bool is_string();

			virtual bool is_array();

			virtual bool is_static_array();

			virtual bool is_function();

			virtual bool is_nullptr();

			virtual bool is_ref();

			virtual bool is_rval_ref();

			virtual bool is_const();

			virtual bool is_final();

			virtual bool is_static();

			virtual bool is_thread_local();

			virtual bool is_private();

			virtual bool is_protected();

			virtual bool is_public();

			virtual bool is_modified();

		protected:
			void static_name_();

			void static_size_();

			id_type id_;
			attribute attributes_;
			std::string name_;
			size_type size_;
		};

		SLANG_MAKE_OPERATORS(object::attribute);
	}
}

#endif /* !SLANG_TYPE_OBJECT_H */
