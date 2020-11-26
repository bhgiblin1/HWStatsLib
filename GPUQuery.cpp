#include "GPUQuery.h"

GPUQuery::GPUQuery()
{
	alive = true;
	nvmlReturn_t result = nvmlInit_v2();
	if (NVML_SUCCESS != result)
	{
		std::cout << "Failed to initialize nvml.dll" << std::endl;
		return;
	}

	// 1 GPU in my system
	gpuDeviceNum = 0;
	cudaDeviceProp properties;

	nvmlReturn_t nvReturn = nvmlDeviceGetHandleByIndex_v2(gpuDeviceNum, &device);
	cudaGetDeviceProperties(&properties, gpuDeviceNum);
	gpuName = properties.name;
	//std::cout << "GPU Name: " << properties.name;
}

GPUQuery::~GPUQuery()
{
	alive = false;
	backgroundThread.join();
	if (result != NULL)
		delete result;
	nvmlShutdown();
}

gpuStats* GPUQuery::GetGPUStats()
{
	result = new gpuStats;

	std::thread bt([this] { this->BackgroundRefresh(); });
	// kick this off in the background. we have the pointer, so can query it any time
	backgroundThread = std::move(bt);

	return result;
}

void GPUQuery::BackgroundRefresh()
{
	while (alive)
	{
		ExecuteQuery();
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
	std::cout << "GPU Background Refresh killed" << std::endl;
}

// https://docs.nvidia.com/deploy/nvml-api/group__nvmlDeviceQueries.html
void GPUQuery::ExecuteQuery()
{
	nvmlDeviceGetTemperature(device, NVML_TEMPERATURE_GPU, &temp);
	nvmlDeviceGetClockInfo(device, NVML_CLOCK_GRAPHICS, &clock);
	nvmlDeviceGetFanSpeed(device, &fanUsage);
	nvmlDeviceGetUtilizationRates(device, &utilization);

	result->temp = this->temp;
	result->clock = this->clock;
	result->fanUsage = this->fanUsage;
	result->load = this->utilization.gpu;

	std::cout << "GPU Temperature: " << temp << "C" << std::endl;
	std::cout << "GPU Clock: " << clock << "MHz" << std::endl;
	std::cout << "GPU Fan Utilization: " << fanUsage << " %" << std::endl;
	std::cout << "GPU Load: " << utilization.gpu << std::endl;
}

std::string GPUQuery::GetGPUName()
{
	return gpuName;
}