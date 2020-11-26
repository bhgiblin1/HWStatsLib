
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "nvml.h"

#include "../pcm/cpucounters.h"

#include "windows.h"
#include "pdh.h"

#include <stdio.h>
#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#include <Wbemidl.h>
#include "GPUQuery.h"

void getRAMUsage();
void gpuInfo();
void getCPULoad();
short getCPUMaxClock();
void getCPUTemp();
void getCPUClock(short maxClock);

// install windows SDK and windows driver kit... needed to add stuff to vs 2019
// to link PCM. git clone dir above. build as lib. properties -> linker. add pcm.lib. add it to this path also
int main()
{
	auto gpu = std::make_shared<GPUQuery>();
	gpu->GetGPUStats();

	/*auto x = getCPUMaxClock();
	getCPUClock(x);*/
	return 0;
}

void log(std::string msg)
{
	std::cout << msg << std::endl;
}

void getCPULoad()
{
	PDH_HQUERY cpuQuery;
	PDH_HCOUNTER cpuTotal;
	PdhOpenQuery(NULL, NULL, &cpuQuery);
	PdhAddEnglishCounter(cpuQuery, LPCSTR("\\Processor(_Total)\\% Processor Time"), NULL, &cpuTotal);
	PDH_FMT_COUNTERVALUE counterVal;
	PdhCollectQueryData(cpuQuery);

	while (true)
	{
		PdhCollectQueryData(cpuQuery);
		PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
		std::cout << counterVal.doubleValue << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
	PdhCloseQuery(cpuQuery);
}

// https://stackoverflow.com/questions/61802420/unable-to-get-current-cpu-frequency-in-powershell-or-python
void getCPUClock(short maxClock)
{
	PDH_HQUERY cpuQuery;
	PDH_HCOUNTER cpuTotal;
	PdhOpenQuery(NULL, NULL, &cpuQuery);
	PdhAddEnglishCounter(cpuQuery, LPCSTR("\\Processor Information(_Total)\\% Processor Performance"), NULL, &cpuTotal);
	PDH_FMT_COUNTERVALUE counterVal;
	PdhCollectQueryData(cpuQuery);

	while (true)
	{
		PdhCollectQueryData(cpuQuery);
		PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
		std::cout << maxClock * (counterVal.doubleValue / 100) << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
	PdhCloseQuery(cpuQuery);
}


void getCPUTemp()
{
	auto intelPCM = pcm::PCM::getInstance();
	std::cout << intelPCM->getCPUBrandString() << std::endl;

	// way to read this?
	int thermalMax = 100;

	pcm::SystemCounterState scs;
	std::vector<pcm::SocketCounterState> ss;
	std::vector<pcm::CoreCounterState> ccs;
	int averageTemp = 0;

	while (true)
	{
		intelPCM->getAllCounterStates(scs, ss, ccs);
		for (auto state : ccs)
		{
			averageTemp += thermalMax - state.getThermalHeadroom();
		}

		std::cout << "Average Temp = " << averageTemp / intelPCM->getNumCores() << "C" << std::endl;
		averageTemp = 0;
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

// wmic /namespace:\\root\wmi PATH MSAcpi_ThermalZoneTemperature get *
// cimv2 SELECT * FROM Win32_Processor
// https://docs.microsoft.com/en-us/windows/win32/wmisdk/example--getting-wmi-data-from-the-local-computer
short getCPUMaxClock()
{
	IWbemLocator* pLoc = NULL;
	IWbemServices* pSvc = NULL;
	IEnumWbemClassObject* pEnumerator = NULL;
	IWbemClassObject* pclsObj = NULL;
	HRESULT hr = NULL;

	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);

	if (SUCCEEDED(hr))
	{
		log("CoInitializeSecurity start");
		hr = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
		if (SUCCEEDED(hr))
		{
			log("CoCreateInstance start");
			hr = CoCreateInstance(CLSID_WbemLocator, NULL, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&pLoc);
			if (SUCCEEDED(hr))
			{
				log("ConnectServer start");
				hr = pLoc->ConnectServer(BSTR(L"ROOT\\cimv2"), NULL, NULL, NULL, NULL, NULL, NULL, &pSvc);
				if (SUCCEEDED(hr))
				{
					log("CoSetProxyBlanket start");
					hr = CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
					if (SUCCEEDED(hr))
					{
						log("ExecQuery start");
						hr = pSvc->ExecQuery(BSTR(L"WQL"), BSTR(L"SELECT * FROM Win32_Processor"), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);
						if (SUCCEEDED(hr))
						{
							ULONG uReturn = 0;
							while (pEnumerator)
							{
								hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
								if (uReturn == 0)
									break;
								VARIANT vtProp;
								pclsObj->Get(L"MaxClockSpeed", 0, &vtProp, NULL, NULL);
								std::cout << "MaxClockSpeed : " << vtProp.iVal << std::endl;
								return vtProp.iVal;
								VariantClear(&vtProp);
							}
						}
					}
				}
			}

		}
	}

	//cleanup 

	return NULL;
}

// https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/ns-sysinfoapi-memorystatusex
void getRAMUsage()
{
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	std::cout << "RAM Load: " << memInfo.dwMemoryLoad << "%" << std::endl;
	// byte to gibibyte
	std::cout << "RAM Used: " << (memInfo.ullTotalPhys - memInfo.ullAvailPhys) / 1.074e+9 << "GB" << std::endl;
}

// https://docs.nvidia.com/deploy/nvml-api/group__nvmlDeviceQueries.html
void gpuInfo()
{
	nvmlReturn_t result = nvmlInit_v2();
	if (NVML_SUCCESS != result)
	{
		std::cout << "Failed to initialize nvml.dll" << std::endl;
		return;
	}

	// 1 GPU in my system
	int gpuDeviceNum = 0;
	cudaDeviceProp properties;
	nvmlDevice_t device;
	unsigned int temp = 0, clock = 0, speed = 0;
	nvmlUtilization_t utilization;

	nvmlReturn_t nvReturn = nvmlDeviceGetHandleByIndex_v2(gpuDeviceNum, &device);
	cudaGetDeviceProperties(&properties, gpuDeviceNum);

	std::cout << "GPU Name: " << properties.name;
	while (true)
	{
		nvmlDeviceGetTemperature(device, NVML_TEMPERATURE_GPU, &temp);
		std::cout << "GPU Temperature: " << temp << "C" << std::endl;
		nvmlDeviceGetClockInfo(device, NVML_CLOCK_GRAPHICS, &clock);
		std::cout << "GPU Clock: " << clock << "MHz" << std::endl;
		nvmlDeviceGetFanSpeed(device, &speed);
		std::cout << "GPU Fan Utilization: " << speed << " %" << std::endl;
		nvmlDeviceGetUtilizationRates(device, &utilization);
		std::cout << "GPU Load: " << utilization.gpu << std::endl;
		getRAMUsage();

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}
