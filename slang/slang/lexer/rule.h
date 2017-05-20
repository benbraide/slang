#pragma once

#ifndef SLANG_RULE_H
#define SLANG_RULE_H

#include <vector>
#include <boost/regex.hpp>

namespace slang{
	namespace lexer{
		template <class key_type>
		class rule{
		public:
			typedef const char *ptr_type;

			typedef std::vector<key_type> list_type;
			typedef typename list_type::size_type size_type;

			struct match_info{
				key_type key;
				ptr_type begin;
				ptr_type end;
			};

			bool match(ptr_type begin, ptr_type end, match_info &matched, bool whole = false){
				boost::cmatch results;
				if (whole){//Match whole range
					if (!boost::regex_match(begin, end, results, compiled_))
						return false;
				}
				else if (!boost::regex_search(begin, end, results, compiled_))
					return false;

				auto index = matched_index_(results);
				if (map_.size() <= index)
					return false;

				matched.key = map_[index];
				matched.begin = (begin + results.position());
				matched.end = (begin + results.position() + results.length());

				return true;
			}

		protected:
			size_type matched_index_(boost::cmatch &results){
				size_type index = 0;
				for (auto match = std::next(results.begin()); match != results.end(); ++match, ++index){
					if (match->matched)
						break;
				}

				return index;
			}

			list_type map_;
			boost::regex compiled_;
		};
	}
}

#endif /* !SLANG_RULE_H */
