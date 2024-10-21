#include "pch.h"

#include "Renderwerk/Renderer/RendererSubsystem.h"

#include "Renderwerk/Engine/Engine.h"
#include "Renderwerk/Platform/Window.h"
#include "Renderwerk/Platform/WindowSubsystem.h"
#include "Renderwerk/RHI/RHIBackend.h"

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

	Backend = MakeUnique<FRHIBackend>();

	RW_LOG(LogRenderer, Info, "Renderer subsystem initialized");
}

void FRendererSubsystem::Shutdown()
{
	Backend.reset();
	GetEngine()->GetTickDelegate()->Unbind(OnTickHandle);
}

void FRendererSubsystem::OnTick(MAYBE_UNUSED float64 DeltaTime) const
{
	RW_PROFILING_MARK_FUNCTION();

	if (!Window || !Window->IsValid())
		return;
}
