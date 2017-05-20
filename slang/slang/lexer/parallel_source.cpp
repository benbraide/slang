#include "parallel_source.h"
#include "../common/env.h"

void slang::lexer::parallel_source_helper::run(std::function<void()> callback){
	common::env::thread_pool.add(callback, utilities::thread_pool::option::parallel);
}
