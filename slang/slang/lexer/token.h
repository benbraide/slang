#pragma once

#ifndef SLANG_TOKEN_H
#define SLANG_TOKEN_H

#include <string>
#include <variant>
#include <string_view>

#include "token_id.h"

namespace slang{
	namespace lexer{
		class token{
		public:
			typedef token_id id_type;

			typedef std::size_t size_type;
			typedef std::variant<std::string_view, std::string> string_type;

			struct index_type{
				size_type line;
				size_type column;
			};

			token();

			template <typename value_type>
			token(id_type id, index_type index, value_type &&value)
				: value_(std::forward<value_type>(value)){}

			id_type id() const;

			const index_type &index() const;

			std::string_view value() const;

		protected:
			id_type id_;
			index_type index_;
			string_type value_;
		};
	}
}

#endif /* !SLANG_TOKEN_H */
