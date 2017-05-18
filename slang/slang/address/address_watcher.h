#pragma once

#ifndef SLANG_ADDRESS_WATCHER_H
#define SLANG_ADDRESS_WATCHER_H

#include <functional>
#include <unordered_map>
#include <shared_mutex>

namespace slang{
	namespace address{
		class watcher{
		public:
			typedef unsigned __int64 uint64_type;

			typedef std::function<void(uint64_type)> callback_type;
			typedef std::unordered_map<uint64_type, callback_type> callback_list_type;

			typedef std::shared_mutex lock_type;
			typedef std::lock_guard<lock_type> exclusive_lock_type;
			typedef std::shared_lock<lock_type> shared_lock_type;

			virtual ~watcher();

			virtual uint64_type add(callback_type callback, uint64_type id = 0ull);

			virtual void remove(uint64_type id);

			virtual void on_change(uint64_type value);

		protected:
			callback_list_type callback_list_;
			lock_type lock_;
		};
	}
}

#endif /* !SLANG_ADDRESS_WATCHER_H */
