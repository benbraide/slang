#pragma once

#ifndef SLANG_ADDRESS_TABLE_H
#define SLANG_ADDRESS_TABLE_H

#include <thread>
#include <shared_mutex>
#include <memory>
#include <functional>
#include <unordered_map>
#include <map>
#include <climits>
#include <vector>

#include "address_head.h"
#include "address_dependency.h"
#include "address_range.h"
#include "address_watcher.h"

namespace slang_test{
	class address_table_test;
}

namespace slang{
	namespace address{
		class table{
		public:
			typedef head::attribute_type attribute_type;
			typedef head::uint64_type uint64_type;
			typedef head::uint_type uint_type;
			typedef range<uint64_type> range_type;

			typedef std::thread::id thread_id_type;
			typedef std::shared_mutex lock_type;
			typedef std::lock_guard<lock_type> exclusive_lock_type;
			typedef std::shared_lock<lock_type> shared_lock_type;

			typedef std::unordered_map<uint64_type, head> head_list_type;
			typedef std::map<uint64_type, uint_type> available_list_type;

			typedef std::shared_ptr<dependency> dependency_ptr_type;
			typedef std::unordered_map<uint64_type, dependency_ptr_type> dependency_list_type;

			typedef std::shared_ptr<watcher> watcher_ptr_type;
			typedef std::map<thread_id_type, watcher_ptr_type> watcher_ptr_list_type;
			typedef std::unordered_map<uint64_type, watcher_ptr_list_type> watcher_list_type;

			explicit table(uint64_type protected_range = 0u);

			~table();

			void on_thread_entry();

			void on_thread_exit();

			void capture_tls(uint64_type value);

			void protect();

			void set_dependency(uint64_type value, dependency_ptr_type dependency);

			template <typename dependency_type, typename... arg_types>
			void set_dependency(uint64_type value, arg_types &&... args){
				set_dependency(value, std::make_shared<dependency_type>(std::forward<arg_types>(args)...));
			}

			watcher *watch(uint64_type value, watcher_ptr_type watcher);

			template <typename watcher_type, typename... arg_types>
			watcher *watch(uint64_type value, arg_types &&... args){
				return watch(value, std::make_shared<watcher_type>(std::forward<arg_types>(args)...));
			}

			watcher *find_watcher(uint64_type value) const;

			dependency *get_dependency(uint64_type value) const;

			bool deallocate(uint64_type value);

			head *allocate(uint_type size);

			head *allocate(uint_type count, uint_type size);

			template <typename value_type>
			head *allocate(){
				auto entry = allocate(sizeof(value_type));
				if (std::is_floating_point<value_type>::value && entry != nullptr)
					SLANG_SET(entry->attributes, attribute_type::is_float);

				return entry;
			}

			template <typename value_type>
			head *allocate_scalar(value_type value){
				auto entry = allocate(sizeof(value_type));
				if (entry != nullptr){//Successful allocation
					entry->attributes = attribute_type::write_protect;
					if (std::is_floating_point<std::decay_t<value_type>>::value)
						SLANG_SET(entry->attributes, attribute_type::is_float);

					*reinterpret_cast<value_type *>(entry->ptr) = value;
				}

				return entry;
			}

			head *allocate_scalar(const char *value);

			head *allocate_scalar(const wchar_t *value);

			template <typename value_type>
			head *allocate_scalar(const value_type *value, uint_type size){
				auto entry = allocate(size * sizeof(value_type));
				if (entry != nullptr){//Successful allocation
					entry->attributes = attribute_type::write_protect;

					auto ptr = (const char *)value;//Cast to bytes
					for (auto i = 0u; i < entry->size; ++i)//Copy bytes
						*(entry->ptr + i) = *(ptr + i);
				}

				return entry;
			}

			head *allocate_scalar_cstr(const char *value, uint_type size = 0u);

			head *allocate_scalar_wcstr(const wchar_t *value, uint_type size = 0u);

			head *reallocate(uint64_type value, uint_type size);

			head *allocate_reserved(uint64_type value, uint_type size);

			uint64_type reserve(uint_type size);

			head *find(uint64_type value) const;

			head *get_head(uint64_type value) const;

			bool contains(uint64_type value) const;

			void copy(uint64_type destination, uint64_type source, uint_type size);

			void copy(uint64_type destination, uint64_type source);

			void set(uint64_type value, char c, uint_type size);

			void write(uint64_type destination, const char *source, uint_type size);

			void write(uint64_type destination, const wchar_t *source, uint_type size);

			void write(uint64_type destination, const char *source);

			void write(uint64_type destination, const wchar_t *source);

			template <typename value_type>
			void write(uint64_type destination, value_type source){
				write(destination, reinterpret_cast<const char *>(&source), sizeof(value_type));
			}

			template <typename value_type>
			void write_numeric(uint64_type destination, value_type source){
				if (!is_locked_()){
					set_locked_state_(true);
					shared_lock_type guard(lock_);

					write_numeric_(destination, source);
					set_locked_state_(false);
				}
				else//Already locked
					write_numeric_(destination, source);
			}

			void read(uint64_type value, char *buffer, uint_type size) const;

			template <typename value_type>
			value_type read(uint64_type value) const{
				if (!is_locked_()){
					set_locked_state_(true);
					shared_lock_type guard(lock_);

					auto result = read_<value_type>(value);
					set_locked_state_(false);

					return result;
				}

				return read_<value_type>(value);
			}

			void convert_numeric(uint64_type destination, uint64_type source) const;

