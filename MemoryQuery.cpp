#include "MemoryQuery.h"

MemoryQuery::MemoryQuery()
{
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	alive = true;
}

MemoryQuery::~MemoryQuery()
{
	alive = false;
	backgroundThread.join();
	if (result != NULL)
		delete result;
}

memStats* MemoryQuery::GetMemoryStats()
{
	result = new memStats;

	std::thread bt([this] { this->BackgroundRefresh(); });
	backgroundThread = std::move(bt);

	return result;
}

// https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-globalmemorystatusex
void MemoryQuery::BackgroundRefresh()
{
	while (alive)
	{
		GlobalMemoryStatusEx(&memInfo);
		result->load = memInfo.dwMemoryLoad;
		// byte to gibibyte
		result->amtUsed = (memInfo.ullTotalPhys - memInfo.ullAvailPhys) / 1.074e+9;

		std::cout << "RAM Load: " << result->load << "%" << std::endl;
		std::cout << "RAM Used: " << result->amtUsed << "GB" << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
	std::cout << "Memory Background Refresh killed" << std::endl;
}