#pragma once

#ifndef SLANG_OUTPUT_WRITER_INTERFACE_H
#define SLANG_OUTPUT_WRITER_INTERFACE_H

#include <cstdlib>

namespace slang{
	namespace common{
		class output_writer_interface{
		public:
			typedef std::size_t size_type;

			virtual ~output_writer_interface() = default;

			virtual void begin() = 0;

			virtual void end(bool end_line = true) = 0;

			virtual void write(const char *value, size_type size = 0u) = 0;

			virtual void write(const wchar_t *value, size_type size = 0u) = 0;

			virtual bool has_begun() const = 0;
		};
	}
}

#endif /* !SLANG_OUTPUT_WRITER_INTERFACE_H */
