#pragma once

#ifndef SLANG_NUMERIC_DRIVER_H
#define SLANG_NUMERIC_DRIVER_H

#include "driver_object.h"

#include "../address/address_table.h"
#include "../storage/temp_storage.h"

namespace slang{
	namespace driver{
		class numeric : public object{
		public:
			using object::echo;
			using object::convert;

			virtual ~numeric();

			virtual entry_type *cast(entry_type &entry, type::object &type, cast_type options = cast_type::nil) override;

			virtual std::string to_string(entry_type &entry) override;

			virtual std::wstring to_wstring(entry_type &entry) override;

		protected:
			virtual entry_type *evaluate_(entry_type &entry, binary_info_type &info, entry_type &operand) override;

			virtual entry_type *evaluate_(entry_type &entry, unary_info_type &info) override;

			virtual entry_type *assign_(entry_type &entry, entry_type &value) override;

			virtual void convert_(entry_type &entry, type_id_type id, char *buffer) override;

			virtual void echo_(entry_type &entry, writer_type &out, bool no_throw) override;

			virtual entry_type *cast_(entry_type &entry, type_id_type id);

			virtual entry_type *static_cast_(entry_type &entry, type::object &type, cast_type options);

			virtual entry_type *reinterpret_cast_(entry_type &entry, type::object &type, cast_type options);

			template <typename value_type>
			entry_type *evaluate_unary_(entry_type &entry, unary_info_type &info){
				if (info.is_left){
					switch (info.id){
					case operator_id_type::minus:
						return get_temp_storage()->add(read_<value_type>(entry));
					default:
						break;
					}
				}

				return object::evaluate_(entry, info);
			}

			template <typename value_type>
			entry_type *evaluate_signed_(entry_type &entry, unary_info_type &info){
				if (info.is_left){
					switch (info.id){
					case operator_id_type::minus:
						return get_temp_storage()->add(static_cast<value_type>(-read_<value_type>(entry)));
					default:
						break;
					}
				}

				return evaluate_unary_<value_type>(entry, info);
			}

			template <typename value_type>
			entry_type *evaluate_signed_integral_(entry_type &entry, unary_info_type &info){
				auto value = evaluate_integral_<value_type>(entry, info);
				if (value != nullptr || has_error())
					return value;

				return evaluate_signed_<value_type>(entry, info);
			}

			template <typename value_type>
			entry_type *evaluate_unsigned_integral_(entry_type &entry, unary_info_type &info){
				auto value = evaluate_integral_<value_type>(entry, info);
				if (value != nullptr || has_error())
					return value;

				return evaluate_unary_<value_type>(entry, info);
			}

			template <typename value_type>
			entry_type *evaluate_integral_(entry_type &entry, unary_info_type &info){
				switch (info.id){
				case operator_id_type::bitwise_inverse:
					if (info.is_left)
						return object::evaluate_(entry, info);
					return get_temp_storage()->add(static_cast<value_type>(~read_<value_type>(entry)));
				case operator_id_type::decrement:
					return evaluate_increment_<value_type>(entry, info, false, info.is_left);
				case operator_id_type::increment:
					return evaluate_increment_<value_type>(entry, info, true, info.is_left);
				default:
					break;
				}

				return nullptr;
			}

			template <typename value_type>
			entry_type *evaluate_increment_(entry_type &entry, unary_info_type &info, bool increment, bool lval){
				if (!entry.is_lval())
					return common::env::error.set_and_return<nullptr_t>("Operator requires an lvalue.", true);

				if (entry.is_const())
					return common::env::error.set_and_return<nullptr_t>("Cannot modify a constant object.", true);

				auto value = read_<value_type>(entry), previous_value = value;
				if (increment)
					++value;
				else//Decrement
					--value;

				get_address_table().write(entry.address_value(), value);
				return lval ? &entry : get_temp_storage()->add(previous_value);
			}

