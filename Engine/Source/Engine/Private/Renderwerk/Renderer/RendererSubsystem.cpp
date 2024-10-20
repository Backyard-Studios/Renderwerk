#include "pch.h"

#include "Renderwerk/Renderer/RendererSubsystem.h"

#include "Renderwerk/Engine/Engine.h"
#include "Renderwerk/Platform/Window.h"
#include "Renderwerk/Platform/WindowSubsystem.h"
#include "Renderwerk/Renderer/Graphics/Adapter.h"
#include "Renderwerk/Renderer/Graphics/GraphicsContext.h"

DEFINE_LOG_CATEGORY(LogRenderer);

FRendererSubsystem::FRendererSubsystem() = default;

FRendererSubsystem::~FRendererSubsystem() = default;

void FRendererSubsystem::Initialize()
{
	RW_PROFILING_MARK_FUNCTION();

	OnTickHandle = GetEngine()->GetTickDelegate()->Bind(BIND_MEMBER_ONE(FRendererSubsystem::OnTick));

	TSharedPtr<FWindowSubsystem> WindowSubsystem = GetSubsystem<FWindowSubsystem>();
	Window = WindowSubsystem->GetWindow(WindowSubsystem->GetMainWindowId());
	ASSERTM(Window, "Failed to get main window");

	GraphicsContext = MakeShared<FGraphicsContext>();

	TSharedPtr<FAdapter> SelectedAdapter = SelectSuitableAdapter(GraphicsContext->QueryAdapters());
	FAdapterCapabilities Capabilities = SelectedAdapter->GetCapabilities();
	Window->AppendTitle(std::format(TEXT(" | D3D12<{}, {}>"), ToString(Capabilities.MaxFeatureLevel), ToString(Capabilities.MaxShaderModel)).c_str());
	RW_LOG(LogRenderer, Info, "Selected Adapter: {}", SelectedAdapter->GetName());

	RW_LOG(LogRenderer, Info, "Renderer subsystem initialized");
}

void FRendererSubsystem::Shutdown()
{
	GraphicsContext.reset();
	GetEngine()->GetTickDelegate()->Unbind(OnTickHandle);
}

void FRendererSubsystem::OnTick(MAYBE_UNUSED float64 DeltaTime) const
{
	RW_PROFILING_MARK_FUNCTION();

	if (!Window || !Window->IsValid())
		return;
}

TSharedPtr<FAdapter> FRendererSubsystem::SelectSuitableAdapter(const TVector<TSharedPtr<FAdapter>>& Adapters)
{
	TSharedPtr<FAdapter> SelectedAdapter;
	RW_LOG(LogRenderer, Info, "Available Adapters:", Adapters.size());
	for (const TSharedPtr<FAdapter>& Adapter : Adapters)
	{
		RW_LOG(LogRenderer, Info, "\t- Adapter{}:", Adapter->GetIndex());
		RW_LOG(LogRenderer, Info, "\t\t- Name: {}", Adapter->GetName());
		RW_LOG(LogRenderer, Info, "\t\t- Type: {}", ToString(Adapter->GetType()));
		RW_LOG(LogRenderer, Info, "\t\t- Vendor: {}", ToString(Adapter->GetVendor()));
		RW_LOG(LogRenderer, Info, "\t\t- Feature Level: {}", ToString(Adapter->GetCapabilities().MaxFeatureLevel));
		RW_LOG(LogRenderer, Info, "\t\t- Shader Model: {}", ToString(Adapter->GetCapabilities().MaxShaderModel));
		RW_LOG(LogRenderer, Info, "\t\t- Ray Tracing: {}", ToString(Adapter->GetCapabilities().RayTracingTier));
		RW_LOG(LogRenderer, Info, "\t\t- Variable Shading Rate: {}", ToString(Adapter->GetCapabilities().VariableShadingRateTier));
		RW_LOG(LogRenderer, Info, "\t\t- Additional Shading Rates: {}", Adapter->GetCapabilities().bSupportsAdditionalShadingRates);
		RW_LOG(LogRenderer, Info, "\t\t- Mesh Shader: {}", ToString(Adapter->GetCapabilities().MeshShaderTier));

		if (IsAdapterSuitable(Adapter))
			SelectedAdapter = Adapter;
	}
	ASSERTM(SelectedAdapter, "No suitable adapter found");
	return SelectedAdapter;
}

bool8 FRendererSubsystem::IsAdapterSuitable(const TSharedPtr<FAdapter>& Adapter)
{
	if (Adapter->GetType() != EAdapterType::Discrete)
		return false;
	if (Adapter->GetCapabilities().MaxFeatureLevel < EFeatureLevel::Level_12_2)
		return false;
	if (Adapter->GetCapabilities().MaxShaderModel < EShaderModel::SM_6_8)
		return false;
	if (Adapter->GetCapabilities().RayTracingTier < ERayTracingTier::Tier_1_0)
		return false;
	if (Adapter->GetCapabilities().VariableShadingRateTier < EVariableShadingRateTier::Tier_1)
		return false;
	if (Adapter->GetCapabilities().MeshShaderTier < EMeshShaderTier::Tier_1)
		return false;
	return true;
}
