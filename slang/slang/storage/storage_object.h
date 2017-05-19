#pragma once

#ifndef SLANG_STORAGE_OBJECT_H
#define SLANG_STORAGE_OBJECT_H

#include <list>
#include <shared_mutex>
#include <unordered_map>

#include "storage_value.h"

namespace slang{
	namespace storage{
		class object{
		public:
			typedef value<object> value_type;
			typedef std::unordered_map<std::string, value_type> value_list_type;

			typedef std::pair<const std::string *, value_type *> order_info_type;
			typedef std::list<order_info_type> order_list_type;

			typedef std::shared_mutex lock_type;
			typedef std::lock_guard<lock_type> exclusive_lock_type;
			typedef std::shared_lock<lock_type> shared_lock_type;

			object();

			explicit object(object &parent);

			virtual ~object();

			virtual object *parent() const;

			virtual object *match(const std::string &name);

			virtual value_type *find(const std::string &key, bool recursive = false);

			template <typename target_type, typename... arg_types>
			value_type *add(const std::string &key, arg_types &&... args){
				exclusive_lock_type guard(lock_);
				auto entry = value_list_.try_emplace(key, target_type(std::forward<arg_types>(args)...));
				if (!entry.second)
					return nullptr;

				order_list_.push_front(std::make_pair(&entry.first->first, &entry.first->second));
				entry.first->second.key_ = &entry.first->first;

				return &entry.first->second;
			}

			virtual bool remove(const std::string &key);

			virtual bool remove(value_type &value);

			virtual object &use(const std::string &key, value_type &value);

			virtual object &use(object &storage);

		protected:
			void clean_(value_type &value);

			void use_(const std::string &key, value_type &value);

			object *parent_;
			value_list_type value_list_;
			order_list_type order_list_;
			lock_type lock_;
		};
	}
}

#endif /* !SLANG_STORAGE_OBJECT_H */
