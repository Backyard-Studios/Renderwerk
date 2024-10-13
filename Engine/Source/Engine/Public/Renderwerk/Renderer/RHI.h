#pragma once

#include "Adapter.h"

#include "Renderwerk/Core/CoreMinimal.h"

enum class ENGINE_API EGraphicsAPI : uint8
{
	None,
	DirectX12,
};

ENGINE_API std::string ToString(EGraphicsAPI GraphicsAPI);

struct ENGINE_API FRHIDesc
{
	bool8 bEnableDebugging = false;

	/**
	 * Requires bEnableDebugging to be true.
	 */
	bool8 bEnableValidation = false;
};

class ENGINE_API IRHI
{
public:
	IRHI(const EGraphicsAPI& InGraphicsAPI, const FRHIDesc& InDescription);
	virtual ~IRHI() = default;

	DEFINE_DEFAULT_COPY_AND_MOVE(IRHI);

public:
	[[nodiscard]] virtual TVector<TSharedPtr<IAdapter>> GetAdapters() = 0;

public:
	[[nodiscard]] EGraphicsAPI GetGraphicsApi() const { return GraphicsAPI; }

protected:
	EGraphicsAPI GraphicsAPI = EGraphicsAPI::None;

	FRHIDesc Description;
};
