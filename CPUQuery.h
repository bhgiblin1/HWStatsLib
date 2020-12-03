#pragma once
#include "windows.h"
#include <Wbemidl.h>
#include <thread>
#include <iostream>
#include "pdh.h"

#include "../pcm/cpucounters.h"
#include "returnStructs.h"

class __declspec(dllexport) CPUQuery
{
public:
	CPUQuery();
	~CPUQuery();
	cpuStats* GetCPUStats();
	std::string GetCPUName();
	short GetNominalClock();

private:
	cpuStats* result = NULL;
	std::thread backgroundThread;
	bool alive;
	// load
	PDH_HQUERY cpuLoadQuery;
	PDH_HCOUNTER cpuLoadTotal;
	PDH_FMT_COUNTERVALUE cpuLoadCounterVal;
	// temp
	pcm::PCM* intelPCM;
	pcm::SystemCounterState scs;
	std::vector<pcm::SocketCounterState> ss;
	std::vector<pcm::CoreCounterState> ccs;
	int thermalMax;
	// clock
	PDH_HQUERY cpuClockQuery;
	PDH_HCOUNTER cpuClockTotal;
	PDH_FMT_COUNTERVALUE cpuClockCounterVal;

	void BackgroundRefresh();
	void Release(IUnknown* x);
};