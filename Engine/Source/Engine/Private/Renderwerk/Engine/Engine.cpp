#include "pch.h"

#include "Renderwerk/Engine/Engine.h"

#include <csignal>

DEFINE_LOG_CATEGORY(LogEngine);

TSharedPtr<FEngine> GEngine = nullptr;

FEngine::FEngine()
{
	RegisterInterruptSignals();

	OnSignalReceived.Bind(BIND_MEMBER_ONE(FEngine::SignalHandler));
}

FEngine::~FEngine()
{
	OnSignalReceived.Unbind();
}

void FEngine::Run() const
{
	while (bIsRunning)
	{
	}
}

void FEngine::SignalHandler(int Signal)
{
	switch (Signal)
	{
	case SIGTERM:
		RW_LOG(LogEngine, Warn, "Termination signal received");
		break;
	case SIGSEGV:
		RW_LOG(LogEngine, Warn, "Segmentation fault signal received");
		break;
	case SIGINT:
		RW_LOG(LogEngine, Warn, "External Interrupt signal received");
		break;
	case SIGILL:
		RW_LOG(LogEngine, Warn, "Illegal Instruction signal received");
		break;
	case SIGABRT:
		RW_LOG(LogEngine, Warn, "Abort signal received");
		break;
	case SIGFPE:
		RW_LOG(LogEngine, Warn, "Erroneous arithmetic operation signal received");
		break;
	default:
		RW_LOG(LogEngine, Warn, "Signal {} received", Signal);
		break;
	}
	RW_LOG(LogEngine, Warn, "Shutting down the engine");
	bIsRunning = false;
}

void FEngine::RegisterInterruptSignals()
{
	_crt_signal_t SignalHandlerFunc = [](const int Signal)
	{
		if (OnSignalReceived.IsBound())
			OnSignalReceived.Execute(Signal);
	};
	signal(SIGTERM, SignalHandlerFunc);
	signal(SIGSEGV, SignalHandlerFunc);
	signal(SIGINT, SignalHandlerFunc);
	signal(SIGILL, SignalHandlerFunc);
	signal(SIGABRT, SignalHandlerFunc);
	signal(SIGFPE, SignalHandlerFunc);
}

TSharedPtr<FEngine> GetEngine()
{
	DEBUG_ASSERTM(GEngine, "Global engine pointer is null");
	return GEngine;
}
