#pragma once
#include "CPUQuery.h"

extern "C" {
	extern __declspec(dllexport) CPUQuery* CreateCPUQuery();
	extern __declspec(dllexport) void DestroyCPUQuery(CPUQuery* cpuQuery);
	extern __declspec(dllexport) cpuStats* GetCPUStats(CPUQuery* cpuQuery);
	extern __declspec(dllexport) const wchar_t* GetCPUName(CPUQuery* cpuQuery);
	extern __declspec(dllexport) short GetCPUNominalClock(CPUQuery* cpuQuery);
}