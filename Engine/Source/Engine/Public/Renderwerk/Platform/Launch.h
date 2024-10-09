#pragma once

#include "Renderwerk/Application/Application.h"
#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Memory/SmartPointers.h"

#if RW_KIND_CONSOLE_APP
#	define PLATFORM_LAUNCH_HEADER int main(int ArgumentCount, char* Arguments[])
#else
#	define PLATFORM_LAUNCH_HEADER int WINAPI WinMain(HINSTANCE InstanceHandle, HINSTANCE PreviousInstanceHandle, LPSTR CommandLine, int ShowCommand)
#endif


ENGINE_API int32 LaunchRenderwerk(const TSharedPtr<IApplication>& Application);

#define LAUNCH_RENDERWERK(Application, ...) \
	extern "C" { _declspec(dllexport) extern const UINT D3D12SDKVersion = 614;} \
	extern "C" { _declspec(dllexport) extern const char* D3D12SDKPath = ".\\D3D12\\"; } \
	 \
	PLATFORM_LAUNCH_HEADER \
	{ \
		return LaunchRenderwerk(MakeShared<Application>(__VA_ARGS__)); \
	}
