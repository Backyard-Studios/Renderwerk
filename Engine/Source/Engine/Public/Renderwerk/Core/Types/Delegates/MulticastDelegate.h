#pragma once

#include "Renderwerk/Core/CoreAPI.h"
#include "Renderwerk/Core/Types/Containers.h"
#include "Renderwerk/Core/Types/Types.h"

using FDelegateHandle = uint64;

template <typename... TArguments>
class RENDERWERK_API TMulticastDelegate
{
public:
	using FFunctionType = void(TArguments...);
	using FDelegateEntry = std::pair<FDelegateHandle, TFunction<FFunctionType>>;

public:
	TMulticastDelegate() = default;
	~TMulticastDelegate() = default;

	DEFINE_DEFAULT_COPY_AND_MOVE(TMulticastDelegate);

public:
	NODISCARD FDelegateHandle Bind(const TFunction<FFunctionType>&& InFunction)
	{
		const FDelegateHandle Handle = NewHandleId();
		Delegates.push_back({Handle, std::move(InFunction)});
		return Handle;
	}

	void Unbind(const FDelegateHandle Handle)
	{
		std::erase_if(Delegates, [Handle](const FDelegateEntry& Entry) { return Entry.first == Handle; });
	}

	NODISCARD void Execute(TArguments... Arguments) const
	{
		for (const FDelegateEntry& Entry : Delegates)
			Entry.second(std::forward<TArguments>(Arguments)...);
	}

private:
	INLINE static uint64 NextHandleId = 0;

	static uint64 NewHandleId()
	{
		return NextHandleId++;
	}

private:
	TVector<FDelegateEntry> Delegates;
};

#define DECLARE_MULTICAST_DELEGATE(Name, ...) using F##Name##Delegate = TMulticastDelegate<__VA_ARGS__>
