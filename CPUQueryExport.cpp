#include "CPUQueryExport.h"

CPUQuery* CreateCPUQuery()
{
	return new CPUQuery();
}

void DestroyCPUQuery(CPUQuery* cpuQuery)
{
	if (cpuQuery != NULL)
		delete cpuQuery;
}

cpuStats* GetCPUStats(CPUQuery* cpuQuery)
{
	if (cpuQuery != NULL)
		return cpuQuery->GetCPUStats();
}

const wchar_t* GetCPUName(CPUQuery* cpuQuery)
{
	if (cpuQuery != NULL)
	{
		std::string cpuName = cpuQuery->GetCPUName();
		static auto wstr = std::wstring(cpuName.begin(), cpuName.end());
		return wstr.c_str();
	}
}

short GetCPUNominalClock(CPUQuery* cpuQuery)
{
	if (cpuQuery != NULL)
		return cpuQuery->GetNominalClock();
}