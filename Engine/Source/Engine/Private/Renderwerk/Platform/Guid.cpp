#include "pch.h"

#include "Renderwerk/Platform/Guid.h"

bool IsValidHexChar(const char Char)
{
	if (Char > 47 && Char < 58)
		return true;
	if (Char > 96 && Char < 103)
		return true;
	if (Char > 64 && Char < 71)
		return true;
	return false;
}

uint8 HexDigitToChar(const char Char)
{
	if (Char > 47 && Char < 58)
		return Char - 48;
	if (Char > 96 && Char < 103)
		return Char - 87;
	if (Char > 64 && Char < 71)
		return Char - 55;
	return 0;
}

uint8 HexPairToChar(const char First, const char Second)
{
	return HexDigitToChar(First) * 16 + HexDigitToChar(Second);
}

FGuid::FGuid()
{
	Data.resize(16);
	std::ranges::fill(Data, static_cast<uint8>(0));
}

FGuid::FGuid(const TVector<uint8_t>& InData)
	: Data(InData)
{
}

FGuid::FGuid(const std::string& InString)
{
	Data.resize(16);
	char FirstChar = '\0';

	bool bLookingForFirst = true;
	byte NextByte = 0;

	for (const char& Char : InString)
	{
		if (Char == '-')
			continue;
		if (NextByte >= 16 || !IsValidHexChar(Char))
		{
			std::ranges::fill(Data, static_cast<uint8>(0));
			return;
		}

		if (bLookingForFirst)
		{
			FirstChar = Char;
			bLookingForFirst = false;
		}
		else
		{
			char SecondChar = Char;
			Data[NextByte] = HexPairToChar(FirstChar, SecondChar);
			NextByte++;
			bLookingForFirst = true;
		}
	}

	if (NextByte < 16)
	{
		std::ranges::fill(Data, static_cast<uint8>(0));
		return;
	}
}

bool FGuid::IsValid() const
{
	return Data != TVector<uint8_t>(16, 0);
}

std::string FGuid::ToString() const
{
	char First[10], Second[6], Third[6], Fourth[6], Fifth[14];
	int32 Written = 0;
	Written += sprintf_s(First, "%02x%02x%02x%02x", Data[0], Data[1], Data[2], Data[3]);
	Written += sprintf_s(Second, "%02x%02x", Data[4], Data[5]);
	Written += sprintf_s(Third, "%02x%02x", Data[6], Data[7]);
	Written += sprintf_s(Fourth, "%02x%02x", Data[8], Data[9]);
	Written += sprintf_s(Fifth, "%02x%02x%02x%02x%02x%02x", Data[10], Data[11], Data[12], Data[13], Data[14], Data[15]);
	assert(Written == 32);
	return std::string(First) + "-" + std::string(Second) + "-" + std::string(Third) + "-" + std::string(Fourth) + "-" + std::string(Fifth);
}

bool FGuid::operator==(const FGuid& Other) const
{
	return Data == Other.Data;
}

bool FGuid::operator!=(const FGuid& Other) const
{
	return Data != Other.Data;
}

bool FGuid::operator<(const FGuid& Other) const
{
	return Data < Other.Data;
}

bool FGuid::operator>(const FGuid& Other) const
{
	return Data > Other.Data;
}

FGuid::operator std::string() const
{
	return ToString();
}
