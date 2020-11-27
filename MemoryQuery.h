#pragma once
#include <iostream>
#include <thread>
#include "windows.h"
#include "returnStructs.h"

class __declspec(dllexport) MemoryQuery {
public:
	MemoryQuery();
	~MemoryQuery();
	memStats* GetMemoryStats();

private:
	MEMORYSTATUSEX memInfo;
	memStats* result;
	std::thread backgroundThread;
	bool alive;

	void BackgroundRefresh();
};