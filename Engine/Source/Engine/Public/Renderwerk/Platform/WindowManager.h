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

	[[nodiscard]] bool Exists(const FGuid& Id) const;
	[[nodiscard]] bool Exists(const TSharedPointer<IWindow>& Id) const;
	[[nodiscard]] TSharedPointer<IWindow> Create(const FWindowSettings& Settings);
	[[nodiscard]] TSharedPointer<IWindow> Get(const FGuid& Id) const;

	void Remove(const FGuid& Id);
	void Remove(const TSharedPointer<IWindow>& Id);

private:
	TMap<FGuid, TSharedPointer<IWindow>> Windows;
};
