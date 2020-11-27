#include "MemoryQueryExport.h"

MemoryQuery* CreateMemoryQuery()
{
	return new MemoryQuery();
}

void DestroyMemoryQuery(MemoryQuery* memQuery)
{
	if (memQuery != NULL)
		delete memQuery;
}

memStats* GetMemoryStats(MemoryQuery* memQuery)
{
	if (memQuery != NULL)
		return memQuery->GetMemoryStats();
}