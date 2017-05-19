#pragma once

#ifndef SLANG_STORAGE_VALUE_H
#define SLANG_STORAGE_VALUE_H

#include <variant>
#include <memory>

#include "storage_entry.h"

namespace slang{
	namespace storage{
		template <class object_type>
		class value{
		public:
			typedef object_type object_type;
			typedef entry::type_ptr_type type_ptr_type;

			typedef std::shared_ptr<object_type> object_ptr_type;
			typedef std::variant<entry, object_ptr_type, type_ptr_type> info_type;

			template <typename arg_type>
			explicit value(const arg_type &arg)
				: info_(arg), key_(nullptr){}

			entry *object(){
				return is_object() ? &std::get<entry>(info_) : nullptr;
			}

			object_type *storage(bool convert = true){
				if (!is_storage())//Try converting type to storage
					return convert ? dynamic_cast<object_type *>(type().get()) : nullptr;
				return std::get<object_ptr_type>(info_).get();
			}

			type_ptr_type type(){
				return is_type() ? std::get<type_ptr_type>(info_) : nullptr;
			}

			bool is_object() const{
				return (info_.index() == 0u);
			}

			bool is_storage() const{
				return (info_.index() == 1u);
			}

			bool is_type() const{
				return (info_.index() == 2u);
			}

		private:
			friend object_type;

			info_type info_;
			const std::string *key_;
		};
	}
}

#endif /* !SLANG_STORAGE_VALUE_H */
