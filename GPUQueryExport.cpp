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
