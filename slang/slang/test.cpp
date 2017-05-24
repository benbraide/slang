#include "common/env.h"
#include "lexer/lang_rules.h"

int main(){
	slang::common::env::bootstrap();
	slang::storage::temp tmp;

	auto val = tmp.add(4.5f), val2 = tmp.add(108);

	slang::driver::object::get_driver(*val)->echo(*val);
	slang::driver::object::get_driver(*val2)->echo(*val2);

	slang::common::binary_operator_info boi{ slang::lexer::operator_id::plus };
	auto val3 = slang::driver::object::get_driver(*val)->evaluate(*val, boi, *val2);
	slang::driver::object::get_driver(*val3)->echo(*val3);

	slang::common::env::tear_down();
	return 0;
}
