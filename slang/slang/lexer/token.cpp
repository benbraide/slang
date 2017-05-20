#include "token.h"

slang::lexer::token::token()
	: id_(id_type::nil){}

slang::lexer::token::id_type slang::lexer::token::id() const{
	return id_;
}

const slang::lexer::token::index_type &slang::lexer::token::index() const{
	return index_;
}

std::string_view slang::lexer::token::value() const{
	if (value_.index() == 0u)
		return std::get<0>(value_);
	return std::string_view(std::get<1>(value_).c_str(), std::get<1>(value_).size());
}
