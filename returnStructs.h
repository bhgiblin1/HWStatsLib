#pragma once
typedef struct
{
	unsigned int temp;
	unsigned int clock;
	unsigned int fanUsage;
	unsigned int load;
	double memoryTotal;
	double memoryUsage;
} gpuStats;

typedef struct
{
	unsigned int load;
	double amtUsed;
} memStats;

typedef struct
{
	int temp;
	double load;
	double clockPercent;
} cpuStats;