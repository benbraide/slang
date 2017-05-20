#pragma once

#ifndef SLANG_LANG_RULES_H
#define SLANG_LANG_RULES_H

#include "rule.h"
#include "token.h"

namespace slang{
	namespace lexer{
		class lang_rules : public rule<token_id>{
		public:
			lang_rules();
		};
	}
}

#endif /* !SLANG_LANG_RULES_H */
