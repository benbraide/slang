#pragma once

#ifndef SLANG_ENUM_TYPE_H
#define SLANG_ENUM_TYPE_H

#include <vector>
#include <climits>

#include "type_object.h"

#include "../common/output_writer_interface.h"
#include "../address/address_head.h"
#include "../storage/named_storage.h"

namespace slang{
	namespace type{
		class enum_type : public object, public storage::named{
		public:
			typedef storage::named storage_type;
			typedef address::head::uint64_type uint64_type;
			typedef common::output_writer_interface writer_type;

			enum_type(size_type count, const std::string &name, storage_type *parent);

			virtual ~enum_type();

			virtual driver_object_type *driver() const override;

			virtual const std::string &name() const override;

			virtual std::string print() const override;

			virtual size_type size() const override;

			virtual id_type id() const override;

			virtual bool is_enum() const override;

			virtual bool is_linear() const;

			virtual bool insert(const std::string &item);

			virtual bool print_item(uint64_type value, writer_type &out) const;

			virtual unsigned long long compute_value(size_type index) const;

		protected:
			size_type size_;
			size_type insert_count_;
			bool is_linear_;
			uint64_type address_;
		};
	}
}

#endif /* !SLANG_ENUM_TYPE_H */
