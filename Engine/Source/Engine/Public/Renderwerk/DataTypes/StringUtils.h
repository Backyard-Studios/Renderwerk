﻿#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/DataTypes/String.h"

class RENDERWERK_API FStringUtils
{
public:
	static FWideString ConvertToWideString(const FAnsiChar* InString);
	static FWideString ConvertToWideString(const FAnsiStringView& InString);

	static FAnsiString ConvertToAnsiString(const FWideChar* InString);
	static FAnsiString ConvertToAnsiString(const FWideStringView& InString);

private:
};
