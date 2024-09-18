#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Platform/Thread.h"

class ENGINE_API FWin32Thread : public IThread
{
public:
	FWin32Thread(const FThreadFunction& InThreadFunction, const EThreadPriority& InPriority);
	~FWin32Thread() override;

	DEFINE_DEFAULT_COPY_AND_MOVE(FWin32Thread)

public:
	void Start() override;
	void Join() override;
	void ForceKill(bool bWaitForCompletion = false) override;

public:
	[[nodiscard]] void* GetNativeHandle() const override;

private:
	static int32 ConvertThreadPriority(const EThreadPriority& InPriority);

private:
	HANDLE ThreadHandle;
};
