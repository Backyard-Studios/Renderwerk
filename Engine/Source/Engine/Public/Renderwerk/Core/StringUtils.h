#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"

INLINE ENGINE_API std::string ToNarrow(const wchar_t* InWide)
{
	const size_t WideLength = wcslen(InWide);
	const size_t NarrowLength = WideCharToMultiByte(CP_UTF8, 0, InWide, static_cast<int32>(WideLength), nullptr, 0, nullptr, nullptr);
	std::string Narrow(NarrowLength, '\0');
	WideCharToMultiByte(CP_UTF8, 0, InWide, static_cast<int32>(WideLength), Narrow.data(), static_cast<int32>(NarrowLength), nullptr, nullptr);
	Narrow[NarrowLength] = '\0';
	return Narrow;
}

INLINE ENGINE_API std::wstring ToWide(const char* InNarrow)
{
	const size_t NarrowLength = strlen(InNarrow);
	const size_t WideLength = MultiByteToWideChar(CP_UTF8, 0, InNarrow, static_cast<int32>(NarrowLength), nullptr, 0);
	std::wstring Wide(WideLength, L'\0');
	MultiByteToWideChar(CP_UTF8, 0, InNarrow, static_cast<int32>(NarrowLength), Wide.data(), static_cast<int32>(WideLength));
	Wide[WideLength] = L'\0';
	return Wide;
}
