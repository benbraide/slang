#pragma once

#ifndef SLANG_THREAD_POOL_H
#define SLANG_THREAD_POOL_H

#include <mutex>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <list>

#ifndef SLANG_THREAD_POOL_POLL_SLEEP
#define SLANG_THREAD_POOL_POLL_SLEEP 5
#endif // !SLANG_THREAD_POOL_POLL_SLEEP

#ifndef SLANG_THREAD_POOL_POLL_TIMEOUT
#define SLANG_THREAD_POOL_POLL_TIMEOUT 2000
#endif // !SLANG_THREAD_POOL_POLL_TIMEOUT

namespace slang{
	namespace utilities{
		class thread_pool{
		public:
			typedef std::size_t size_type;

			typedef std::function<void()> task_type;
			typedef std::thread thread_type;

			typedef std::list<task_type> list_type;
			typedef std::list<thread_type> thread_list_type;

			typedef thread_list_type::iterator thread_iterator_type;

			typedef std::mutex lock_type;
			typedef std::lock_guard<lock_type> guard_type;
			typedef std::unique_lock<lock_type> unique_guard_type;

			typedef std::condition_variable condition_type;

			typedef void(thread_pool::*run_method_type)(thread_iterator_type, task_type);

			enum class option{
				nil,
				dedicated,
				parallel,
			};

			thread_pool();

			thread_pool(size_type min_size, size_type max_size);

			~thread_pool();

			void stop();

			void update_sizes(size_type min_size, size_type max_size);

			bool add(task_type task, option options = option::nil);

			size_type min_size() const;

			size_type max_size() const;

			bool has_tasks() const;

		private:
			bool add_persistent_(task_type task);

			void do_run_(task_type task, run_method_type method);

			void run_(thread_iterator_type thread, task_type task);

			void run_persistent_(thread_iterator_type thread, task_type task);

			size_type min_;
			size_type max_;

			size_type free_count_;
			size_type thread_count_;

			list_type tasks_;
			thread_list_type threads_;

			bool destruct_;
			condition_type condition_;
			lock_type lock_;
		};
	}
}

#endif /* !SLANG_THREAD_POOL_H */
