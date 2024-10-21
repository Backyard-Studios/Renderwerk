#include "pch.h"

#include "Renderwerk/RHI/RHIBackend.h"
#include "Renderwerk/RHI/RHIContext.h"

FRHIBackend::FRHIBackend()
{
	Context = MakeShared<FRHIContext>();
}

FRHIBackend::~FRHIBackend()
{
	Context.reset();
}
