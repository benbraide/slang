#include "common/env.h"
#include "lexer/lang_rules.h"

int main(){
	slang::common::env::bootstrap();

	slang::lexer::lang_rules lr;
	slang::lexer::lang_rules::match_info mi;
	
	std::string m("##ben");
	lr.match(m.c_str(), m.c_str() + m.size(), mi);
	lr.match(mi.end, m.c_str() + m.size(), mi);

	return 0;
}
