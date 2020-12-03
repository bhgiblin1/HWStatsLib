#include "CPUQuery.h"

CPUQuery::CPUQuery()
{
	// load
	PdhOpenQuery(NULL, NULL, &cpuLoadQuery);
	PdhAddEnglishCounter(cpuLoadQuery, LPCSTR("\\Processor(_Total)\\% Processor Time"), NULL, &cpuLoadTotal);
	// temp
	intelPCM = pcm::PCM::getInstance();
	// any way to read this?
	thermalMax = 100;
	//load
	PdhOpenQuery(NULL, NULL, &cpuClockQuery);
	PdhAddEnglishCounter(cpuClockQuery, LPCSTR("\\Processor Information(_Total)\\% Processor Performance"), NULL, &cpuClockTotal);
	alive = true;
}

CPUQuery::~CPUQuery()
{
	alive = false;
	PdhCloseQuery(cpuLoadQuery);
	PdhCloseQuery(cpuClockQuery);
	// PCM cleanup?
	if (backgroundThread.joinable())
		backgroundThread.join();
	if (result != NULL)
		delete result;
}

cpuStats* CPUQuery::GetCPUStats()
{
	result = new cpuStats();
	
	std::thread bt([this] { this->BackgroundRefresh(); });
	backgroundThread = std::move(bt);

	return result;
}

// wmic /namespace:\\root\wmi PATH MSAcpi_ThermalZoneTemperature get *
// cimv2 SELECT * FROM Win32_Processor
// https://stackoverflow.com/questions/61802420/unable-to-get-current-cpu-frequency-in-powershell-or-python
void CPUQuery::BackgroundRefresh()
{
	int averageTemp = 0;
	while (alive)
	{
		auto t1 = std::chrono::high_resolution_clock::now();

		// load
		PdhCollectQueryData(cpuLoadQuery);
		PdhGetFormattedCounterValue(cpuLoadTotal, PDH_FMT_DOUBLE, NULL, &cpuLoadCounterVal);
		
		// temp
		averageTemp = 0;
		intelPCM->getAllCounterStates(scs, ss, ccs);
		for (auto state : ccs)
		{
			averageTemp += thermalMax - state.getThermalHeadroom();
		}
		auto overallTemp = averageTemp / intelPCM->getNumCores();

		// clock
		PdhCollectQueryData(cpuClockQuery);
		PdhGetFormattedCounterValue(cpuClockTotal, PDH_FMT_DOUBLE, NULL, &cpuClockCounterVal);

		result->load = cpuLoadCounterVal.doubleValue;
		result->temp = overallTemp;
		result->clockPercent = cpuClockCounterVal.doubleValue;

		std::cout << "CPU Load " << cpuLoadCounterVal.doubleValue << std::endl;
		std::cout << "Average Temp = " << overallTemp << "C" << std::endl;
		std::cout << "CPU Clock " << cpuClockCounterVal.doubleValue << std::endl;

		auto t2 = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
		std::cout << "CPU Query duration = " << duration << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
	std::cout << "CPU Background Refresh killed" << std::endl;
}

std::string CPUQuery::GetCPUName()
{
	return intelPCM->getCPUBrandString();
}

// https://docs.microsoft.com/en-us/windows/win32/wmisdk/example--getting-wmi-data-from-the-local-computer
short CPUQuery::GetNominalClock()
{
	IWbemLocator* pLoc = NULL;
	IWbemServices* pSvc = NULL;
	IEnumWbemClassObject* pEnumerator = NULL;
	IWbemClassObject* pclsObj = NULL;
	HRESULT hr = NULL;
	short result = 0;

	std::cout << "CoInitializeEx start";
	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (SUCCEEDED(hr))
	{
		// cannot call this from a DLL 
		// std::cout << "CoInitializeSecurity start";
		// hr = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
		if (SUCCEEDED(hr))
		{
			std::cout << "CoCreateInstance start";
			hr = CoCreateInstance(CLSID_WbemLocator, NULL, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&pLoc);
			if (SUCCEEDED(hr))
			{
				std::cout << "ConnectServer start";
				hr = pLoc->ConnectServer(BSTR(L"ROOT\\cimv2"), NULL, NULL, NULL, NULL, NULL, NULL, &pSvc);
				if (SUCCEEDED(hr))
				{
					std::cout << "CoSetProxyBlanket start";
					hr = CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
					if (SUCCEEDED(hr))
					{
						std::cout << "ExecQuery start";
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
								result = vtProp.iVal;
								std::cout << "MaxClockSpeed : " << result << std::endl;
								VariantClear(&vtProp);
							}
						}
					}
				}
			}
		}
		Release(pLoc);
		Release(pSvc);
		Release(pEnumerator);
		Release(pclsObj);
		CoUninitialize();
	}
	return result;
}

void CPUQuery::Release(IUnknown *x)
{
	if (x != NULL)
		x->Release();
}