#include "pch.h"

#include "Renderwerk/Platform/WindowSubsystem.h"

#include "Renderwerk/Engine/Engine.h"

LRESULT CALLBACK WindowProcess(const HWND WindowHandle, const UINT Message, const WPARAM WParam, const LPARAM LParam)
{
	if (FWindow* Window = reinterpret_cast<FWindow*>(GetWindowLongPtr(WindowHandle, GWLP_USERDATA)))
	{
		return Window->WindowProcess(WindowHandle, Message, WParam, LParam);
	}
	if (Message == WM_CREATE)
	{
		CREATESTRUCT* CreateStruct = reinterpret_cast<CREATESTRUCT*>(LParam);
		SetWindowLongPtr(WindowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(CreateStruct->lpCreateParams));
	}
	return DefWindowProc(WindowHandle, Message, WParam, LParam);
}

DEFINE_LOG_CATEGORY(LogWindow);

FWindowSubsystem::FWindowSubsystem() = default;

FWindowSubsystem::~FWindowSubsystem() = default;

bool8 FWindowSubsystem::IsValidWindowId(const FGuid& Id) const
{
	return Windows.contains(Id);
}

TSharedPtr<FWindow> FWindowSubsystem::NewWindow(const FWindowDesc& Description)
{
	TSharedPtr<FWindow> Window = MakeShared<FWindow>(WindowClass, Description);
	Windows.insert({Window->GetId(), Window});
	return Window;
}

void FWindowSubsystem::DeleteWindow(const FGuid& Id)
{
	TSharedPtr<FWindow> Window = GetWindow(Id);
	if (Window->IsValid())
		Window->Destroy();
	Window.reset();
	Windows.erase(Id);
}

void FWindowSubsystem::DeleteWindow(const TSharedPtr<FWindow>& Window)
{
	DEBUG_ASSERTM(Windows.contains(Window->GetId()), "Window not found")
	DeleteWindow(Window->GetId());
}

TSharedPtr<FWindow> FWindowSubsystem::GetWindow(const FGuid& Id) const
{
	return Windows.at(Id);
}

void FWindowSubsystem::SetMainWindowId(const FGuid& Id)
{
	MainWindowId = Id;
}

void FWindowSubsystem::SetMainWindowId(const TSharedPtr<FWindow>& Window)
{
	SetMainWindowId(Window->GetId());
}

void FWindowSubsystem::Initialize()
{
	WindowClass = {};
	WindowClass.cbSize = sizeof(WNDCLASSEX);
	WindowClass.style = CS_OWNDC;
	WindowClass.lpfnWndProc = WindowProcess;
	WindowClass.hInstance = GetModuleHandle(nullptr);
	WindowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	WindowClass.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
	WindowClass.lpszClassName = TEXT("RenderwerkDefaultWindowClass");
	DEBUG_ASSERTM(RegisterClassEx(&WindowClass) != 0, "Failed to register window class")

	OnTickHandle = GetEngine()->GetTickDelegate()->Bind(BIND_MEMBER_ONE(FWindowSubsystem::OnTick));

	FWindowDesc MainWindowDesc = {};
	TSharedPtr<FWindow> MainWindow = NewWindow(MainWindowDesc);
	SetMainWindowId(MainWindow);
}

void FWindowSubsystem::Shutdown()
{
	for (TSharedPtr<FWindow>& Window : Windows | std::views::values)
	{
		Window->Destroy();
		Window.reset();
	}
	Windows.clear();

	GetEngine()->GetTickDelegate()->Unbind(OnTickHandle);

	DEBUG_ASSERTM(UnregisterClass(WindowClass.lpszClassName, WindowClass.hInstance), "Failed to unregister window class")
}

void FWindowSubsystem::OnTick(MAYBE_UNUSED float64 DeltaTime)
{
	MSG Message = {};
	while (PeekMessage(&Message, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

	for (TSharedPtr<FWindow>& Window : Windows | std::views::values)
	{
		if (!Window->IsValid())
			WindowsToDelete.push(Window->GetId());
	}

	while (!WindowsToDelete.empty())
	{
		FGuid Id = WindowsToDelete.front();
		DeleteWindow(Id);
		if (Id == MainWindowId)
		{
			MainWindowId = {};
			RW_LOG(LogWindow, Warn, "Main window was destroyed. Requesting exit...");
			GetEngine()->RequestExit();
		}
		WindowsToDelete.pop();
	}
}
