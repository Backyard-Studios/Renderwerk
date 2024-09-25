#pragma once

#include <Renderwerk/Core/Assertion.h>

#include <d3d12.h>
#include <d3d12sdklayers.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>

#include <wrl.h>

using Microsoft::WRL::ComPtr;

#define CHECK_RESULT(Result, ...) RW_DEBUG_ASSERT(SUCCEEDED(Result), __VA_ARGS__)
#define CHECK_RESULT_CRITICAL(Result, ...) RW_ASSERT_CRITICAL(SUCCEEDED(Result), "Direct3D 12 Error: %s", __VA_ARGS__)
