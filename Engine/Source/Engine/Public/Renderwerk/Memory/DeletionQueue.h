#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
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


// Convenience macros for adding objects to the deletion queue
#define DQ_ADD_CQ(Queue, Object) Queue.Add([=]() { Object.reset(); RW_LOG_TRACE("Deleting {}", #Object); })
#define DQ_ADD_CUSTOM_CQ(Queue, Object, CustomDeletion) Queue.Add([=]() { if(Object) CustomDeletion; Object.reset(); RW_LOG_TRACE("Deleting {}", #Object); })
#define DQ_ADD_CUSTOM_PREDICATE_CQ(Queue, Object, Predicate, CustomDeletion) Queue.Add([=]() { if(Object && Predicate) CustomDeletion; Object.reset(); RW_LOG_TRACE("Deleting {}", #Object); })

// Default deletion queue name
#define DQ_ADD(Object) DQ_ADD_CQ(DeletionQueue, Object)
#define DQ_ADD_CUSTOM(Object, CustomDeletion) DQ_ADD_CUSTOM_CQ(DeletionQueue, Object, CustomDeletion)
#define DQ_ADD_CUSTOM_PREDICATE(Object, Predicate, CustomDeletion) DQ_ADD_CUSTOM_PREDICATE_CQ(DeletionQueue, Object, Predicate, CustomDeletion)
