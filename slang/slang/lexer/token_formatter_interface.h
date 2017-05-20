#pragma once

#ifndef SLANG_TOKEN_FORMATTER_INTERFACE_H
#define SLANG_TOKEN_FORMATTER_INTERFACE_H

#include <functional>

#include "token.h"
#include "lang_rules.h"

namespace slang{
	namespace lexer{
		class token_formatter_interface{
		public:
			typedef lang_rules::match_info match_info;

			virtual ~token_formatter_interface() = default;

			virtual bool format(match_info &info) = 0;
		};
	}
}

#endif /* !SLANG_TOKEN_FORMATTER_INTERFACE_H */
