#include "type_mapper.h"
#include "../common/env.h"

slang::type::object::ptr_type slang::type::static_mapper::map(type::id id){
	return common::env::map_type(id);
}
