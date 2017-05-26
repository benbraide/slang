#pragma once

#ifndef SLANG_TYPE_ID_H
#define SLANG_TYPE_ID_H

namespace slang{
	namespace type{
		enum class id{
			nil,
			auto_,
			any,
			void_,
			bool_,
			bit,
			byte,
			char_,
			uchar,
			wchar,
			short_,
			ushort,
			int_,
			uint,
			long_,
			ulong,
			llong,
			ullong,
			float_,
			double_,
			ldouble,
			array_,
			pointer,
			function,
			nullptr_,
			type_,
			node_,
			storage_,
			variant,
			compile_t,
			runtime_t,
			enum_,
			union_,
			struct_,
			class_,
			string_,
			wstring_,
			nan,
		};
	}
}

#endif /* !SLANG_TYPE_ID_H */
