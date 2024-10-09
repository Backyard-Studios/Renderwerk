#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Memory/SmartPointers.h"

#include "Renderwerk/Platform/Window.h"

class ENGINE_API FWindowManager
{
public:
	FWindowManager();
	~FWindowManager();

	DELETE_COPY_AND_MOVE(FWindowManager);

public:
	void Update();
	void ClearRemoveQueue();

	[[nodiscard]] bool Exists(const FGuid& Id) const;
	[[nodiscard]] bool Exists(const TSharedPtr<FWindow>& Id) const;
	[[nodiscard]] TSharedPtr<FWindow> Create(const FWindowSettings& Settings);
	[[nodiscard]] TSharedPtr<FWindow> Get(const FGuid& Id) const;

	void Remove(const FGuid& Id);
	void Remove(const TSharedPtr<FWindow>& Id);

private:
	TMap<FGuid, TSharedPtr<FWindow>> Windows;
	TQueue<FGuid> RemoveQueue;
};
