#pragma once

#if RW_ENABLE_PROFILING
#	include "tracy/Tracy.hpp"
#	define RW_PROFILER_INIT() tracy::StartupProfiler()
#	define RW_PROFILER_SHUTDOWN() tracy::ShutdownProfiler()
#	define RW_IS_PROFILER_STARTED() tracy::IsProfilerStarted()
#	define RW_PROFILING_SET_APP_NAME(Name) TracySetProgramName(Name)
#	define RW_PROFILING_MARK_FRAME_START() FrameMarkStart(nullptr)
#	define RW_PROFILING_MARK_FRAME_END() FrameMarkEnd(nullptr)
#	define RW_PROFILING_MARK_FUNCTION() ZoneScoped
#	define RW_PROFILING_MARK_SCOPE(Name) ZoneScopedN(Name)
#	define RW_PROFILING_SET_SCOPE_NAME(Format, ...) ZoneNameF(Format, __VA_ARGS__)
#	define RW_PROFILING_MARK_THREAD(Name) tracy::SetThreadName(Name)
#	define RW_PROFILING_MARK_THREAD_HINT(Name, Hint) tracy::SetThreadNameWithHint(Name, Hint)
#	define RW_PROFILING_MEMORY_ALLOCATION(Pointer, Size) TracyAlloc(Pointer, Size)
#	define RW_PROFILING_MEMORY_FREE(Pointer) TracyFree(Pointer)
#
#	include "tracy/TracyD3D12.hpp"
using FD3D12ProfilerContext = TracyD3D12Ctx;
#	define RW_PROFILING_D3D12_CREATE_CONTEXT(Device, CommandQueue) TracyD3D12Context(Device, CommandQueue)
#	define RW_PROFILING_D3D12_DESTROY_CONTEXT(Context) TracyD3D12Destroy(Context)
#	define RW_PROFILING_D3D12_COLLECT(Context) TracyD3D12Collect(Context)
#	define RW_PROFILING_D3D12_MARK_FRAME(Context) TracyD3D12NewFrame(Context)
#	define RW_PROFILING_D3D12_MARK_SCOPE(Context, CommandList, Name) TracyD3D12Zone(Context, CommandList, Name)
#else
#	define RW_PROFILER_INIT()
#	define RW_PROFILER_SHUTDOWN()
#	define RW_IS_PROFILER_STARTED() false
#	define RW_PROFILING_SET_APP_NAME(Name)
#	define RW_PROFILING_MARK_FRAME_START()
#	define RW_PROFILING_MARK_FRAME_END()
#	define RW_PROFILING_MARK_FUNCTION()
#	define RW_PROFILING_MARK_SCOPE(Name)
#	define RW_PROFILING_SET_SCOPE_NAME(Format, ...)
#	define RW_PROFILING_MARK_THREAD(Name)
#	define RW_PROFILING_MEMORY_ALLOCATION(Pointer, Size)
#	define RW_PROFILING_MEMORY_FREE(Pointer)

using FD3D12ProfilerContext = void*;
#	define RW_PROFILING_D3D12_CREATE_CONTEXT(Device, CommandQueue) nullptr
#	define RW_PROFILING_D3D12_DESTROY_CONTEXT(Context) Context = nullptr
#	define RW_PROFILING_D3D12_COLLECT(Context)
#	define RW_PROFILING_D3D12_MARK_FRAME(Context)
#	define RW_PROFILING_D3D12_MARK_SCOPE(Context, CommandList, Name)
#endif
