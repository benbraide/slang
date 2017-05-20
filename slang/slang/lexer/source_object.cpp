#include "source_object.h"
#include "../common/env.h"

slang::lexer::source_object::source_object(ptr_type begin, ptr_type end){
	marker_ = marker_info{ begin, begin, end };
	last_index_ = index_type{};
}

void slang::lexer::source_object::save(token_ptr_type value){
	cache_.push_back(value);
}

slang::lexer::source_object::token_ptr_type slang::lexer::source_object::next(int count, option options, list_type *all){
	if (all != nullptr)//Clear list
		all->clear();

	if (count <= 0)
		return nullptr;

	auto value = SLANG_IS(options, option::no_cache) ? nullptr : from_cache_(count, options, all);
	if (value != nullptr){
		last_index_ = value->index;
		return value;
	}
	
	if (count <= 0 || SLANG_IS(options, option::cache_only))//No match
		return nullptr;

	lang_rules::match_info match_info{};
	while (count > 0){//Extract count tokens
		value = nullptr;
		if (marker_.end <= marker_.current)
			return nullptr;

		if (!rules.match(marker_.current, marker_.end, match_info)){
			if (SLANG_IS(options, option::no_throw)){//Create error token
				match_info.key = token_id::error;
				value = std::make_shared<token_info>(token_info{ match_info, index_ });
				if (SLANG_IS(options, option::no_remove))//Add to cache
					cache_.push_back(value);

				return value;
			}

			//#TODO: Throw exception
			return nullptr;
		}

		if (match_info.key == token_id::new_line)
			new_line_();
		else//Advance column
			index_.column += static_cast<size_type>(match_info.end - marker_.current);

		marker_.current = match_info.end;//Advance marker
		if (!SLANG_IS(options, option::no_format) && source_info.formatter != nullptr)
			source_info.formatter->format(match_info);//Apply format

		if (match_info.key == token_id::error){
			if (SLANG_IS(options, option::no_throw)){//Create error token
				if (value == nullptr)
					value = std::make_shared<token_info>(token_info{ match_info, index_ });

				if (SLANG_IS(options, option::no_remove))//Add to cache
					cache_.push_back(value);

				return value;
			}

			//#TODO: Throw exception
			return nullptr;
		}

		if (!SLANG_IS(options, option::no_halt) && source_info.halt != nullptr){//Check for halt
			if (value == nullptr)
				value = std::make_shared<token_info>(token_info{ match_info, index_ });

			if (source_info.halt->is(value->match_info.key, std::string_view(value->match_info.begin,
				value->match_info.end - value->match_info.begin))){//Halt
				cache_.push_back(value);
				break;
			}
		}

		if (SLANG_IS(options, option::no_remove)){//Add to cache
			if (value == nullptr)
				value = std::make_shared<token_info>(token_info{ match_info, index_ });
			cache_.push_back(value);
		}

		if (all != nullptr){//Add to list
			if (value == nullptr)
				value = std::make_shared<token_info>(token_info{ match_info, index_ });
			all->push_back(value);
		}

		if (!SLANG_IS(options, option::no_skip) && source_info.skip != nullptr){//Check for skip
			if (value == nullptr)
				value = std::make_shared<token_info>(token_info{ match_info, index_ });
			
			if (source_info.skip->is(value->match_info.key, std::string_view(value->match_info.begin,
				value->match_info.end - value->match_info.begin))){//Skip
				value = nullptr;
				continue;
			}
		}
		
		--count;//Decrement count
	}

	last_index_ = index_;
	if (!SLANG_IS(options, option::ignore) && value == nullptr)
		value = std::make_shared<token_info>(token_info{ match_info, index_ });

	return value;
}

slang::lexer::source_object::token_ptr_type slang::lexer::source_object::get(int count, list_type *all){
	return next(count, option::nil, all);
}

slang::lexer::source_object::token_ptr_type slang::lexer::source_object::get_non_skipped(int count){
	return next(count, option::no_skip);
}

slang::lexer::source_object::token_ptr_type slang::lexer::source_object::peek(int count, list_type *all){
	return next(count, option::no_remove, all);
}

slang::lexer::source_object::token_ptr_type slang::lexer::source_object::peek_non_skipped(int count){
	return next(count, option::no_remove | option::no_skip);
}

void slang::lexer::source_object::ignore(int count){
	next(count, option::ignore);
}

void slang::lexer::source_object::ignore_non_skipped(int count){
	next(count, option::ignore | option::no_skip);
}

bool slang::lexer::source_object::cache(int count){
	return (next(count, option::no_cache | option::ignore | option::no_remove | option::no_throw) != nullptr);
}

bool slang::lexer::source_object::cache_non_skipped(int count){
	return (next(count, option::no_cache | option::ignore | option::no_remove | option::no_throw | option::no_skip) != nullptr);
}

char slang::lexer::source_object::next_char(int count){
	auto value = '\0';
	while (count > 0){//Extract count chars
		if (marker_.end <= marker_.current)
			return '\0';

		if (!is_new_line_(value = *(marker_.current++)))//Advance column
			++index_.column;
	}

	last_index_ = index_;
	return value;
}

const slang::lexer::source_object::index_type &slang::lexer::source_object::last_index() const{
	return last_index_;
}

slang::lexer::lang_rules slang::lexer::source_object::rules;

slang::lexer::source_object::source_info_type slang::lexer::source_object::source_info{};

slang::lexer::source_object::token_ptr_type slang::lexer::source_object::from_cache_(int &count, option options, list_type *all){
	token_ptr_type value;
	auto iter = cache_.begin();
	for (; count > 0 && iter != cache_.end(); ++iter){
		value = *iter;
		if (value->match_info.key == token_id::error){
			if (SLANG_IS(options, option::no_throw))
				return value;

			//#TODO: Throw exception
			return nullptr;
		}

		if (!SLANG_IS(options, option::no_halt) && source_info.halt != nullptr){//Check for halt
			if (source_info.halt->is(value->match_info.key, std::string_view(value->match_info.begin,
				value->match_info.end - value->match_info.begin))){//Halt
				cache_.push_back(value);
				count = 0;
				value = nullptr;
				break;
			}
		}

		if (all != nullptr)//Add to list
			all->push_back(value);

		if (!SLANG_IS(options, option::no_skip) && source_info.skip != nullptr){//Check for skip
			if (source_info.skip->is(value->match_info.key, std::string_view(value->match_info.begin,
				value->match_info.end - value->match_info.begin))){//Skip
				value = nullptr;
				continue;
			}
		}

		--count;//Decrement count
		value = nullptr;
	}

	if (!SLANG_IS(options, option::no_remove))//Remove from cache
		cache_.erase(cache_.begin(), iter);

	return value;
}

void slang::lexer::source_object::new_line_(){
	index_ = index_type{ index_.line + 1, 1 };
}

bool slang::lexer::source_object::is_new_line_(char c){
	if (c != '\r' && c != '\n')
		return false;

	auto next = (c == '\r') ? '\n' : '\r';
	if (marker_.current < marker_.end && *marker_.current == next)// \r\n
		++marker_.current;//Ignore
	
	new_line_();
	return true;
}
