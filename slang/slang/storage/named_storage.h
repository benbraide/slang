#pragma once

#ifndef SLANG_NAMED_STORAGE_H
#define SLANG_NAMED_STORAGE_H

#include "storage_object.h"

namespace slang{
	namespace storage{
		class named : public object{
		public:
			explicit named(const std::string &name);

			named(const std::string &name, object &parent);

			virtual ~named();

			virtual object *match(const std::string &name) override;

			virtual const std::string &name() const;

			virtual std::string print() const;

		protected:
			std::string name_;
		};
	}
}

#endif /* !SLANG_NAMED_STORAGE_H */
