#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"

#if defined(__clang__)
#	define RW_COMPILER_CLANG FORWARD(1)
#	define RW_COMPILER_VERSION __clang_version__
#	define RW_COMPILER_NAME "Clang"

#	define RW_COMPILER_GCC FORWARD(0)
#	define RW_COMPILER_MSVC FORWARD(0)
#elif defined(__GNUC__)
#	define RW_COMPILER_GCC FORWARD(1)
#	define RW_COMPILER_VERSION __GNUC__
#	define RW_COMPILER_NAME "GCC"

#	define RW_COMPILER_CLANG FORWARD(0)
#	define RW_COMPILER_MSVC FORWARD(0)
#elif defined(_MSC_VER)
#	define RW_COMPILER_MSVC FORWARD(1)
#	define RW_COMPILER_VERSION _MSC_VER
#	define RW_COMPILER_NAME "MSVC"

#	define RW_COMPILER_CLANG FORWARD(0)
#	define RW_COMPILER_GCC FORWARD(0)
#else
#	error "Unsupported compiler"
#endif

#if RW_COMPILER_MSVC
#	define CPP_VERSION _MSVC_LANG
#else
#	define CPP_VERSION __cplusplus
#endif

#if CPP_VERSION == 201103L
#	define RW_CPP11 FORWARD(1)

#	define RW_CPP14 FORWARD(0)
#	define RW_CPP17 FORWARD(0)
#	define RW_CPP20 FORWARD(0)
#elif CPP_VERSION == 201402L
#	define RW_CPP14 FORWARD(1)

#	define RW_CPP11 FORWARD(0)
#	define RW_CPP17 FORWARD(0)
#	define RW_CPP20 FORWARD(0)
#elif CPP_VERSION == 201703L
#	define RW_CPP17 FORWARD(1)

#	define RW_CPP11 FORWARD(0)
#	define RW_CPP14 FORWARD(0)
#	define RW_CPP20 FORWARD(0)
#elif CPP_VERSION == 202002L
#	define RW_CPP20 FORWARD(1)

#	define RW_CPP11 FORWARD(0)
#	define RW_CPP14 FORWARD(0)
#	define RW_CPP17 FORWARD(0)
#else
#	error "Unsupported C++ version"
#endif

#if defined(__cpp_attributes) && defined(__has_cpp_attribute)
#	if __has_cpp_attribute(deprecated)
#		define DEPRECATED [[deprecated]]
#		define DEPRECATED_MSG(msg) [[deprecated(msg)]]
#	else
#		define DEPRECATED
#		define DEPRECATED_MSG(msg)
#	endif
#	if __has_cpp_attribute(nodiscard)
#		define NODISCARD [[nodiscard]]
#		if RW_CPP20
#			define NODISCARD_MSG(msg) [[nodiscard(msg)]]
#		endif
#	else
#		define NODISCARD
#		define NODISCARD_MSG(msg)
#	endif
#	if __has_cpp_attribute(likely)
#		define LIKELY [[likely]]
#	else
#		define LIKELY
#	endif
#	if __has_cpp_attribute(unlikely)
#		define UNLIKELY [[unlikely]]
#	else
#		define UNLIKELY
#	endif
#	if __has_cpp_attribute(maybe_unused)
#		define MAYBE_UNUSED [[maybe_unused]]
#	else
#		define MAYBE_UNUSED
#	endif
#else
#	define DEPRECATED
#	define DEPRECATED_MSG(msg)
#	define NODISCARD
#	define NODISCARD_MSG(msg)
#	define LIKELY
#	define UNLIKELY
#	define MAYBE_UNUSED
#endif

#ifdef __cpp_static_assert
#	define STATIC_ASSERT static_assert
#else
#	define STATIC_ASSERT
#endif

#if RW_COMPILER_MSVC
#	define FORCEINLINE __forceinline
#	define NOINLINE __declspec(noinline)
#	define INLINE __inline
#	define DLL_EXPORT __declspec(dllexport)
#	define DLL_IMPORT __declspec(dllimport)
#elif RW_COMPILER_GCC || RW_COMPILER_CLANG
#	define FORCEINLINE __attribute__((always_inline)) inline
#	define NOINLINE __attribute__((noinline))
#	define INLINE inline
#	define DLL_EXPORT __attribute__((visibility("default")))
#	define DLL_IMPORT
#endif
