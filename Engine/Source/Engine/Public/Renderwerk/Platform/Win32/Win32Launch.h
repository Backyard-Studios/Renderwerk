#pragma once

#include <Windows.h>

#if RW_KIND_CONSOLE_APP
#	define PLATFORM_LAUNCH_HEADER int main(int ArgumentCount, char* Arguments[])
#else
#	define PLATFORM_LAUNCH_HEADER int WINAPI WinMain(HINSTANCE InstanceHandle, HINSTANCE PreviousInstanceHandle, LPSTR CommandLine, int ShowCommand)
#endif
