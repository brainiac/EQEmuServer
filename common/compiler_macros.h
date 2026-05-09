#pragma once

#if defined(_MSC_VER)
	#define PUSH_WARNINGS() __pragma(warning(push))
	#define POP_WARNINGS() __pragma(warning(pop))

	#define PUSH_DISABLE_DEPRECATED_WARNINGS()      \
		PUSH_WARNINGS()                             \
		__pragma(warning(disable:4996))
	#define POP_DISABLE_DEPRECATED_WARNINGS()       \
		POP_WARNINGS()
	#define MSVC_DISABLE_WARNING(warningNumber)     \
		__pragma(warning(disable:warningNumber))

	#define GCC_DISABLE_WARNING(warningName)

#elif defined(__GNUC__) || defined(__clang__)
	#define PRAGMA_WARNING_HELPER__(x) _Pragma(#x)

	#define PUSH_WARNINGS() _Pragma("GCC diagnostic push")
	#define POP_WARNINGS() _Pragma("GCC diagnostic pop")

	#define PUSH_DISABLE_DEPRECATED_WARNINGS()      \
		PUSH_WARNINGS()                             \
		PRAGMA_WARNING_HELPER__(GCC diagnostic ignored "-Wdeprecated-declarations")
	#define POP_DISABLE_DEPRECATED_WARNINGS()       \
		POP_WARNINGS()
	#define GCC_DISABLE_WARNING(warningName)        \
		PRAGMA_WARNING_HELPER__(GCC diagnostic ignored warningName)

	#define MSVC_DISABLE_WARNING(warningNumber)
#else
	#define PUSH_WARNINGS()
	#define POP_WARNINGS()
	#define PUSH_DISABLE_DEPRECATED_WARNINGS()
	#define POP_DISABLE_DEPRECATED_WARNINGS()
	#define GCC_DISABLE_WARNING(warningName)
	#define MSVC_DISABLE_WARNING(warningNumber)
#endif

#if defined(_MSC_VER) && !defined(__clang__)
	#define UNREACHABLE() __assume(0)
#else
	#define UNREACHABLE() __builtin_unreachable()
#endif
