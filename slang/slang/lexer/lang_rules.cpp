#include "lang_rules.h"

#define SLANG_LEXER_RULE_REAL			"[0-9]+\\.[0-9]*"
#define SLANG_LEXER_RULE_REAL2			"\\.[0-9]+"
#define SLANG_LEXER_RULE_EXCLUDES		"\\s\\r\\n\\w\\d\\(\\)\\[\\]\\{\\}\"'`\\\\,;$_"
#define SLANG_LEXER_RULE_ID				"[$_a-zA-Z][$_a-zA-Z0-9]*"

slang::lexer::lang_rules::lang_rules(){
	std::vector<std::string> reserved({
		"module", "export", "import", "enum", "union", "struct", "class", "namespace", "using", "new", "delete", "typedef",
		"operator", "sizeof", "typeof", "echo", "true", "false", "indeterminate", "nan", "nullptr", "auto", "any", "void",
		"bool", "bit", "byte", "char", "wchar", "short", "int", "long", "unsigned", "float", "double", "array", "type_t",
		"try", "catch", "finally", "throw", "if", "else", "unless", "while", "for", "until", "do", "switch", "case", "default",
		"break", "continue", "return", "decltype", "function", "pointer_t", "thread_local", "static", "const", "final", "ref",
		"private", "protected", "public", "static_cast", "reinterpret_cast", "self", "__placeholder", "node_t", "storage_t",
		"nullptr_t", "nan_t"
	});

	std::vector<std::string> lang({
		SLANG_LEXER_RULE_REAL "[eE][+-]?[0-9]+",					//Exponentiated number
		SLANG_LEXER_RULE_REAL2 "[eE][+-]?[0-9]+",					//Exponentiated number
		"[0-9]+[eE][+-]?[0-9]+",									//Exponentiated number
		SLANG_LEXER_RULE_REAL,										//Real number
		SLANG_LEXER_RULE_REAL2,										//Real number
		"[3][0-6]r[0-9a-zA-Z]+",									//Radix integer
		"[12][0-9]r[0-9a-zA-Z]+",									//Radix integer
		"[2-9]r[0-9a-zA-Z]+",										//Radix integer
		"0x[0-9a-fA-F]+",											//Hexadecimal integer
		"[1-9][0-9]*",												//Decimal integer
		"[0][0-7]*",												//Octal integer
		"0b[01]+",													//Binary integer
		SLANG_LEXER_RULE_ID,										//Identifier
		"\"",														//Escaped double quoted string
		"'",														//Escaped single quoted string
		"`",														//Escaped back quoted string
		"@\"",														//Double quoted string
		"@'",														//Single quoted string
		"@`",														//Back quoted string
		"\\[",														//Symbol
		"\\(",														//Symbol
		"\\{",														//Symbol
		"\\]",														//Symbol
		"\\)",														//Symbol
		"\\}",														//Symbol
		";",														//Symbol
		",",														//Symbol
		"\\\\",														//Symbol
		"//[^\\r\\n]*",												//Single line comment
		"/\\*",														//Multiline comment
		"\\r\\n",													//New line
		"\\n\\r",													//New line
		"[\\r\\n]",													//New line
		"[\\s]+",													//Blanks
		"::",														//Symbol
		":",														//Symbol
	});

	std::vector<std::string> punc({
		"[^\\r\\n" SLANG_LEXER_RULE_EXCLUDES "]{4,}",
		"===",
		"!==",
		"<<=",
		">>=",
		"\\.\\.\\.",
		"[^\\r\\n" SLANG_LEXER_RULE_EXCLUDES "]{3}",
		"\\|\\|",
		"&&",
		"~=",
		"==",
		"!=",
		"<=",
		">=",
		"\\+=",
		"\\-=",
		"\\*=",
		"/=",
		"%=",
		"\\|=",
		"&=",
		"\\^=",
		"\\->",
		"\\+\\+",
		"\\-\\-",
		"<<",
		">>",
		"[^\\r\\n" SLANG_LEXER_RULE_EXCLUDES "]{2}",
		"\\?",
		"\\|",
		"\\^",
		"&",
		"<",
		">",
		"\\+",
		"\\-",
		"\\*",
		"/",
		"%",
		"\\.",
		"!",
		"~",
		"=",
		"[^\\r\\n" SLANG_LEXER_RULE_EXCLUDES "]{1}"
	});

	std::string combined_value;
	for (auto &value : reserved){
		if (combined_value.empty())
			combined_value = "(" + value + "\\b)";
		else//Append
			combined_value += "|(" + value + "\\b)";
	}

	for (auto &value : lang){
		if (combined_value.empty())
			combined_value = "(" + value + ")";
		else//Append
			combined_value += "|(" + value + ")";
	}

	for (auto &value : punc){
		if (combined_value.empty())
			combined_value = "(" + value + ")";
		else//Append
			combined_value += "|(" + value + ")";
	}

	compiled_.assign("^" + combined_value, boost::regex_constants::optimize);
	map_ = list_type({
		token_id::module_,
		token_id::export_,
		token_id::import_,
		token_id::enum_,
		token_id::union_,
		token_id::struct_,
		token_id::class_,
		token_id::namespace_,
		token_id::using_,
		token_id::new_,
		token_id::delete_,
		token_id::typedef_,
		token_id::operator_,
		token_id::sizeof_,
		token_id::typeof,
		token_id::echo,
		token_id::true_,
		token_id::false_,
		token_id::indeterminate,
		token_id::nan,
		token_id::nullptr_,
		token_id::auto_,
		token_id::any,
		token_id::void_,
		token_id::bool_,
		token_id::bit,
		token_id::byte,
		token_id::char_,
		token_id::wchar,
		token_id::short_,
		token_id::int_,
		token_id::long_,
		token_id::unsigned_,
		token_id::float_,
		token_id::double_,
		token_id::array_,
		token_id::type_,
		token_id::try_,
		token_id::catch_,
		token_id::finally_,
		token_id::throw_,
		token_id::if_,
		token_id::else_,
		token_id::unless,
		token_id::while_,
		token_id::for_,
		token_id::until,
		token_id::do_,
		token_id::switch_,
		token_id::case_,
		token_id::default_,
		token_id::break_,
		token_id::continue_,
		token_id::return_,
		token_id::decltype_,
		token_id::function,
		token_id::pointer,
		token_id::tls,
		token_id::static_,
		token_id::const_,
		token_id::final_,
		token_id::ref,
		token_id::private_,
		token_id::protected_,
		token_id::public_,
		token_id::static_cast_,
		token_id::reinterpret_cast_,
		token_id::self,
		token_id::placeholder,
		token_id::node,
		token_id::storage,
		token_id::nullptr_t,
		token_id::nan_t,
		token_id::exp,
		token_id::exp,
		token_id::exp,
		token_id::real,
		token_id::real,
		token_id::rad,
		token_id::rad,
		token_id::rad,
		token_id::hex,
		token_id::dec,
		token_id::oct,
		token_id::bin,
		token_id::identifier,
		token_id::esc_quote_dbl,
		token_id::esc_quote_sng,
		token_id::esc_quote_back,
		token_id::quote_dbl,
		token_id::quote_sng,
		token_id::quote_back,
		token_id::open_sq,
		token_id::open_par,
		token_id::open_cur,
		token_id::close_sq,
		token_id::close_par,
		token_id::close_cur,
		token_id::semi_colon,
		token_id::comma,
		token_id::backslash,
		token_id::comment_sng,
		token_id::comment_mult,
		token_id::new_line,
		token_id::new_line,
		token_id::new_line,
		token_id::blank,
		token_id::scope_resolution,
		token_id::colon,
		token_id::punct,
		token_id::explicit_equality,
		token_id::explicit_inverse_equality,
		token_id::compound_left_shift,
		token_id::compound_right_shift,
		token_id::ellipsis,
		token_id::punct,
		token_id::relational_or,
		token_id::relational_and,
		token_id::like,
		token_id::equality,
		token_id::inverse_equality,
		token_id::less_or_equal,
		token_id::more_or_equal,
		token_id::compound_plus,
		token_id::compound_minus,
		token_id::compound_times,
		token_id::compound_divide,
		token_id::compound_modulus,
		token_id::compound_bitwise_or,
		token_id::compound_bitwise_and,
		token_id::compound_bitwise_xor,
		token_id::member_pointer_access,
		token_id::increment,
		token_id::decrement,
		token_id::left_shift,
		token_id::right_shift,
		token_id::punct,
		token_id::ternary,
		token_id::bitwise_or,
		token_id::bitwise_xor,
		token_id::bitwise_and,
		token_id::less,
		token_id::more,
		token_id::plus,
		token_id::minus,
		token_id::times,
		token_id::divide,
		token_id::modulus,
		token_id::member_access,
		token_id::relational_not,
		token_id::bitwise_inverse,
		token_id::assignment,
		token_id::punct,
	});
}
