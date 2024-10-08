﻿#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Memory/SmartPointers.h"

class FWindow;

struct ENGINE_API FWindowSettings
{
	uint32 Width = 1280;
	uint32 Height = 720;
	int32 PositionX = 200;
	int32 PositionY = 200;
	std::string Title = RW_ENGINE_NAME " " RW_ENGINE_FULL_VERSION;
	bool bIsVisibleAfterCreation = false;
	TSharedPtr<FWindow> ParentWindow = nullptr;
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
	TSharedPtr<FWindow> ParentWindow = nullptr;
};

class ENGINE_API FWindow
{
public:
	FWindow(const FWindowSettings& InWindowSettings);
	FWindow(const FWindowSettings& InWindowSettings, const FGuid& InGuid);
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
	void Destroy() const;

	void SetPosition(int32 PositionX, int32 PositionY) const;
	void SetSize(int32 Width, int32 Height) const;
	void SetTitle(const std::string& Title);
	void AppendTitle(const std::string& Title);

	void ResetResizeFlag();

public:
	[[nodiscard]] HWND GetHandle() const { return WindowHandle; }

	[[nodiscard]] FWindowState GetWindowState() const { return State; }
	[[nodiscard]] FGuid GetGuid() const { return Guid; }

	[[nodiscard]] bool8 DidResize() const { return bDidResize; }

private:
	LRESULT WindowProcess(HWND InWindowHandle, UINT Message, WPARAM WParam, LPARAM LParam);

	void OnSizeMessage(LPARAM LParam);
	void OnMoveMessage(LPARAM LParam);
	void OnCloseMessage();
	void OnEnterSizeMoveMessage(WPARAM WParam);
	void OnExitSizeMoveMessage(WPARAM WParam);
	void OnShowWindowMessage(WPARAM WParam);
	void OnDestroyMessage();

protected:
	FWindowSettings Settings;
	FGuid Guid;

	HWND WindowHandle = nullptr;
	FWindowState State;
	bool8 bDidResize = false;

	friend LRESULT CALLBACK WindowProcess(HWND WindowHandle, UINT Message, WPARAM WParam, LPARAM LParam);
};
