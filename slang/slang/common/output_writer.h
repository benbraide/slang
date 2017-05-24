#pragma once

#ifndef SLANG_OUTPUT_WRITER_H
#define SLANG_OUTPUT_WRITER_H

#include <mutex>
#include <string_view>

#include "output_writer_interface.h"

namespace slang{
	namespace common{
		struct output_writer_helper{
			static void lock_ouput(bool is_locked);

			static void enable_append(bool enabled);

			static bool is_locked();

			static bool append_is_enabled();
		};

		template <class output_type, class wide_output_type>
		class output_writer : public output_writer_interface{
		public:
			typedef output_type output_type;
			typedef wide_output_type wide_output_type;

			typedef std::mutex lock_type;
			typedef std::lock_guard<lock_type> guard_type;

			output_writer(output_type &output, wide_output_type &wide_output)
				: output_(&output), wide_output_(&wide_output){}

			virtual ~output_writer() = default;

			virtual void begin() override{
				lock_.lock();
				output_writer_helper::enable_append(true);
			}

			virtual void end(bool end_line = true) override{
				if (end_line)
					*output_ << "\n";

				output_writer_helper::enable_append(false);
				lock_.unlock();
			}

			virtual void write(const char *value, size_type size = 0u) override{
				if (!output_writer_helper::is_locked()){
					guard_type guard(lock_);
					output_writer_helper::lock_ouput(true);

					write_(value, size);
					output_writer_helper::lock_ouput(false);
				}
				else//Already locked
					write_(value, size);
			}

			virtual void write(const wchar_t *value, size_type size = 0u) override{
				if (!output_writer_helper::is_locked()){
					guard_type guard(lock_);
					output_writer_helper::lock_ouput(true);

					write_(value, size);
					output_writer_helper::lock_ouput(false);
				}
				else//Already locked
					write_(value, size);
			}

			virtual bool has_begun() const override{
				return output_writer_helper::is_locked();
			}

		protected:
			virtual void write_(const char *value, size_type size){
				if (size == static_cast<size_type>(0u))
					*output_ << value;
				else//Size specified
					*output_ << std::string_view(value, size);

				if (!output_writer_helper::append_is_enabled())
					*output_ << "\n";
			}

			virtual void write_(const wchar_t *value, size_type size){
				if (size == static_cast<size_type>(0u))
					*wide_output_ << value;
				else//Size specified
					*wide_output_ << std::wstring_view(value, size);

				if (!output_writer_helper::append_is_enabled())
					*wide_output_ << L"\n";
			}

			output_type *output_;
			wide_output_type *wide_output_;
			lock_type lock_;
		};
	}
}

#endif /* !SLANG_OUTPUT_WRITER_H */
