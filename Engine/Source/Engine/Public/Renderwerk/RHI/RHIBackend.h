#pragma once

#include "Renderwerk/RHI/RHICommon.h"

/**
 * @brief The backend of the Render Hardware Interface.
 * The backend is responsible for creating and managing the context, adapters, devices and other resources.
 * It's a lower level interface to interact with the graphics API.
 */
class RENDERWERK_API FRHIBackend
{
public:
	FRHIBackend();
	~FRHIBackend();

	DELETE_COPY_AND_MOVE(FRHIBackend);

public:
	NODISCARD TSharedPtr<FDevice> GetDevice() const { return Device; }

private:
	static TSharedPtr<FAdapter> SelectSuitableAdapter(const TVector<TSharedPtr<FAdapter>>& Adapters);
	static bool8 IsAdapterSuitable(const TSharedPtr<FAdapter>& Adapter);

private:
	TSharedPtr<FRHIContext> Context;
	TSharedPtr<FDevice> Device;
};
