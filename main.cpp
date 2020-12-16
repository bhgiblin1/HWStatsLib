
#include <iostream>
#include <chrono>
#include <thread>
#include "GPUQuery.h"
#include "CPUQuery.h"

void audioDev();
// install windows SDK and windows driver kit... needed to add stuff to vs 2019
// to link PCM. git clone dir above. build as lib. properties -> linker. add pcm.lib. add it to this path also
int main()
{
	auto gpu = std::make_shared<GPUQuery>();
	gpu->GetGPUStats();
	// auto cpu = std::make_shared<CPUQuery>();
	// std::cout << cpu->GetNominalClock();
	audioDev();

	std::cin.get();
	return 0;
}

void audioDev()
{

}