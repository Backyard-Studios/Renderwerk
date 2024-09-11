#pragma once

#if RW_COMPILE_ENGINE_API
#	define ENGINE_API __declspec(dllexport)
#else
#	define ENGINE_API __declspec(dllimport)
#endif

ENGINE_API int TestFunction(int InValue);
