#pragma once
#include "MemoryQuery.h"

extern "C" {
	extern __declspec(dllexport) MemoryQuery* CreateMemoryQuery();
	extern __declspec(dllexport) void DestroyMemoryQuery(MemoryQuery* memQuery);
	extern __declspec(dllexport) memStats* GetMemoryStats(MemoryQuery* memQuery);
}