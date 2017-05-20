#pragma once

#ifndef SLANG_BOOL_TYPE_H
#define SLANG_BOOL_TYPE_H

namespace slang{
	namespace type{
		enum class bool_type : char{
			indeterminate	= '\x0',
			false_			= '\x1',
			true_			= '\x2',
		};
	}
}

#endif /* !SLANG_BOOL_TYPE_H */
