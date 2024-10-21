#include "pch.h"

#include "Renderwerk/Platform/Window.h"

FWindow::FWindow(const WNDCLASSEX& WindowClass, const FWindowDesc& InDescription)
	: Id(NewGuid()), Description(InDescription)
{
	uint32 Style = GetStyleFromDescription(Description);
	uint32 ExtendedStyle = GetExtendedStyleFromDescription(Description);

	int32 PositionX = 200;
	int32 PositionY = 200;

	if (Description.bUseCustomPosition)
	{
		PositionX = Description.CustomPositionX;
		PositionY = Description.CustomPositionY;
	}

	State.WindowWidth = Description.Width;
	State.WindowHeight = Description.Height;
	State.ClientWidth = Description.Width;
	State.ClientHeight = Description.Height;
	State.PositionX = PositionX;
	State.PositionY = PositionY;
	State.Title = Description.Title;

	HWND ParentWindowHandle = nullptr;
	if (Description.ParentWindow)
		ParentWindowHandle = Description.ParentWindow->GetHandle();
	WindowHandle = CreateWindowEx(ExtendedStyle,
	                              WindowClass.lpszClassName,
	                              Description.Title.c_str(),
	                              Style,
	                              PositionX, PositionY,
	                              Description.Width, Description.Height,
	                              ParentWindowHandle,
	                              nullptr,
	                              WindowClass.hInstance,
	                              this);
	ASSERTM(WindowHandle, "Failed to create window")
}

FWindow::~FWindow()
{
	if (WindowHandle && IsWindow(WindowHandle))
		DestroyWindow(WindowHandle);
}

void FWindow::Show() const
{
	ShowWindow(WindowHandle, SW_SHOW);
}

void FWindow::Hide() const
{
	ShowWindow(WindowHandle, SW_HIDE);
}

void FWindow::Minimize() const
{
	ShowWindow(WindowHandle, SW_MINIMIZE);
}

void FWindow::Maximize() const
{
	ShowWindow(WindowHandle, SW_MAXIMIZE);
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

void FWindow::Destroy()
{
	DEBUG_ASSERTM(WindowHandle && IsWindow(WindowHandle), "Window handle is invalid")
	DestroyWindow(WindowHandle);
	WindowHandle = nullptr;
}

void FWindow::SetPosition(const int32 PositionX, const int32 PositionY) const
{
	SetWindowPos(WindowHandle, nullptr, PositionX, PositionY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

void FWindow::SetSize(const int32 Width, const int32 Height) const
{
	SetWindowPos(WindowHandle, nullptr, 0, 0, Width, Height, SWP_NOMOVE | SWP_NOZORDER);
}

void FWindow::SetTitle(const FString& Title)
{
	State.Title = Title;
	SetWindowText(WindowHandle, Title.c_str());
}

void FWindow::AppendTitle(const FString& Title)
{
	SetTitle(State.Title + Title);
}

bool8 FWindow::IsValid() const
{
	return WindowHandle && !IsClosed() && !IsDestroyed();
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
	case WM_ENTERSIZEMOVE:
		OnEnterSizeMoveMessage(WParam);
		break;
	case WM_EXITSIZEMOVE:
		OnExitSizeMoveMessage(WParam);
		break;
	case WM_SHOWWINDOW:
		OnShowWindowMessage(WParam);
		break;
	case WM_SETFOCUS:
		OnSetFocusMessage();
		break;
	case WM_KILLFOCUS:
		OnKillFocusMessage();
		break;
	case WM_CLOSE:
		OnCloseMessage();
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

	OnWindowResized.Execute(State.WindowWidth, State.WindowHeight);
	OnClientAreaResized.Execute(State.ClientWidth, State.ClientHeight);
}

void FWindow::OnMoveMessage(const LPARAM LParam)
{
	State.PositionX = LOWORD(LParam);
	State.PositionY = HIWORD(LParam);
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

void FWindow::OnSetFocusMessage()
{
	State.bIsFocused = true;
	OnFocusChange.Execute(true);
}

void FWindow::OnKillFocusMessage()
{
	State.bIsFocused = false;
	OnFocusChange.Execute(false);
}

void FWindow::OnCloseMessage()
{
	bIsClosed = true;
}

void FWindow::OnDestroyMessage()
{
	bIsDestroyed = true;
}

uint32 FWindow::GetStyleFromDescription(const FWindowDesc& Description)
{
	uint32 Style = 0;

	if (Description.Style == EWindowStyle::Windowed)
		Style |= WS_OVERLAPPEDWINDOW;
	else if (Description.Style == EWindowStyle::Borderless)
		Style |= WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (Description.ParentWindow)
		Style |= WS_CHILDWINDOW;

	if (Description.bShowAfterCreation)
		Style |= WS_VISIBLE;

	return Style;
}

uint32 FWindow::GetExtendedStyleFromDescription(const FWindowDesc& Description)
{
	uint32 ExStyle = 0;
	if (Description.Style == EWindowStyle::Windowed)
		ExStyle |= WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW;
	else if (Description.Style == EWindowStyle::Borderless)
		ExStyle |= WS_EX_APPWINDOW;
	return ExStyle;
}
