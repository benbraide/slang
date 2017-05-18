#include "address/address_table.h"

int main(){
	auto i32 = static_cast<__int32>(9);

	auto i32c = reinterpret_cast<char *>(&i32);
	auto i64c = new char[sizeof(__int64)];

	for (auto i = 0u; i < sizeof(__int64); ++i){
		if (i < sizeof(__int32))
			*(i64c + i) = *(i32c + i);
		else
			*(i64c + i) = '\0';
	}

	auto i64 = *reinterpret_cast<__int64 *>(i64c);
	auto fv = *reinterpret_cast<long double *>(i64c);
	delete[] i64c;

	return 0;
}
