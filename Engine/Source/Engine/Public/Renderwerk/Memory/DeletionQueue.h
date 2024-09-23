#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Core/CoreTypes.h"
#include "Renderwerk/Debug/Profiling.h"

struct ENGINE_API FDeletionQueue
{
public:
	using FDeleter = TFunction<void()>;

public:
	INLINE void Add(FDeleter&& Deleter)
	{
		DeleteQueue.push_back(std::move(Deleter));
	}

	INLINE void Flush()
	{
		RW_PROFILING_MARK_FUNCTION();

		while (!DeleteQueue.empty())
		{
			FDeleter Deleter = DeleteQueue.back();
			Deleter();
			DeleteQueue.pop_back();
		}
	}

private:
	TDeque<FDeleter> DeleteQueue;
};