			template <typename value_type>
			value_type convert_numeric(uint64_type value) const{
				if (!is_locked_()){
					set_locked_state_(true);
					shared_lock_type guard(lock_);

					auto result = convert_numeric_<value_type>(value);
					set_locked_state_(false);

					return result;
				}

				return convert_numeric_<value_type>(value);
			}

		private:
			friend class slang_test::address_table_test;

			void on_thread_entry_();

			void on_thread_exit_();

			void capture_tls_(uint64_type value);

			watcher *watch_(uint64_type value, watcher_ptr_type watcher);

			dependency *get_dependency_(uint64_type value) const;

			bool deallocate_(uint64_type value, bool merge = true, bool ignore_tls = false);

			head *allocate_(uint_type size, uint64_type value = 0ull);

			head *allocate_contiguous_(uint_type count, uint_type size);

			head *reallocate_(uint64_type value, uint_type size);

			head *shrink_(head &head, uint_type size);

			head *expand_(head &head, uint_type size);

			uint64_type reserve_(uint_type size);

			head *find_(uint64_type value) const;

			head *get_head_(uint64_type value) const;

			watcher *find_watcher_(uint64_type value) const;

			void copy_(uint64_type destination, uint64_type source, uint_type size);

			void copy_(uint64_type destination, uint64_type source);

			void write_(uint64_type value, const char *source, uint_type size, bool is_array);

			template <typename value_type>
			void write_numeric_(uint64_type destination, value_type source){
				auto destination_entry = find_(destination);
				if (destination_entry == nullptr){
					throw_("Memory write access violation.");
					return;
				}

				auto is_indirect = SLANG_IS_ANY(destination_entry->attributes, attribute_type::is_string | attribute_type::indirect);
				uint64_type linked_value = (is_indirect ? *reinterpret_cast<uint64_type *>(destination_entry->ptr) : 0u);

				if (!SLANG_IS(destination_entry->attributes, attribute_type::is_float)){
					switch (destination_entry->size){
					case sizeof(__int8):
						*reinterpret_cast<__int8 *>(destination_entry->ptr) = static_cast<__int8>(source);
						break;
					case sizeof(__int16):
						*reinterpret_cast<__int16 *>(destination_entry->ptr) = static_cast<__int16>(source);
						break;
					case sizeof(__int32):
						*reinterpret_cast<__int32 *>(destination_entry->ptr) = static_cast<__int32>(source);
						break;
					case sizeof(__int64):
						*reinterpret_cast<__int64 *>(destination_entry->ptr) = static_cast<__int64>(source);
						break;
					default:
						throw_("Failed to write numeric - invalid memory layout.");
						return;
					}
				}
				else{//Floating point
					switch (destination_entry->size){
					case sizeof(float):
						*reinterpret_cast<float *>(destination_entry->ptr) = static_cast<float>(source);
						break;
					case sizeof(long double):
						*reinterpret_cast<long double *>(destination_entry->ptr) = static_cast<long double>(source);
						break;
					default:
						throw_("Failed to write numeric - invalid memory layout.");
						return;
					}
				}

				if (is_indirect)//Deallocate linked memory
					deallocate_(linked_value, true, true);
			}

			void read_(uint64_type value, char *buffer, uint_type size) const;

			template <typename value_type>
			value_type read_(uint64_type value) const{
				auto entry = find_(value);
				if (entry != nullptr && entry->actual_size >= sizeof(value_type))
					return *reinterpret_cast<value_type *>(entry->ptr);

				value_type out_value = value_type();
				read_(value, reinterpret_cast<char *>(&out_value), static_cast<uint_type>(sizeof(value_type)));

				return out_value;
			}

			void convert_numeric_(uint64_type destination, uint64_type source) const;

			template <typename value_type>
			value_type convert_numeric_(uint64_type value) const{
				auto entry = find_(value);
				if (entry == nullptr)//Read value
					return read_<value_type>(value);

				if (!SLANG_IS(entry->attributes, attribute_type::is_float)){
					switch (entry->size){
					case sizeof(__int8):
						return static_cast<value_type>(*reinterpret_cast<__int8 *>(entry->ptr));
					case sizeof(__int16):
						return static_cast<value_type>(*reinterpret_cast<__int16 *>(entry->ptr));
					case sizeof(__int32):
						return static_cast<value_type>(*reinterpret_cast<__int32 *>(entry->ptr));
					case sizeof(__int64):
						return static_cast<value_type>(*reinterpret_cast<__int64 *>(entry->ptr));
					default:
						break;
					}
				}
				else{//Floating point
					switch (entry->size){
					case sizeof(float):
						return static_cast<value_type>(*reinterpret_cast<float *>(entry->ptr));
					case sizeof(long double):
						return static_cast<value_type>(*reinterpret_cast<long double *>(entry->ptr));
					default:
						break;
					}
				}

				return read_<value_type>(value);
			}

			void merge_available_(uint64_type value, uint_type size);

			uint64_type get_available_(uint_type size, uint64_type match = 0ull);

			void throw_(const char *err) const;

			template <typename value_type>
			value_type throw_and_return_(const char *err) const{
				throw_(err);
				return value_type();
			}

			bool is_locked_() const;

			void set_locked_state_(bool is_locked) const;

			static thread_local head_list_type tls_;

			uint64_type next_;
			uint64_type protected_;
			head_list_type head_list_;
			head_list_type tls_captures_;
			available_list_type available_list_;
			dependency_list_type dependencies_;
			watcher_list_type watchers_;
			thread_id_type thread_id_;
			mutable lock_type lock_;
		};
	}
}

#endif /* !SLANG_ADDRESS_TABLE_H */
