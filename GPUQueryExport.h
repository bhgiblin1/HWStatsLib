#pragma once

#include "GPUQuery.h"

extern "C" {
	extern __declspec(dllexport) GPUQuery* CreateGPUQuery();
	extern __declspec(dllexport) void DestroyGPUQuery(GPUQuery* gpuQuery);
	extern __declspec(dllexport) gpuStats* GetGPUStats(GPUQuery* gpuQuery);
}