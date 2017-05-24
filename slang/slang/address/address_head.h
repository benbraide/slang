#pragma once

#ifndef SLANG_ADDRESS_HEAD_H
#define SLANG_ADDRESS_HEAD_H

#include <cstdlib>

#include "../common/preprocessor.h"

namespace slang{
	namespace address{
		struct head{
			typedef unsigned __int64 uint64_type;
			typedef std::size_t uint_type;

			enum class attribute_type : unsigned int{
				nil				= (0 << 0x0000),
				indirect		= (1 << 0x0000),
				allocated		= (1 << 0x0001),
				is_string		= (1 << 0x0002),
				is_function		= (1 << 0x0003),
				write_protect	= (1 << 0x0004),
				tls				= (1 << 0x0005),
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
