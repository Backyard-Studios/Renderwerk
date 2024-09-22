#pragma once

#include "Renderwerk/Core/CoreDefinitions.h"
#include "Renderwerk/Core/CoreTypes.h"

#include "Renderwerk/Platform/Window.h"

class ENGINE_API FWindowManager
{
public:
	FWindowManager();
	~FWindowManager();

	DELETE_COPY_AND_MOVE(FWindowManager)

public:
	void Update();
	void ClearRemoveQueue();

	[[nodiscard]] bool Exists(const FGuid& Id) const;
	[[nodiscard]] bool Exists(const TSharedPointer<FWindow>& Id) const;
	[[nodiscard]] TSharedPointer<FWindow> Create(const FWindowSettings& Settings);
	[[nodiscard]] TSharedPointer<FWindow> Get(const FGuid& Id) const;

	void Remove(const FGuid& Id);
	void Remove(const TSharedPointer<FWindow>& Id);

private:
	TMap<FGuid, TSharedPointer<FWindow>> Windows;
	TQueue<FGuid> RemoveQueue;
};
