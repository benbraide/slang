#pragma once

#ifndef SLANG_PRIMITIVE_TYPES_H
#define SLANG_PRIMITIVE_TYPES_H

#include "type_object.h"

namespace slang{
	namespace type{
		template <class value_type>
		struct mapper;

		template <>
		struct mapper<std::nullptr_t>{
			static const object::id_type id = object::id_type::nullptr_;
		};

		template <>
		struct mapper<void>{
			static const object::id_type id = object::id_type::void_;
		};

		template <>
		struct mapper<object::bool_type>{
			static const object::id_type id = object::id_type::bool_;
		};

		template <>
		struct mapper<wchar_t>{
			static const object::id_type id = object::id_type::wchar;
		};

		template <>
		struct mapper<char>{
			static const object::id_type id = object::id_type::char_;
		};

		template <>
		struct mapper<unsigned char>{
			static const object::id_type id = object::id_type::uchar;
		};

		template <>
		struct mapper<short>{
			static const object::id_type id = object::id_type::short_;
		};

		template <>
		struct mapper<unsigned short>{
			static const object::id_type id = object::id_type::ushort;
		};

		template <>
		struct mapper<int>{
			static const object::id_type id = object::id_type::int_;
		};

		template <>
		struct mapper<unsigned int>{
			static const object::id_type id = object::id_type::uint;
		};

		template <>
		struct mapper<long>{
			static const object::id_type id = object::id_type::long_;
		};

		template <>
		struct mapper<unsigned long>{
			static const object::id_type id = object::id_type::ulong;
		};

		template <>
		struct mapper<long long>{
			static const object::id_type id = object::id_type::llong;
		};

		template <>
		struct mapper<unsigned long long>{
			static const object::id_type id = object::id_type::ullong;
		};

		template <>
		struct mapper<float>{
			static const object::id_type id = object::id_type::float_;
		};

		template <>
		struct mapper<double>{
			static const object::id_type id = object::id_type::double_;
		};

		template <>
		struct mapper<long double>{
			static const object::id_type id = object::id_type::ldouble;
		};

		template <>
		struct mapper<const char *>{
			static const object::id_type id = object::id_type::string_;
		};

		template <>
		struct mapper<char *>{
			static const object::id_type id = object::id_type::string_;
		};

		template <>
		struct mapper<const wchar_t *>{
			static const object::id_type id = object::id_type::wstring_;
		};

		template <>
		struct mapper<wchar_t *>{
			static const object::id_type id = object::id_type::wstring_;
		};

		struct static_mapper{
			static object::ptr_type map(object::id_type id);
		};
	}
}

#endif /* !SLANG_PRIMITIVE_TYPES_H */
