#include "pch.h"

#include "Renderwerk/Renderer/Renderer.h"

#include "Renderwerk/Renderer/D3D12/D3D12RHI.h"

FRenderer::FRenderer(const FRendererSettings& InDescription)
	: Description(InDescription)
{
	FRHIDesc RHIDesc;
	RHIDesc.bEnableDebugging = true;
	RHIDesc.bEnableValidation = true;
	RHI = CreateRHI(RHIDesc);

	TSharedPtr<IAdapter> Adapter = RHI->GetSuitableAdapter();
	RW_LOG_INFO("Adapter: {}", Adapter->GetName());
	RW_LOG_INFO("\t- Vendor: {}", ToString(Adapter->GetVendor()));
	RW_LOG_INFO("\t- Type: {}", ToString(Adapter->GetType()));
	RW_LOG_INFO("\t- Mesh Shading: {}", Adapter->GetCapabilities().bSupportsMeshShading);
	RW_LOG_INFO("\t- Ray Tracing: {}", Adapter->GetCapabilities().bSupportsRayTracing);
	RW_LOG_INFO("\t- Variable Rate Shading: {}", Adapter->GetCapabilities().bSupportsVariableRateShading);

	FDeviceDesc DeviceDesc = {};
	Device = RHI->CreateDevice(Adapter, DeviceDesc);
}

FRenderer::~FRenderer()
{
	Device.reset();
	RHI.reset();
}

TSharedPtr<IRHI> FRenderer::CreateRHI(const FRHIDesc& Desc) const
{
	switch (Description.GraphicsAPI)
	{
	case EGraphicsAPI::DirectX12: return MakeShared<FD3D12RHI>(Desc);
	case EGraphicsAPI::None:
	default:
		RW_DEBUG_ASSERT(false, "Invalid graphics API")
		return nullptr;
	}
}
