#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Core/Version.h"

struct ENGINE_API FApplicationMetadata
{
	std::string Name;
	FVersion Version;
	std::string VersionSuffix = "";
};

class ENGINE_API IApplication
{
public:
	IApplication(const FApplicationMetadata& InMetadata)
		: Metadata(InMetadata)
	{
	}

	virtual ~IApplication() = default;

	DELETE_COPY_AND_MOVE(IApplication);

public:
	virtual void Initialize()
	{
	}

	virtual void Shutdown()
	{
	}

public:
	[[nodiscard]] FApplicationMetadata GetMetadata() const { return Metadata; }

protected:
	const FApplicationMetadata Metadata;
};
