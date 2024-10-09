#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

#include "Renderwerk/Graphics/GraphicsCommon.h"

enum class ENGINE_API EGraphicsResourceType : uint8
{
	Buffer,
	Texture,
};

ENGINE_API std::string ToString(const EGraphicsResourceType& Type);

struct ENGINE_API IGraphicsResource
{
public:
	IGraphicsResource(const EGraphicsResourceType& InType);
	virtual ~IGraphicsResource();

	DEFINE_DEFAULT_COPY_AND_MOVE(IGraphicsResource);

public:
	[[nodiscard]] ComPtr<ID3D12Resource2> GetHandle() const { return Resource; }

	[[nodiscard]] EGraphicsResourceType GetType() const { return Type; }

protected:
	EGraphicsResourceType Type;

	D3D12_RESOURCE_DESC Description = {};
	ComPtr<D3D12MA::Allocation> Allocation;
	ComPtr<ID3D12Resource2> Resource;
};

struct ENGINE_API FGraphicsBuffer : IGraphicsResource
{
public:
	explicit FGraphicsBuffer(const ComPtr<D3D12MA::Allocator>& ResourceAllocator, size64 InSize, size64 InCount, void* InData);
	~FGraphicsBuffer() override;

public:
	[[nodiscard]] D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const;
	[[nodiscard]] D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;

public:
	[[nodiscard]] size64 GetSize() const { return Size; }

protected:
	size64 Size;
	size64 Count;
	void* Data;
};
