#pragma once

#ifndef SLANG_OPERATOR_INFO_H
#define SLANG_OPERATOR_INFO_H

#include <string>

#include "../lexer/operator_id.h"

namespace slang{
	namespace common{
		struct binary_operator_info{
			lexer::operator_id id;
			std::string value;
		};

		struct unary_operator_info{
			bool is_left;
			lexer::operator_id id;
			std::string value;
		};
	}
}

#endif /* !SLANG_OPERATOR_INFO_H */
