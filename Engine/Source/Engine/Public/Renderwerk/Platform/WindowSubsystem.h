#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/DataTypes/Delegates/MulticastDelegate.h"
#include "Renderwerk/Engine/Subsystem.h"
#include "Renderwerk/Logging/LogCategory.h"
#include "Renderwerk/Platform/Guid.h"
#include "Renderwerk/Platform/Window.h"

DECLARE_LOG_CATEGORY(LogWindowSubsystem, Trace);

class RENDERWERK_API FWindowSubsystem : public ISubsystem
{
public:
	FWindowSubsystem();
	~FWindowSubsystem() override;

	DELETE_COPY_AND_MOVE(FWindowSubsystem);

public:
	NODISCARD bool8 IsValidWindowId(const FGuid& Id) const;

	NODISCARD TSharedPtr<FWindow> NewWindow(const FWindowDesc& Description);

	void DeleteWindow(const FGuid& Id);
	void DeleteWindow(const TSharedPtr<FWindow>& Window);

	NODISCARD TSharedPtr<FWindow> GetWindow(const FGuid& Id) const;

	void SetMainWindowId(const FGuid& Id);
	void SetMainWindowId(const TSharedPtr<FWindow>& Window);

public:
	NODISCARD WNDCLASSEX GetWindowClass() const { return WindowClass; }
	NODISCARD FGuid GetMainWindowId() const { return MainWindowId; }

private:
	void Initialize() override;
	void Shutdown() override;

private:
	void OnTick(float64 DeltaTime);

private:
	WNDCLASSEX WindowClass;

	FDelegateHandle OnTickHandle;

	FGuid MainWindowId;
	TMap<FGuid, TSharedPtr<FWindow>> Windows;

	TQueue<FGuid> WindowsToDelete;
};
