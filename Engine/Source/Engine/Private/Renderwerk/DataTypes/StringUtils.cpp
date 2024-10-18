#include "pch.h"

#include "Renderwerk/DataTypes/StringUtils.h"

#include <WinString.h>

FWideString FStringUtils::ConvertToWideString(const FAnsiChar* InString)
{
	const size64 NarrowLength = strlen(InString);
	const size64 WideLength = MultiByteToWideChar(CP_UTF8, 0, InString, static_cast<int32>(NarrowLength), nullptr, 0);
	FWideString Wide(WideLength, L'\0');
	MultiByteToWideChar(CP_UTF8, 0, InString, static_cast<int32>(NarrowLength), Wide.data(), static_cast<int32>(WideLength));
	Wide[WideLength] = L'\0';
	return Wide;
}

FWideString FStringUtils::ConvertToWideString(const FAnsiStringView& InString)
{
	return ConvertToWideString(InString.data());
}

FAnsiString FStringUtils::ConvertToAnsiString(const FWideChar* InString)
{
	const size64 WideLength = wcslen(InString);
	const size64 NarrowLength = WideCharToMultiByte(CP_UTF8, 0, InString, static_cast<int32>(WideLength), nullptr, 0, nullptr, nullptr);
	FAnsiString Narrow(NarrowLength, '\0');
	WideCharToMultiByte(CP_UTF8, 0, InString, static_cast<int32>(WideLength), Narrow.data(), static_cast<int32>(NarrowLength), nullptr, nullptr);
	Narrow[NarrowLength] = '\0';
	return Narrow;
}

FAnsiString FStringUtils::ConvertToAnsiString(const FWideStringView& InString)
{
	return ConvertToAnsiString(InString.data());
}
