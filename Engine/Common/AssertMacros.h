#pragma once
#include <cstdlib>
#include "LogMacros.h"

#define BREAK __debugbreak();

#define ALWAYS_ASSERT(expression, ...) \
	if (!(expression)) \
	{ \
		LOG(#expression);\
		BREAK; \
		std::abort(); \
	} else {}

#ifdef NDEBUG
#define DEV_ASSERT(...)
#else
#define DEV_ASSERT(expression, ...) ALWAYS_ASSERT(expression, __VA_ARGS__);
#endif
