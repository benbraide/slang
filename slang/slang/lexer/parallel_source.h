#pragma once

#ifndef SLANG_PARALLEL_SOURCE_H
#define SLANG_PARALLEL_SOURCE_H

#include <mutex>
#include <functional>

#include "../utilities/thread_pool.h"
#include "source_object.h"

namespace slang{
	namespace lexer{
		struct parallel_source_helper{
			static void run(std::function<void()> callback);
		};

		template <class source_type>
		class parallel_source : public source_type{
		public:
			typedef source_type source_type;

			typedef source_object::size_type size_type;
			typedef source_object::token_ptr_type token_ptr_type;
			typedef source_object::list_type list_type;
			typedef source_object::option option;

			typedef std::recursive_mutex lock_type;
			typedef std::lock_guard<lock_type> guard_type;

			struct run_info{
				size_type max;
				unsigned long sleep_duration;
			};

			template <typename... arg_types>
			explicit parallel_source(const run_info &info, arg_types &&... args)
				: source_type(std::forward<arg_types>(args)...){
				parallel_source_helper::run([this, info]{
					while (true){
						if (source_type::cache_.size() < info.max && !source_type::cache_non_skipped())
							break;//Done

						std::this_thread::sleep_for(std::chrono::milliseconds(info.sleep_duration));
					}
				});
			}

			virtual void save(token_ptr_type value){
				guard_type guard(lock_);
				source_type::save(value);
			}

			virtual token_ptr_type next(int count = 1, option options = option::nil, list_type *all = nullptr) override{
				guard_type guard(lock_);
				return source_type::next(count, options, all);
			}

			virtual char next_char(int count = 1) override{
				guard_type guard(lock_);
				return source_type::next_char(count);
			}

		protected:
			lock_type lock_;
		};
	}
}

#endif /* !SLANG_PARALLEL_SOURCE_H */
