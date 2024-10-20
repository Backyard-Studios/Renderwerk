#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/DataTypes/Types.h"
#include "Renderwerk/DataTypes/Delegates/MulticastDelegate.h"

DECLARE_MULTICAST_DELEGATE(WindowResized, uint32, uint32);
DECLARE_MULTICAST_DELEGATE(ClientAreaResized, uint32, uint32);
DECLARE_MULTICAST_DELEGATE(FocusChange, bool8);

enum class RENDERWERK_API EWindowStyle : uint8
{
	Windowed = 0,
	Borderless,
};

struct RENDERWERK_API FWindowState
{
	uint32 WindowWidth = 0;
	uint32 WindowHeight = 0;
	uint32 ClientWidth = 0;
	uint32 ClientHeight = 0;
	int32 PositionX = 0;
	int32 PositionY = 0;
	FString Title = TEXT("");
	bool8 bIsMoving = false;
	bool8 bIsResizing = false;
	bool8 bIsVisible = false;
	bool8 bIsFocused = false;
};

struct RENDERWERK_API FWindowDesc
{
	uint32 Width = 1280;
	uint32 Height = 720;
	EWindowStyle Style = EWindowStyle::Windowed;
	FString Title = TEXT("Renderwerk");
	bool8 bUseCustomPosition = false;
	int32 CustomPositionX = 0;
	int32 CustomPositionY = 0;
	bool8 bShowAfterCreation = true;
	TSharedPtr<class FWindow> ParentWindow = nullptr;
};

class RENDERWERK_API FWindow
{
public:
	FWindow(const WNDCLASSEX& WindowClass, const FWindowDesc& InDescription);
	~FWindow();

	DELETE_COPY_AND_MOVE(FWindow);

public:
	void Show() const;
	void Hide() const;
	void Minimize() const;
	void Maximize() const;
	void Restore() const;
	void Focus() const;
	void Close() const;
	void Destroy();

	void SetPosition(int32 PositionX, int32 PositionY) const;
	void SetSize(int32 Width, int32 Height) const;
	void SetTitle(const FString& Title);
	void AppendTitle(const FString& Title);

	NODISCARD bool8 IsValid() const;

public:
	NODISCARD HWND GetHandle() const { return WindowHandle; }

	NODISCARD FGuid GetId() const { return Id; }
	NODISCARD FWindowState GetState() const { return State; }
	NODISCARD bool8 IsClosed() const { return bIsClosed; }
	NODISCARD bool8 IsDestroyed() const { return bIsDestroyed; }

	NODISCARD FWindowResizedDelegate* GetWindowResizedDelegate() { return &OnWindowResized; }
	NODISCARD FClientAreaResizedDelegate* GetClientAreaResizedDelegate() { return &OnClientAreaResized; }
	NODISCARD FFocusChangeDelegate* GetFocusChangeDelegate() { return &OnFocusChange; }

private:
	LRESULT WindowProcess(HWND InWindowHandle, UINT Message, WPARAM WParam, LPARAM LParam);

	void OnSizeMessage(LPARAM LParam);
	void OnMoveMessage(LPARAM LParam);
	void OnEnterSizeMoveMessage(WPARAM WParam);
	void OnExitSizeMoveMessage(WPARAM WParam);
	void OnShowWindowMessage(WPARAM WParam);
	void OnSetFocusMessage();
	void OnKillFocusMessage();
	void OnCloseMessage();
	void OnDestroyMessage();

private:
	static uint32 GetStyleFromDescription(const FWindowDesc& Description);
	static uint32 GetExtendedStyleFromDescription(const FWindowDesc& Description);

private:
	FGuid Id;
	FWindowDesc Description;
	HWND WindowHandle;

	FWindowState State;
	bool8 bIsClosed = false;
	bool8 bIsDestroyed = false;

	FWindowResizedDelegate OnWindowResized;
	FClientAreaResizedDelegate OnClientAreaResized;
	FFocusChangeDelegate OnFocusChange;

	friend LRESULT CALLBACK WindowProcess(HWND WindowHandle, UINT Message, WPARAM WParam, LPARAM LParam);
};
