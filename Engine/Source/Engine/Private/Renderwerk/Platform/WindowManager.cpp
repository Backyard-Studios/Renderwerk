﻿#include "pch.h"

#include "Renderwerk/Platform/WindowManager.h"

FWindowManager::FWindowManager()
{
}

FWindowManager::~FWindowManager()
{
	for (TSharedPtr<FWindow>& Window : Windows | std::views::values)
		Window.reset();
	Windows.clear();
}

void FWindowManager::Update()
{
	RW_PROFILING_MARK_FUNCTION();

#if RW_PLATFORM_WINDOWS
	{
		RW_PROFILING_MARK_SCOPE("Win32MessageHandling");

		MSG Message = {};
		while (PeekMessage(&Message, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
	}
#endif

	{
		RW_PROFILING_MARK_SCOPE("CollectDestroyedWindows");

		for (const TSharedPtr<FWindow>& Window : Windows | std::views::values)
		{
			if (Window->GetWindowState().bIsDestroyed)
				Remove(Window);
		}
	}
}

void FWindowManager::ClearRemoveQueue()
{
	RW_PROFILING_MARK_FUNCTION();

	while (!RemoveQueue.empty())
	{
		FGuid Id = RemoveQueue.front();
		RemoveQueue.pop();
		RW_LOG_INFO("Removing window {} [Title: {}]", Id.ToString(), Windows.at(Id)->GetWindowState().Title);
		Windows.erase(Id);
	}
}

bool FWindowManager::Exists(const FGuid& Id) const
{
	return Windows.contains(Id);
}

bool FWindowManager::Exists(const TSharedPtr<FWindow>& Id) const
{
	return Exists(Id->GetGuid());
}

TSharedPtr<FWindow> FWindowManager::Create(const FWindowSettings& Settings)
{
	TSharedPtr<FWindow> Window = MakeShared<FWindow>(Settings);
	Windows.insert({Window->GetGuid(), Window});
	RW_LOG_INFO("New Window created: {} [Title: {}, Size: {}x{}]", Window->GetGuid().ToString(), Settings.Title, Settings.Width, Settings.Height);
	return Window;
}

TSharedPtr<FWindow> FWindowManager::Get(const FGuid& Id) const
{
	return Windows.at(Id);
}

void FWindowManager::Remove(const FGuid& Id)
{
	if (!Exists(Id))
		return;
	RW_LOG_DEBUG("Window {} marked for removal", Id.ToString());
	RemoveQueue.push(Id);
}

void FWindowManager::Remove(const TSharedPtr<FWindow>& Id)
{
	Remove(Id->GetGuid());
}
