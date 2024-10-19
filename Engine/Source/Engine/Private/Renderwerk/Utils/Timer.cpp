#include "pch.h"

#include "Renderwerk/Utils/Timer.h"

FTimer::FTimer()
{
	ASSERTM(QueryPerformanceFrequency(&Frequency), "Failed to query performance frequency")
	ASSERTM(QueryPerformanceCounter(&StartTime), "Failed to query performance counter")
}

FTimer::~FTimer() = default;

void FTimer::Start()
{
	ASSERTM(QueryPerformanceCounter(&StartTime), "Failed to query performance counter")
}

void FTimer::Stop()
{
	ASSERTM(QueryPerformanceCounter(&StopTime), "Failed to query performance counter")
	ElapsedTime = static_cast<float64>(StopTime.QuadPart - StartTime.QuadPart) / Frequency.QuadPart;
}

float64 FTimer::GetElapsedTime() const
{
	return ElapsedTime;
}
