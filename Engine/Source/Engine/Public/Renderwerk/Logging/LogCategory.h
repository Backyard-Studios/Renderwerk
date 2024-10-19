#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/DataTypes/String.h"
#include "Renderwerk/Logging/LogVerbosity.h"

struct RENDERWERK_API ILogCategory
{
	ILogCategory(const FAnsiString& InName, ELogVerbosity InVerbosity);
	~ILogCategory() = default;

	DELETE_COPY_AND_MOVE(ILogCategory);

public:
	NODISCARD CONSTEXPR const FAnsiString& GetName() const { return Name; }
	NODISCARD CONSTEXPR ELogVerbosity GetVerbosity() const { return Verbosity; }

public:
	friend bool operator==(const ILogCategory& Lhs, const ILogCategory& Rhs) { return Lhs.Name == Rhs.Name && Lhs.Verbosity == Rhs.Verbosity; }
	friend bool operator!=(const ILogCategory& Lhs, const ILogCategory& Rhs) { return !(Lhs == Rhs); }

	friend bool operator<=(const ILogCategory& Lhs, const ILogCategory& Rhs) { return Rhs >= Lhs; }
	friend bool operator>(const ILogCategory& Lhs, const ILogCategory& Rhs) { return Rhs < Lhs; }
	friend bool operator>=(const ILogCategory& Lhs, const ILogCategory& Rhs) { return !(Lhs < Rhs); }

	friend bool operator<(const ILogCategory& Lhs, const ILogCategory& Rhs)
	{
		if (Lhs.Name < Rhs.Name)
			return true;
		if (Rhs.Name < Lhs.Name)
			return false;
		return Lhs.Verbosity < Rhs.Verbosity;
	}

private:
	const FAnsiString Name;
	const ELogVerbosity Verbosity;
};

template <ELogVerbosity TCompileTimeVerbosity>
struct RENDERWERK_API FLogCategory : public ILogCategory
{
	FORCEINLINE FLogCategory(const FAnsiString& InName)
		: ILogCategory(InName, TCompileTimeVerbosity)
	{
	}

	~FLogCategory() = default;

	DELETE_COPY_AND_MOVE(FLogCategory);
};

template <>
struct std::hash<ILogCategory>
{
	size_t operator()(const ILogCategory& Category) const noexcept
	{
		return std::hash<FAnsiString>()(Category.GetName());
	}
};

#define DECLARE_LOG_CATEGORY(CategoryName, Verbosity) \
	extern struct FLogCategory##CategoryName : public FLogCategory<ELogVerbosity::Verbosity> \
	{ \
		FORCEINLINE FLogCategory##CategoryName() : FLogCategory(#CategoryName) {} \
	} CategoryName


#define DEFINE_LOG_CATEGORY(CategoryName) FLogCategory##CategoryName CategoryName;
