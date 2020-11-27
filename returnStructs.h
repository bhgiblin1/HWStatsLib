#pragma once
typedef struct
{
	unsigned int temp;
	unsigned int clock;
	unsigned int fanUsage;
	unsigned int load;
} gpuStats;

typedef struct
{
	unsigned int load;
	double amtUsed;
} memStats;