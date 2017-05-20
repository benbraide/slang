#pragma once

#ifndef SLANG_TOKEN_COMPARER_INTERFACE_H
#define SLANG_TOKEN_COMPARER_INTERFACE_H

#include <string_view>

#include "token_id.h"

namespace slang{
	namespace lexer{
		class token_comparer_interface{
		public:
			virtual ~token_comparer_interface() = default;

			virtual bool is(token_id id, const std::string_view &value = std::string_view()) const = 0;
		};
	}
}

#endif /* !SLANG_TOKEN_COMPARER_INTERFACE_H */
