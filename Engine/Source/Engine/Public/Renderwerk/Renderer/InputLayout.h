#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Renderer/D3D12Include.h"
#include "Renderwerk/Renderer/ShaderCompiler.h"

enum class ENGINE_API FInputElementSlotType : uint8
{
	PerVertexData,
	PerInstanceData,
};

ENGINE_API std::string ToString(FInputElementSlotType Type);

struct ENGINE_API FInputElement
{
	std::string SemanticName;
	uint32 SemanticIndex = 0;
	DXGI_FORMAT Format = DXGI_FORMAT_UNKNOWN;
	uint32 InputSlot = 0;
	uint32 AlignedByteOffset = ~0u;
	FInputElementSlotType SlotType = FInputElementSlotType::PerVertexData;
};

struct ENGINE_API FInputLayout
{
public:
	FInputLayout();
	FInputLayout(const TVector<FInputElement>& InElements);
	FInputLayout(const FShader& Shader);
	~FInputLayout();

	DEFINE_DEFAULT_COPY_AND_MOVE(FInputLayout);

public:
	void AddElement(const FInputElement& Element);

public:
	[[nodiscard]] TVector<FInputElement> GetElements() const { return Elements; }

private:
	TVector<FInputElement> Elements;
};
