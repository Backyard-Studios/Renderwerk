#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Platform/Window.h"

class ENGINE_API FWin32Window : public IWindow
{
public:
	FWin32Window(const FWindowSettings& InWindowSettings);
	FWin32Window(const FWindowSettings& InWindowSettings, const FGuid& InGuid);
	~FWin32Window() override;

	DELETE_COPY_AND_MOVE(FWin32Window)

public:
	void Show() override;
	void Hide() override;
	void Minimize() override;
	void Maximize() override;
	void Restore() override;
	void Focus() override;
	void Close() override;
	void Destroy() override;

	void SetPosition(int32 PositionX, int32 PositionY) override;
	void SetSize(int32 Width, int32 Height) override;
	void SetTitle(const std::string& Title) override;

	[[nodiscard]] void* GetNativeHandle() const override;

private:
	LRESULT WindowProcess(HWND InWindowHandle, UINT Message, WPARAM WParam, LPARAM LParam);

	void OnSizeMessage(LPARAM LParam);
	void OnMoveMessage(LPARAM LParam);
	void OnCloseMessage();
	void OnEnterSizeMoveMessage(WPARAM WParam);
	void OnExitSizeMoveMessage(WPARAM WParam);
	void OnShowWindowMessage(WPARAM WParam);
	void OnDestroyMessage();

private:
	HWND WindowHandle = nullptr;

	friend LRESULT CALLBACK WindowProcess(HWND WindowHandle, UINT Message, WPARAM WParam, LPARAM LParam);
};
