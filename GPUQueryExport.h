#pragma once

#include "GPUQuery.h"

extern "C" {
	extern __declspec(dllexport) GPUQuery* CreateGPUQuery();
	extern __declspec(dllexport) void DestroyGPUQuery(GPUQuery* gpuQuery);
	extern __declspec(dllexport) gpuStats* GetGPUStats(GPUQuery* gpuQuery);
	extern __declspec(dllexport) const wchar_t* GetGPUName(GPUQuery* gpuQuery);
	extern __declspec(dllexport) unsigned int GetGPUMaxClock(GPUQuery* gpuQuery);
}