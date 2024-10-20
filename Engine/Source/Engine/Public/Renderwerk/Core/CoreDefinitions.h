#pragma once

// Used to suppress warnings about macros that should be enums
#define FORWARD(...) __VA_ARGS__

#define INNER_CONCAT(ValueA, ValueB) ValueA##ValueB
#define CONCAT(ValueA, ValueB) INNER_CONCAT(ValueA, ValueB)

// Generates a unique name in compile time by using the line number as a suffix
#define INNER_UNIQUE_NAME(Prefix, Line) CONCAT(Prefix, Line)
#define UNIQUE_NAME(Prefix) INNER_UNIQUE_NAME(Prefix, __LINE__)

#define INNER_STRINGIFY(Value) #Value
#define STRINGIFY(Value) INNER_STRINGIFY(Value)

#define DELETE_COPY_CONSTRUCTOR(Type) Type(const Type&) = delete
#define DELETE_COPY_ASSIGNMENT(Type) Type& operator=(const Type&) = delete

#define DELETE_MOVE_CONSTRUCTOR(Type) Type(Type&&) = delete
#define DELETE_MOVE_ASSIGNMENT(Type) Type& operator=(Type&&) = delete

#define DELETE_COPY_AND_MOVE(Type) \
	DELETE_COPY_CONSTRUCTOR(Type); \
	DELETE_COPY_ASSIGNMENT(Type); \
	DELETE_MOVE_CONSTRUCTOR(Type); \
	DELETE_MOVE_ASSIGNMENT(Type)

#define DEFINE_DEFAULT_COPY_CONSTRUCTOR(Type) Type(const Type&) = default
#define DEFINE_DEFAULT_COPY_ASSIGNMENT(Type) Type& operator=(const Type&) = default

#define DEFINE_DEFAULT_MOVE_CONSTRUCTOR(Type) Type(Type&&) = default
#define DEFINE_DEFAULT_MOVE_ASSIGNMENT(Type) Type& operator=(Type&&) = default

#define DEFINE_DEFAULT_COPY_AND_MOVE(Type) \
	DEFINE_DEFAULT_COPY_CONSTRUCTOR(Type); \
	DEFINE_DEFAULT_COPY_ASSIGNMENT(Type); \
	DEFINE_DEFAULT_MOVE_CONSTRUCTOR(Type); \
	DEFINE_DEFAULT_MOVE_ASSIGNMENT(Type)
