#pragma once

#ifndef SLANG_SOURCE_OBJECT_H
#define SLANG_SOURCE_OBJECT_H

#include <list>
#include <memory>

#include "../common/preprocessor.h"

#include "lang_rules.h"
#include "token_comparer_interface.h"
#include "token_formatter_interface.h"

namespace slang{
	namespace lexer{
		class source_object{
		public:
			typedef const char *ptr_type;

			typedef token::size_type size_type;
			typedef token::index_type index_type;

			struct token_info{
				lang_rules::match_info match_info;
				index_type index;
			};

			typedef std::shared_ptr<token_info> token_ptr_type;
			typedef std::list<token_ptr_type> list_type;

			struct marker_info{
				ptr_type begin;
				ptr_type current;
				ptr_type end;
			};

			enum class option : unsigned int{
				nil				= (0 << 0x0000),
				no_remove		= (1 << 0x0000),
				no_skip			= (1 << 0x0001),
				no_format		= (1 << 0x0002),
				no_chain		= (1 << 0x0003),
				ignore			= (1 << 0x0004),
				cache_only		= (1 << 0x0005),
				no_cache		= (1 << 0x0006),
				no_halt			= (1 << 0x0007),
				no_throw		= (1 << 0x0008),
			};

			struct source_info_type{
				source_object *object;
				token_formatter_interface *formatter;
				token_comparer_interface *skip;
				token_comparer_interface *halt;
			};

			source_object(ptr_type begin, ptr_type end);

			virtual void save(token_ptr_type value);

			virtual token_ptr_type next(int count = 1, option options = option::nil, list_type *all = nullptr);

			token_ptr_type get(int count = 1, list_type *all = nullptr);

			token_ptr_type get_non_skipped(int count = 1);

			token_ptr_type peek(int count = 1, list_type *all = nullptr);

			token_ptr_type peek_non_skipped(int count = 1);

			void ignore(int count = 1);

			void ignore_non_skipped(int count = 1);

			bool cache(int count = 1);

			bool cache_non_skipped(int count = 1);

			virtual char next_char(int count = 1);

			const index_type &last_index() const;

			static lang_rules rules;
			static source_info_type source_info;

		protected:
			token_ptr_type from_cache_(int &count, option options, list_type *all);

			void new_line_();

			bool is_new_line_(char c);

			marker_info marker_;
			index_type index_;
			index_type last_index_;
			list_type cache_;
		};

		SLANG_MAKE_OPERATORS(source_object::option);
	}
}

#endif /* !SLANG_SOURCE_OBJECT_H */
