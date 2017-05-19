#include "type_mapper.h"
#include "../common/env.h"

slang::type::object::ptr_type slang::type::static_mapper::map(object::id_type id){
	return common::env::map_type(id);
}
