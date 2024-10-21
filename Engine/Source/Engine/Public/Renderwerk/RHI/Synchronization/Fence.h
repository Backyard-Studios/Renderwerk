#pragma once

#include "Renderwerk/RHI/RHICommon.h"

class RENDERWERK_API FFence : public IDeviceChild
{
public:
	FFence(FDevice* InDevice);
	~FFence() override;

	DELETE_COPY_AND_MOVE(FFence);

public:
	/**
	 * @brief Signals the fence on the CPU.
	 * @note This will use and increment the last signaled value.
	 */
	void SignalCPU();

	/**
	 * @brief Signals the fence on the CPU.
	 * @param Value The value to signal.
	 */
	void SignalCPU(uint64 Value);

	/**
	 * @brief Signals the fence on the GPU.
	 * @note This will use and increment the last signaled value.
	 * @param CommandQueue The command queue to signal.
	 */
	void SignalGPU(const TSharedPtr<FCommandQueue>& CommandQueue);

	/**
	 * @brief Signals the fence on the GPU.
	 * @param CommandQueue The command queue to signal.
	 * @param Value The value to signal.
	 */
	void SignalGPU(const TSharedPtr<FCommandQueue>& CommandQueue, uint64 Value);

	/**
	 * @brief Will wait on the CPU until the fence reaches the specified value.
	 * @note This will use the last signaled value for waiting.
	 * @param Timeout The timeout in milliseconds.
	 */
	void WaitOnCPU(uint64 Timeout = INFINITE) const;

	/**
	 * @brief Will wait on the CPU until the fence reaches the specified value.
	 * @param Value The value to wait for.
	 * @param Timeout The timeout in milliseconds.
	 */
	void WaitOnCPU(uint64 Value, uint64 Timeout = INFINITE) const;

	/**
	 * @brief Will wait on the GPU until the fence reaches the specified value.
	 * @note This will use the last signaled value for waiting.
	 * @param CommandQueue The command queue to wait on.
	 */
	void WaitOnGPU(const TSharedPtr<FCommandQueue>& CommandQueue) const;

	/**
	 * @brief Will wait on the GPU until the fence reaches the specified value.
	 * @param CommandQueue The command queue to wait on.
	 * @param Value The value to wait for.
	 */
	void WaitOnGPU(const TSharedPtr<FCommandQueue>& CommandQueue, uint64 Value) const;

	NODISCARD bool8 IsCompleted(uint64 Value) const;
	NODISCARD uint64 GetLastCompletedValue() const;

public:
	NODISCARD TComPtr<ID3D12Fence1> GetHandle() const { return Fence; }

	NODISCARD uint64 GetLastSignaledValue() const { return LastSignaledValue; }

private:
	TComPtr<ID3D12Fence1> Fence;

	HANDLE EventHandle;
	uint64 LastSignaledValue = 0;
};
