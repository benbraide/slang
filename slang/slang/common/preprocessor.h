#pragma once

#ifndef SLANG_PREPROCESSOR_H
#define SLANG_PREPROCESSOR_H

#define SLANG_MAKE_OPERATORS_(x, t)\
	inline constexpr x operator |(x l, x r){\
		return (x)((t)(l) | (t)(r));\
	}\
\
	inline constexpr x operator &(x l, x r){\
		return (x)((t)(l) & (t)(r));\
	}\
\
	inline constexpr x operator ~(x r){\
		return (x)(~(t)(r));\
	}\
\
	inline x operator |=(x &l, x r){\
		return (l = (x)((t)(l) | (t)(r)));\
	}\
\
	inline x operator &=(x &l, x r){\
		return (l = (x)((t)(l) & (t)(r)));\
	}\
\
	inline x operator <<(x &l, t r){\
		return (x)((t)(l) << (r));\
	}\
\
	inline x operator >>(x &l, t r){\
		return (x)((t)(l) >> (r));\
	}

#define SLANG_MAKE_OPERATORS(x) SLANG_MAKE_OPERATORS_(x, unsigned int)

#define SLANG_MAKE_OPERATORS_EX(x) SLANG_MAKE_OPERATORS_(x, unsigned long long)

#define SLANG_SET(source, target) ((source) |= (target))
#define SLANG_SET_V(source, target) ((source) | (target))

#define SLANG_REMOVE(source, target) ((source) &= ~(target))
#define SLANG_REMOVE_V(source, target) ((source) & ~(target))

#define SLANG_IS(source, target) (((source) & (target)) == (target))
#define SLANG_IS_ANY(source, target) (((source) & (target)) != static_cast<decltype(target)>(0))

#define SLANG_CONVERT_ENUM_(x, t, v) (x)(t)(v)
#define SLANG_CONVERT_ENUM(x, v) SLANG_CONVERT_ENUM_(x, int, v)

#define SLANG_EQUAL_ENUMS_(x, t, l, r) ((l) == (x)(t)(r))
#define SLANG_EQUAL_ENUMS(x, l, r) SLANG_EQUAL_ENUMS_(x, int, l, r)

#define SLANG_INCREMENT_ENUM_(x, t, v) (v) = (x)((t)(v) + (t)1)
#define SLANG_INCREMENT_ENUM(x, v) SLANG_INCREMENT_ENUM_(x, int, v)

#define SLANG_DECREMENT_ENUM_(x, t, v) (v) = (x)((t)(v) - (t)1)
#define SLANG_DECREMENT_ENUM(x, v) SLANG_DECREMENT_ENUM_(x, int, v)

#endif /* !SLANG_PREPROCESSOR_H */
