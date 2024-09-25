#include "pch.h"

#include "Renderwerk/Platform/Window.h"

FWindow::FWindow(const FWindowSettings& InWindowSettings)
	: FWindow(InWindowSettings, NewGuid())
{
}

FWindow::FWindow(const FWindowSettings& InWindowSettings, const FGuid& InGuid)
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

	uint32 WindowStyle = WS_OVERLAPPEDWINDOW;
	uint32 WindowExStyle = WS_EX_APPWINDOW;

	if (Settings.bIsVisibleAfterCreation)
		WindowStyle |= WS_VISIBLE;

	int32 PositionX = Settings.PositionX;
	int32 PositionY = Settings.PositionY;

	HWND ParentHandle = nullptr;
	if (Settings.ParentWindow)
		ParentHandle = Settings.ParentWindow->GetHandle();

	WindowHandle = CreateWindowEx(WindowExStyle,
	                              FPlatform::GetWindowClass().lpszClassName,
	                              ToWide(Settings.Title.c_str()).c_str(),
	                              WindowStyle,
	                              PositionX, PositionY,
	                              static_cast<int32>(Settings.Width), static_cast<int32>(Settings.Height),
	                              ParentHandle,
	                              nullptr,
	                              FPlatform::GetWindowClass().hInstance,
	                              this);
}

FWindow::~FWindow()
{
	if (WindowHandle && IsWindow(WindowHandle))
		DestroyWindow(WindowHandle);
}

void FWindow::Show() const
{
	ShowWindow(WindowHandle, SW_SHOW);
	UpdateWindow(WindowHandle);
}

void FWindow::Hide() const
{
	ShowWindow(WindowHandle, SW_HIDE);
	UpdateWindow(WindowHandle);
}

void FWindow::Minimize() const
{
	ShowWindow(WindowHandle, SW_MINIMIZE);
	UpdateWindow(WindowHandle);
}

void FWindow::Maximize() const
{
	ShowWindow(WindowHandle, SW_MAXIMIZE);
	UpdateWindow(WindowHandle);
}

void FWindow::Restore() const
{
	ShowWindow(WindowHandle, SW_RESTORE);
}

void FWindow::Focus() const
{
	SetFocus(WindowHandle);
	SetForegroundWindow(WindowHandle);
}

void FWindow::Close() const
{
	CloseWindow(WindowHandle);
}

void FWindow::Destroy() const
{
	DestroyWindow(WindowHandle);
}

void FWindow::SetPosition(const int32 PositionX, const int32 PositionY) const
{
	SetWindowPos(WindowHandle, nullptr, PositionX, PositionY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

void FWindow::SetSize(const int32 Width, const int32 Height) const
{
	SetWindowPos(WindowHandle, nullptr, 0, 0, Width, Height, SWP_NOMOVE | SWP_NOZORDER);
}

void FWindow::SetTitle(const std::string& Title)
{
	State.Title = Title;
	SetWindowText(WindowHandle, ToWide(Title.c_str()).c_str());
}

void FWindow::AppendTitle(const std::string& Title)
{
	SetTitle(State.Title + Title);
}

LRESULT FWindow::WindowProcess(const HWND InWindowHandle, const UINT Message, const WPARAM WParam, const LPARAM LParam)
{
	switch (Message)
	{
	case WM_SIZE:
		OnSizeMessage(LParam);
		break;
	case WM_MOVE:
		OnMoveMessage(LParam);
		break;
	case WM_CLOSE:
		OnCloseMessage();
		return 0;
	case WM_ENTERSIZEMOVE:
		OnEnterSizeMoveMessage(WParam);
		break;
	case WM_EXITSIZEMOVE:
		OnExitSizeMoveMessage(WParam);
		break;
	case WM_SHOWWINDOW:
		OnShowWindowMessage(WParam);
		break;
	case WM_DESTROY:
		OnDestroyMessage();
		break;
	default:
		break;
	}
	return DefWindowProc(InWindowHandle, Message, WParam, LParam);
}

void FWindow::OnSizeMessage(const LPARAM LParam)
{
	State.ClientWidth = LOWORD(LParam);
	State.ClientHeight = HIWORD(LParam);

	RECT WindowRect;
	GetWindowRect(WindowHandle, &WindowRect);
	State.WindowWidth = WindowRect.right - WindowRect.left;
	State.WindowHeight = WindowRect.bottom - WindowRect.top;
}

void FWindow::OnMoveMessage(const LPARAM LParam)
{
	State.PositionX = LOWORD(LParam);
	State.PositionY = HIWORD(LParam);
}

void FWindow::OnCloseMessage()
{
	State.bIsClosed = true;
}

void FWindow::OnEnterSizeMoveMessage(const WPARAM WParam)
{
	if (WParam == SC_MOVE)
		State.bIsMoving = true;
	else if (WParam == SC_SIZE)
		State.bIsResizing = true;
}

void FWindow::OnExitSizeMoveMessage(const WPARAM WParam)
{
	if (WParam == SC_MOVE)
		State.bIsMoving = false;
	else if (WParam == SC_SIZE)
		State.bIsResizing = false;
}

void FWindow::OnShowWindowMessage(const WPARAM WParam)
{
	State.bIsVisible = WParam != 0;
}

void FWindow::OnDestroyMessage()
{
	State.bIsDestroyed = true;
}