			template <typename value_type>
			entry_type *evaluate_integral_(entry_type &entry, binary_info_type &info, entry_type &operand){
				auto left = convert_<value_type>(entry), right = convert_<value_type>(operand);
				switch (info.id){
				case operator_id_type::compound_modulus:
					if (right == static_cast<value_type>(0))
						return write_nan_(entry, true);
					return write_(entry, true, left % right);
				case operator_id_type::modulus:
					if (right == static_cast<value_type>(0))
						return write_nan_(entry, false);
					return write_(entry, false, left % right);
				case operator_id_type::compound_left_shift:
					return write_(entry, true, left << right);
				case operator_id_type::left_shift:
					return write_(entry, false, left << right);
				case operator_id_type::compound_right_shift:
					return write_(entry, true, left >> right);
				case operator_id_type::right_shift:
					return write_(entry, false, left >> right);
				case operator_id_type::compound_bitwise_and:
					return write_(entry, true, left & right);
				case operator_id_type::bitwise_and:
					return write_(entry, false, left & right);
				case operator_id_type::compound_bitwise_or:
					return write_(entry, true, left | right);
				case operator_id_type::bitwise_or:
					return write_(entry, false, left | right);
				case operator_id_type::compound_bitwise_xor:
					return write_(entry, true, left ^ right);
				case operator_id_type::bitwise_xor:
					return write_(entry, false, left ^ right);
				default:
					break;
				}

				return evaluate_binary_(entry, info, operand, left, right);
			}

			template <typename value_type>
			entry_type *evaluate_binary_(entry_type &entry, binary_info_type &info, entry_type &operand){
				auto left = convert_<value_type>(entry), right = convert_<value_type>(operand);
				return evaluate_binary_(entry, info, operand, left, right);
			}

			template <typename value_type>
			entry_type *evaluate_binary_(entry_type &entry, binary_info_type &info, entry_type &operand,
				value_type left, value_type right){
				switch (info.id){
				case operator_id_type::compound_plus:
					return write_(entry, true, left + right);
				case operator_id_type::plus:
					return write_(entry, false, left + right);
				case operator_id_type::compound_minus:
					return write_(entry, true, left - right);
				case operator_id_type::minus:
					return write_(entry, false, left - right);
				case operator_id_type::compound_times:
					return write_(entry, true, left * right);
				case operator_id_type::times:
					return write_(entry, false, left * right);
				case operator_id_type::compound_divide:
					if (right == static_cast<value_type>(0))
						return write_nan_(entry, true);
					return write_(entry, true, left / right);
				case operator_id_type::divide:
					if (right == static_cast<value_type>(0))
						return write_nan_(entry, false);
					return write_(entry, false, left / right);
				case operator_id_type::less:
					return get_temp_storage()->add(left < right);
				case operator_id_type::less_or_equal:
					return get_temp_storage()->add(left <= right);
				case operator_id_type::equality:
					return get_temp_storage()->add(left == right);
				case operator_id_type::inverse_equality:
					return get_temp_storage()->add(left != right);
				case operator_id_type::more_or_equal:
					return get_temp_storage()->add(left >= right);
				case operator_id_type::more:
					return get_temp_storage()->add(left > right);
				default:
					break;
				}

				return object::evaluate_(entry, info, operand);
			}

			template <typename value_type>
			entry_type *write_(entry_type &entry, bool assign, value_type value){
				if (assign){
					entry.remove_attributes(attribute_type::nan_);
					get_address_table().write(entry.address_value(), value);
					return &entry;
				}

				return get_temp_storage()->add(value);
			}

			entry_type *write_nan_(entry_type &entry, bool assign);

			virtual std::string prefix_(entry_type &entry);

			virtual std::string suffix_(entry_type &entry);

			template <typename value_type>
			value_type read_(entry_type &entry){
				return get_address_table().read<value_type>(entry.address_value());
			}

			template <typename value_type>
			value_type convert_(entry_type &entry){
				return get_address_table().convert_numeric<value_type>(entry.address_value(), type_of(entry)->is_floating_point());
			}
		};
	}
}

#endif /* !SLANG_NUMERIC_DRIVER_H */
