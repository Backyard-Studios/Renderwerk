#include "pch.h"

#include "Renderwerk/Platform/Win32/Win32Window.h"

FWin32Window::FWin32Window(const FWindowSettings& InWindowSettings)
	: FWin32Window(InWindowSettings, NewGuid())
{
}

FWin32Window::FWin32Window(const FWindowSettings& InWindowSettings, const FGuid& InGuid)
	: IWindow(InWindowSettings, InGuid)
{
	uint32 WindowStyle = WS_OVERLAPPEDWINDOW;
	uint32 WindowExStyle = WS_EX_APPWINDOW;

	if (Settings.bIsVisibleAfterCreation)
		WindowStyle |= WS_VISIBLE;

	int32 PositionX = Settings.PositionX;
	int32 PositionY = Settings.PositionY;

	HWND ParentHandle = nullptr;
	if (Settings.ParentWindow)
		ParentHandle = static_cast<HWND>(Settings.ParentWindow->GetNativeHandle());

	WindowHandle = CreateWindowEx(WindowExStyle,
	                              GetWin32Platform()->GetWindowClass().lpszClassName,
	                              ToWide(Settings.Title.c_str()).c_str(),
	                              WindowStyle,
	                              PositionX, PositionY,
	                              static_cast<int32>(Settings.Width), static_cast<int32>(Settings.Height),
	                              ParentHandle,
	                              nullptr,
	                              GetWin32Platform()->GetWindowClass().hInstance,
	                              this);
}

FWin32Window::~FWin32Window()
{
	if (WindowHandle && IsWindow(WindowHandle))
		DestroyWindow(WindowHandle);
}

void FWin32Window::Show()
{
	ShowWindow(WindowHandle, SW_SHOW);
}

void FWin32Window::Hide()
{
	ShowWindow(WindowHandle, SW_HIDE);
}

void FWin32Window::Minimize()
{
	ShowWindow(WindowHandle, SW_MINIMIZE);
}

void FWin32Window::Maximize()
{
	ShowWindow(WindowHandle, SW_MAXIMIZE);
}

void FWin32Window::Restore()
{
	ShowWindow(WindowHandle, SW_RESTORE);
}

void FWin32Window::Focus()
{
	SetFocus(WindowHandle);
	SetForegroundWindow(WindowHandle);
}

void FWin32Window::Close()
{
	CloseWindow(WindowHandle);
}

void FWin32Window::Destroy()
{
	DestroyWindow(WindowHandle);
}

void FWin32Window::SetPosition(const int32 PositionX, const int32 PositionY)
{
	SetWindowPos(WindowHandle, nullptr, PositionX, PositionY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

void FWin32Window::SetSize(const int32 Width, const int32 Height)
{
	SetWindowPos(WindowHandle, nullptr, 0, 0, Width, Height, SWP_NOMOVE | SWP_NOZORDER);
}

void FWin32Window::SetTitle(const std::string& Title)
{
	State.Title = Title;
	SetWindowText(WindowHandle, ToWide(Title.c_str()).c_str());
}

void* FWin32Window::GetNativeHandle() const
{
	return WindowHandle;
}

LRESULT FWin32Window::WindowProcess(const HWND InWindowHandle, const UINT Message, const WPARAM WParam, const LPARAM LParam)
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

void FWin32Window::OnSizeMessage(const LPARAM LParam)
{
	State.ClientWidth = LOWORD(LParam);
	State.ClientHeight = HIWORD(LParam);

	RECT WindowRect;
	GetWindowRect(WindowHandle, &WindowRect);
	State.WindowWidth = WindowRect.right - WindowRect.left;
	State.WindowHeight = WindowRect.bottom - WindowRect.top;
}

void FWin32Window::OnMoveMessage(const LPARAM LParam)
{
	State.PositionX = LOWORD(LParam);
	State.PositionY = HIWORD(LParam);
}

void FWin32Window::OnCloseMessage()
{
	State.bIsClosed = true;
}

void FWin32Window::OnEnterSizeMoveMessage(const WPARAM WParam)
{
	if (WParam == SC_MOVE)
		State.bIsMoving = true;
	else if (WParam == SC_SIZE)
		State.bIsResizing = true;
}

void FWin32Window::OnExitSizeMoveMessage(const WPARAM WParam)
{
	if (WParam == SC_MOVE)
		State.bIsMoving = false;
	else if (WParam == SC_SIZE)
		State.bIsResizing = false;
}

void FWin32Window::OnShowWindowMessage(const WPARAM WParam)
{
	State.bIsVisible = WParam != 0;
}

void FWin32Window::OnDestroyMessage()
{
	State.bIsDestroyed = true;
}
