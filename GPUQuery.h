#pragma once
#include <iostream>
#include <thread>

#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "nvml.h"
#include "returnStructs.h"
#include "combaseapi.h"

class __declspec(dllexport) GPUQuery
{
public:
	GPUQuery();
	~GPUQuery();
	gpuStats* GetGPUStats();
	std::string GetGPUName();
	unsigned int GetMaxClock();

private:
	int gpuDeviceNum;
	std::string gpuName;
	nvmlDevice_t device;
	nvmlUtilization_t utilization;
	nvmlMemory_t memory;
	gpuStats* result = NULL;
	bool alive;
	std::thread backgroundThread;

	unsigned int temp, clock, fanUsage;
	void BackgroundRefresh();
	void ExecuteQuery();
};