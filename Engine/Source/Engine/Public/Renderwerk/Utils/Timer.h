#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

class RENDERWERK_API FTimer
{
public:
	FTimer();
	~FTimer();

	DEFINE_DEFAULT_COPY_AND_MOVE(FTimer);

public:
	void Start();
	void Stop();

	NODISCARD float64 GetElapsedTime() const;

private:
	LARGE_INTEGER Frequency;

	LARGE_INTEGER StartTime = {};
	LARGE_INTEGER StopTime = {};

	float64 ElapsedTime = 0.0;
};
