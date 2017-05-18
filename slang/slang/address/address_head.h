#pragma once

#ifndef SLANG_ADDRESS_HEAD_H
#define SLANG_ADDRESS_HEAD_H

#include "../common/preprocessor.h"

namespace slang{
	namespace address{
		struct head{
			typedef unsigned __int64 uint64_type;
			typedef std::size_t uint_type;

			enum class attribute_type : unsigned int{
				nil				= (0 << 0x0000),
				lval			= (1 << 0x0000),
				ref				= (1 << 0x0001),
				indirect		= (1 << 0x0002),
				uninitialized	= (1 << 0x0003),
				constant		= (1 << 0x0004),
				is_nan			= (1 << 0x0005),
				byte_aligned	= (1 << 0x0006),
				final_			= (1 << 0x0007),
				static_			= (1 << 0x0008),
				private_		= (1 << 0x0009),
				protected_		= (1 << 0x000A),
				public_			= (1 << 0x000B),
				no_dealloc		= (1 << 0x000C),
				function_base	= (1 << 0x000D),
				allocated		= (1 << 0x000E),
				is_string		= (1 << 0x000F),
				is_function		= (1 << 0x0010),
				declared		= (1 << 0x0011),
				optional		= (1 << 0x0012),
				write_protect	= (1 << 0x0013),
				is_float		= (1 << 0x0014),
				tls				= (1 << 0x0015),
			};

			uint_type ref_count;
			uint64_type value;
			uint_type size;
			uint_type actual_size;
			attribute_type attributes;
			char *ptr;
		};

		SLANG_MAKE_OPERATORS(head::attribute_type);
	}
}

#endif /* !SLANG_ADDRESS_HEAD_H */
