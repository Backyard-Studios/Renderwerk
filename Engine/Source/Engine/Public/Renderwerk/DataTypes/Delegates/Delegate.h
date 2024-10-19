#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/DataTypes/Types.h"

template <typename TReturnValue, typename... TArguments>
class RENDERWERK_API TDelegate
{
public:
	using FFunctionType = TReturnValue(TArguments...);

public:
	TDelegate() = default;
	~TDelegate() = default;

	DEFINE_DEFAULT_COPY_AND_MOVE(TDelegate);

public:
	void Bind(const TFunction<FFunctionType>&& InFunction)
	{
		Function = std::move(InFunction);
	}

	void Unbind()
	{
		Function = nullptr;
	}

	/**
	 * @brief Executes the delegate.
	 * @note You should check if the delegate is bound before executing
	 * @param Arguments The arguments to pass to the delegate.
	 * @return The return value of the delegate.
	 */
	NODISCARD TReturnValue Execute(TArguments&&... Arguments) const
	{
		ASSERTM(IsBound(), "Attempted to execute an empty delegate");
		return Function(std::forward<TArguments>(Arguments)...);
	}

	NODISCARD bool8 IsBound() const
	{
		return Function != nullptr;
	}

private:
	TFunction<FFunctionType> Function;
};

#define DECLARE_DELEGATE(Name, ...) using F##Name##Delegate = TDelegate<void, __VA_ARGS__>
#define DECLARE_DELEGATE_RET(Name, ReturnValue, ...) using F##Name##Delegate = TDelegate<ReturnValue, __VA_ARGS__>
