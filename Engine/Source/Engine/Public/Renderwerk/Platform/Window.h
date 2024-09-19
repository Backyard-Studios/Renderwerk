#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Core/CoreTypes.h"
#include "Renderwerk/Memory/SharedPointer.h"

class IWindow;

struct ENGINE_API FWindowSettings
{
	uint32 Width = 1280;
	uint32 Height = 720;
	int32 PositionX = 200;
	int32 PositionY = 200;
	std::string Title = "Renderwerk";
	bool bIsVisibleAfterCreation = false;
	TSharedPointer<IWindow> ParentWindow = nullptr;
};

struct ENGINE_API FWindowState
{
	uint32 WindowWidth = 0;
	uint32 WindowHeight = 0;
	uint32 ClientWidth = 0;
	uint32 ClientHeight = 0;
	int32 PositionX = 0;
	int32 PositionY = 0;
	bool bIsMoving = false;
	bool bIsResizing = false;
	bool bIsVisible = false;
	bool bIsClosed = false;
	bool bIsDestroyed = false;
	std::string Title = "";
	TSharedPointer<IWindow> ParentWindow = nullptr;
};

/**
 * @brief Platform-independent window interface.
 */
class ENGINE_API IWindow
{
public:
	IWindow(const FWindowSettings& InWindowSettings)
		: IWindow(InWindowSettings, NewGuid())
	{
	}

	IWindow(const FWindowSettings& InWindowSettings, const FGuid& InGuid)
		: Settings(InWindowSettings), Guid(InGuid)
	{
		State.WindowWidth = InWindowSettings.Width;
		State.WindowHeight = InWindowSettings.Height;
		State.ClientWidth = InWindowSettings.Width;
		State.ClientHeight = InWindowSettings.Height;
		State.PositionX = InWindowSettings.PositionX;
		State.PositionY = InWindowSettings.PositionY;
		State.bIsVisible = InWindowSettings.bIsVisibleAfterCreation;
		State.Title = InWindowSettings.Title;
		State.ParentWindow = InWindowSettings.ParentWindow;
	}

	virtual ~IWindow() = default;

	DELETE_COPY_AND_MOVE(IWindow)

public:
	virtual void Show() = 0;
	virtual void Hide() = 0;
	virtual void Minimize() = 0;
	virtual void Maximize() = 0;
	virtual void Restore() = 0;
	virtual void Focus() = 0;
	virtual void Close() = 0;
	virtual void Destroy() = 0;

	virtual void SetPosition(int32 PositionX, int32 PositionY) = 0;
	virtual void SetSize(int32 Width, int32 Height) = 0;
	virtual void SetTitle(const std::string& Title) = 0;

	[[nodiscard]] virtual void* GetNativeHandle() const = 0;

public:
	[[nodiscard]] FWindowState GetWindowState() const { return State; }
	[[nodiscard]] FGuid GetGuid() const { return Guid; }

protected:
	FWindowSettings Settings;
	FGuid Guid;

	FWindowState State;
};
