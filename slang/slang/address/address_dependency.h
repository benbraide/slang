#pragma once

#ifndef SLANG_ADDRESS_DEPENDENCY_H
#define SLANG_ADDRESS_DEPENDENCY_H

namespace slang{
	namespace address{
		class dependency{
		public:
			virtual ~dependency() = default;

			virtual void no_address_deallocation() = 0;
		};
	}
}

#endif /* !SLANG_ADDRESS_DEPENDENCY_H */
