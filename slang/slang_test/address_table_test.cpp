#include "stdafx.h"
#include "CppUnitTest.h"

#include "../slang/common/error.cpp"
#include "../slang/common/env.cpp"

#include "../slang/address/address_table.cpp"
#include "../slang/utilities/thread_pool.cpp"

#include "../slang/storage/storage_entry.cpp"
#include "../slang/storage/storage_object.cpp"
#include "../slang/storage/named_storage.cpp"
#include "../slang/storage/temp_storage.cpp"

#include "../slang/type/type_mapper.cpp"
#include "../slang/type/type_object.cpp"
#include "../slang/type/primitive_type.cpp"
#include "../slang/type/pointer_type.cpp"
#include "../slang/type/modified_type.cpp"

namespace Microsoft{
	namespace VisualStudio{
		namespace CppUnitTestFramework{
			template<> inline std::wstring ToString<long long>(const long long& t)                  { RETURN_WIDE_STRING(t); }
			template<> inline std::wstring ToString<long long>(const long long* t)                  { RETURN_WIDE_STRING(t); }
			template<> inline std::wstring ToString<long long>(long long* t)                        { RETURN_WIDE_STRING(t); }
		}
	}
}

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace slang_test{
	TEST_CLASS(address_table_test){
	public:
		TEST_METHOD(deallocate){
			slang::address::table table;

			auto size = table.head_list_.size();
			auto value = table.allocate(3u, sizeof(__int32))->value;

			Assert::IsTrue(table.deallocate(value), L"Deallocation failed");//Index: 0

			Assert::AreEqual(table.head_list_.size(), size + 2u, L"Head list incorrect");
			Assert::AreEqual(table.available_list_.size(), 1ull, L"Available list incorrect");

			Assert::AreEqual(table.available_list_.begin()->first, value, L"Available address incorrect");
			Assert::AreEqual(table.available_list_.begin()->second, sizeof(__int32), L"Available size incorrect");

			Assert::AreEqual(table.available_list_.rbegin()->first, value, L"Available address incorrect");
			Assert::AreEqual(table.available_list_.rbegin()->second, sizeof(__int32), L"Available size incorrect");

			Assert::IsTrue(table.deallocate(value + (sizeof(__int32) * 2)), L"Deallocation failed");//Index: 2

			Assert::AreEqual(table.head_list_.size(), size + 1u, L"Head list incorrect");
			Assert::AreEqual(table.available_list_.size(), 2ull, L"Available list incorrect");

			Assert::AreEqual(table.available_list_.begin()->first, value, L"Available address incorrect");
			Assert::AreEqual(table.available_list_.begin()->second, sizeof(__int32), L"Available size incorrect");

			Assert::AreEqual(table.available_list_.rbegin()->first, value + (sizeof(__int32) * 2), L"Available address incorrect");
			Assert::AreEqual(table.available_list_.rbegin()->second, sizeof(__int32), L"Available size incorrect");

			Assert::IsTrue(table.deallocate(value + sizeof(__int32)), L"Deallocation failed");//Index: 1

			Assert::AreEqual(table.head_list_.size(), size + 0u, L"Head list incorrect");
			Assert::AreEqual(table.available_list_.size(), 1ull, L"Available list incorrect");

			Assert::AreEqual(table.available_list_.begin()->first, value, L"Available address incorrect");
			Assert::AreEqual(table.available_list_.begin()->second, (sizeof(__int32) * 3), L"Available size incorrect");

			Assert::AreEqual(table.available_list_.rbegin()->first, value, L"Available address incorrect");
			Assert::AreEqual(table.available_list_.rbegin()->second, (sizeof(__int32) * 3), L"Available size incorrect");

			Assert::IsFalse(table.deallocate(value), L"Deallocation failed");//Index: Invalid
		}

		TEST_METHOD(allocate){
			slang::address::table table;

			auto address = table.next_;
			auto size = table.head_list_.size();
			auto entry = table.allocate(sizeof(__int32));//Single allocation by size

			Assert::IsNotNull(entry);
			Assert::IsNotNull(entry->ptr);

			Assert::AreEqual(entry->value, address);
			Assert::AreEqual(entry->size, sizeof(__int32));
			Assert::AreEqual(entry->size, entry->actual_size);

			address += sizeof(__int32);
			entry = table.allocate<__int64>();//Single allocation by type

			Assert::IsNotNull(entry);
			Assert::IsNotNull(entry->ptr);

			Assert::AreEqual(entry->value, address);
			Assert::AreEqual(entry->size, sizeof(__int64));
			Assert::AreEqual(entry->size, entry->actual_size);

			address += sizeof(__int64);
			entry = table.allocate(2, sizeof(__int16));//Multiple allocations

			Assert::IsNotNull(entry);
			Assert::IsNotNull(entry->ptr);

			Assert::AreEqual(entry->value, address);
			Assert::AreEqual(entry->size, sizeof(__int16));
			Assert::AreEqual(entry->size, entry->actual_size);

			address += (sizeof(__int16) * 2);
			entry = table.allocate(0u);//Empty allocation

			Assert::IsNotNull(entry);
			Assert::IsNotNull(entry->ptr);

			Assert::AreEqual(entry->value, address);
			Assert::AreEqual(entry->size, 0ull);
			Assert::AreNotEqual(entry->size, entry->actual_size);

			Assert::AreEqual(table.head_list_.size(), size + 5u);
		}

		TEST_METHOD(reallocate){
			slang::address::table table;

			auto value = table.allocate(3u, sizeof(__int64))->value;
			auto entry = table.reallocate(value, sizeof(__int32));

			Assert::IsNotNull(entry);
			Assert::IsNotNull(entry->ptr);

			Assert::AreEqual(entry->value, value);
			Assert::AreEqual(entry->size, sizeof(__int32));
			Assert::AreEqual(entry->actual_size, sizeof(__int64));

			entry = table.reallocate(value, sizeof(__int64));

			Assert::IsNotNull(entry);
			Assert::IsNotNull(entry->ptr);

			Assert::AreEqual(entry->value, value);
			Assert::AreEqual(entry->size, sizeof(__int64));
			Assert::AreEqual(entry->actual_size, sizeof(__int64));

			entry = table.reallocate(value + (sizeof(__int64) * 2), sizeof(__int64) + sizeof(__int32));

			Assert::IsNotNull(entry);
			Assert::IsNotNull(entry->ptr);

			Assert::AreEqual(entry->value, value + (sizeof(__int64) * 2));
			Assert::AreEqual(entry->size, sizeof(__int64) + sizeof(__int32));
			Assert::AreEqual(entry->actual_size, sizeof(__int64) + sizeof(__int32));

			entry = table.reallocate(value + sizeof(__int64), sizeof(__int64) + sizeof(__int32));

			Assert::IsNotNull(entry);
			Assert::IsNotNull(entry->ptr);

			Assert::AreNotEqual(entry->value, value + sizeof(__int64));
			Assert::AreEqual(entry->size, sizeof(__int64) + sizeof(__int32));
			Assert::AreEqual(entry->actual_size, sizeof(__int64) + sizeof(__int32));
		}

		TEST_METHOD(find){
			slang::address::table table;

			auto value = table.allocate(3u, sizeof(__int64))->value;
			auto entry = table.find(value);

			Assert::IsNotNull(entry);
			Assert::IsNotNull(entry->ptr);

			Assert::AreEqual(entry->value, value);
			Assert::AreEqual(entry->size, sizeof(__int64));

			entry = table.find(value + sizeof(__int64));

			Assert::IsNotNull(entry);
			Assert::IsNotNull(entry->ptr);

			Assert::AreEqual(entry->value, value + sizeof(__int64));
			Assert::AreEqual(entry->size, sizeof(__int64));

			entry = table.find(value + (sizeof(__int64) * 2));

			Assert::IsNotNull(entry);
			Assert::IsNotNull(entry->ptr);

			Assert::AreEqual(entry->value, value + (sizeof(__int64) * 2));
			Assert::AreEqual(entry->size, sizeof(__int64));

			entry = table.find(value + (sizeof(__int64) * 3));
			Assert::IsNull(entry);

			entry = table.find(value + 2ull);
			Assert::IsNull(entry);

			entry = table.find(0ull);
			Assert::IsNull(entry);
		}

		TEST_METHOD(get_head){
			slang::address::table table;

			auto value = table.allocate(3u, sizeof(__int64))->value;
			auto entry = table.get_head(value);

			Assert::IsNotNull(entry);
			Assert::IsNotNull(entry->ptr);

			Assert::AreEqual(entry->value, value);
			Assert::AreEqual(entry->size, sizeof(__int64));

			entry = table.get_head(value + sizeof(__int64));

			Assert::IsNotNull(entry);
			Assert::IsNotNull(entry->ptr);

			Assert::AreEqual(entry->value, value + sizeof(__int64));
			Assert::AreEqual(entry->size, sizeof(__int64));

			entry = table.get_head(value + (sizeof(__int64) * 2));

			Assert::IsNotNull(entry);
			Assert::IsNotNull(entry->ptr);

			Assert::AreEqual(entry->value, value + (sizeof(__int64) * 2));
			Assert::AreEqual(entry->size, sizeof(__int64));

			entry = table.get_head(value + (sizeof(__int64) * 3));
			Assert::IsNull(entry);

			entry = table.get_head(value + 2ull);
			Assert::IsNotNull(entry);
			Assert::IsNotNull(entry->ptr);

			Assert::AreEqual(entry->value, value);
			Assert::AreEqual(entry->size, sizeof(__int64));

			entry = table.get_head(0ull);
			Assert::IsNull(entry);
		}

		TEST_METHOD(read){
			slang::address::table table;
			auto value = table.allocate(3u, sizeof(__int64))->value;

			table.write(value, 9ll);
			Assert::AreEqual(table.read<__int64>(value), 9ll);

			table.write(value, 18);
			Assert::AreEqual(table.read<__int64>(value), 18ll);
			Assert::AreEqual(table.read<__int32>(value), 18);
			Assert::AreEqual(table.read<__int16>(value), static_cast<__int16>(18));
		}

		TEST_METHOD(convert_numeric){
			slang::address::table table;
			auto value = table.allocate_scalar(108)->value;

			Assert::AreEqual(table.convert_numeric<__int32>(value, false), 108);
			Assert::AreEqual(table.convert_numeric<__int64>(value, false), 108ll);
			Assert::AreEqual(table.convert_numeric<float>(value, false), 108.0f);

			value = table.allocate_scalar(4.5l)->value;

			Assert::AreEqual(table.convert_numeric<__int32>(value, true), 4);
			Assert::AreEqual(table.convert_numeric<__int64>(value, true), 4ll);
			Assert::AreEqual(table.convert_numeric<float>(value, true), 4.5f);
		}
	};
}