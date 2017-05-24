#include "output_writer.h"
#include "../common/env.h"

void slang::common::output_writer_helper::lock_ouput(bool is_locked){
	if (is_locked)
		SLANG_SET(common::env::runtime.state, common::env::runtime_state::output_writer_locked);
	else//Unlock
		SLANG_REMOVE(common::env::runtime.state, common::env::runtime_state::output_writer_locked);
}

void slang::common::output_writer_helper::enable_append(bool enabled){
	if (enabled)
		SLANG_SET(common::env::runtime.state, common::env::runtime_state::writer_append_mode);
	else//Disabled
		SLANG_REMOVE(common::env::runtime.state, common::env::runtime_state::writer_append_mode);
}

bool slang::common::output_writer_helper::is_locked(){
	return SLANG_IS(common::env::runtime.state, common::env::runtime_state::output_writer_locked);
}

bool slang::common::output_writer_helper::append_is_enabled(){
	return SLANG_IS(common::env::runtime.state, common::env::runtime_state::writer_append_mode);
}
