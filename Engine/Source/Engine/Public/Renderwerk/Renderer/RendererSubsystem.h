#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Engine/Subsystem.h"
#include "Renderwerk/Logging/LogCategory.h"
#include "Renderwerk/RHI/RHICommon.h"

class FWindow;

DECLARE_LOG_CATEGORY(LogRenderer, Trace);

class RENDERWERK_API FRendererSubsystem : public ISubsystem
{
public:
	FRendererSubsystem();
	~FRendererSubsystem() override;

	DELETE_COPY_AND_MOVE(FRendererSubsystem);

private:
	void Initialize() override;
	void Shutdown() override;

	void OnTick(float64 DeltaTime) const;

private:
	FDelegateHandle OnTickHandle;

	TSharedPtr<FWindow> Window;

	TLocalPtr<FRHIBackend> Backend;
};
