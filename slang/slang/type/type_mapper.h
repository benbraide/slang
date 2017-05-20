#pragma once

#ifndef SLANG_PRIMITIVE_TYPES_H
#define SLANG_PRIMITIVE_TYPES_H

#include "type_object.h"
#include "bool_type.h"

namespace slang{
	namespace type{
		template <class value_type>
		struct mapper;

		template <>
		struct mapper<std::nullptr_t>{
			static const type::id id = type::id::nullptr_;
		};

		template <>
		struct mapper<void>{
			static const type::id id = type::id::void_;
		};

		template <>
		struct mapper<bool>{
			static const type::id id = type::id::bool_;
		};

		template <>
		struct mapper<bool_type>{
			static const type::id id = type::id::bool_;
		};

		template <>
		struct mapper<wchar_t>{
			static const type::id id = type::id::wchar;
		};

		template <>
		struct mapper<char>{
			static const type::id id = type::id::char_;
		};

		template <>
		struct mapper<unsigned char>{
			static const type::id id = type::id::uchar;
		};

		template <>
		struct mapper<short>{
			static const type::id id = type::id::short_;
		};

		template <>
		struct mapper<unsigned short>{
			static const type::id id = type::id::ushort;
		};

		template <>
		struct mapper<int>{
			static const type::id id = type::id::int_;
		};

		template <>
		struct mapper<unsigned int>{
			static const type::id id = type::id::uint;
		};

		template <>
		struct mapper<long>{
			static const type::id id = type::id::long_;
		};

		template <>
		struct mapper<unsigned long>{
			static const type::id id = type::id::ulong;
		};

		template <>
		struct mapper<long long>{
			static const type::id id = type::id::llong;
		};

		template <>
		struct mapper<unsigned long long>{
			static const type::id id = type::id::ullong;
		};

		template <>
		struct mapper<float>{
			static const type::id id = type::id::float_;
		};

		template <>
		struct mapper<double>{
			static const type::id id = type::id::double_;
		};

		template <>
		struct mapper<long double>{
			static const type::id id = type::id::ldouble;
		};

		template <>
		struct mapper<const char *>{
			static const type::id id = type::id::string_;
		};

		template <>
		struct mapper<char *>{
			static const type::id id = type::id::string_;
		};

		template <>
		struct mapper<const wchar_t *>{
			static const type::id id = type::id::wstring_;
		};

		template <>
		struct mapper<wchar_t *>{
			static const type::id id = type::id::wstring_;
		};

		struct static_mapper{
			static object::ptr_type map(type::id id);
		};
	}
}

#endif /* !SLANG_PRIMITIVE_TYPES_H */
