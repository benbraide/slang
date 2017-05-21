#pragma once

#ifndef SLANG_ERROR_H
#define SLANG_ERROR_H

#include "../lexer/source_object.h"
#include "../storage/storage_entry.h"

namespace slang{
	namespace common{
		class error{
		public:
			typedef lexer::source_object::index_type index_type;
			typedef storage::entry storage_entry_type;
			typedef type::object::id_type type_id_type;

			typedef std::size_t size_type;

			enum class type{
				nil,
				return_,
				break_,
				continue_,
				suppressed,
			};

			~error();

			void set(storage_entry_type *object);

			void set(type type, storage_entry_type *object = nullptr);

			void set(const char *value, bool is_runtime = false);

			void set(const char *value, size_type size);

			void set(const std::string &value, const index_type &index, bool is_runtime = false);

			template <typename value_type>
			value_type set_and_return(const char *value, bool is_runtime = false){
				set(value, is_runtime);
				return value_type();
			}

			template <typename value_type>
			value_type set_and_return(const std::string &value, const index_type &index, bool is_runtime = false){
				set(value, index, is_runtime);
				return value_type();
			}

			void warn(const char *value);

			void suppress();

			void unsuppress();

			void clear();

			void dump();

			storage_entry_type *get();

			bool has() const;

			bool is_error() const;

			bool is_return() const;

			bool is_break() const;

			bool is_continue() const;

			bool is_suppressed() const;

		private:
			type type_ = type::nil;
			storage_entry_type object_;
		};
	}
}

#endif /* !SLANG_ERROR_H */
