#include "pch.h"

#include "Renderwerk/Renderer/Renderer.h"

FRenderer::FRenderer(const FRendererSettings& InDescription)
	: Description(InDescription)
{
	FRendererContextDesc RendererContextDesc;
	RendererContextDesc.bEnableDebugging = true;
	RendererContextDesc.bEnableValidation = true;
	Context = MakeShared<FRendererContext>(RendererContextDesc);

	TSharedPtr<FAdapter> Adapter = Context->GetSuitableAdapter();
	RW_LOG_INFO("Adapter: {}", Adapter->GetName());
	RW_LOG_INFO("\t- Vendor: {}", ToString(Adapter->GetVendor()));
	RW_LOG_INFO("\t- Type: {}", ToString(Adapter->GetType()));
	RW_LOG_INFO("\t- Mesh Shading: {}", Adapter->GetCapabilities().bSupportsMeshShading);
	RW_LOG_INFO("\t- Ray Tracing: {}", Adapter->GetCapabilities().bSupportsRayTracing);
	RW_LOG_INFO("\t- Variable Rate Shading: {}", Adapter->GetCapabilities().bSupportsVariableRateShading);

	FDeviceDesc DeviceDesc = {};
	Device = Context->CreateDevice(Adapter, DeviceDesc);

	FSwapchainDesc SwapchainDesc = {};
	SwapchainDesc.Window = Description.Window;
	Swapchain = MakeShared<FSwapchain>(Context->GetFactory(), Device, SwapchainDesc);
}

FRenderer::~FRenderer()
{
	Swapchain.reset();
	Device.reset();
	Context.reset();
}
