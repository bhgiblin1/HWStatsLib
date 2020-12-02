#include "GPUQueryExport.h"
#include <stdio.h>

GPUQuery* CreateGPUQuery()
{
	return new GPUQuery;
}

void DestroyGPUQuery(GPUQuery* gpuQuery)
{
	if (gpuQuery != NULL)
		delete gpuQuery;
}

gpuStats* GetGPUStats(GPUQuery* gpuQuery)
{
	if (gpuQuery != NULL)
		return gpuQuery->GetGPUStats();
}

const wchar_t* GetGPUName(GPUQuery* gpuQuery)
{
	if (gpuQuery != NULL)
	{
		std::string gpuName = gpuQuery->GetGPUName();
		static auto wstr = std::wstring(gpuName.begin(), gpuName.end());
		return wstr.c_str();
	}
}

unsigned int GetGPUMaxClock(GPUQuery* gpuQuery)
{
	if (gpuQuery != NULL)
		return gpuQuery->GetMaxClock();
}