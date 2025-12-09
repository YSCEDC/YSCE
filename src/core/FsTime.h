#pragma once
#include <time.h>
#include <windows.h>
#include <ysclass.h>
#include <ysclass11.h>
#include "YsClock.h"

class FsTime
{
public:
	FsTime();

	YSBOOL timeInitialized;
	long long int currentRawTime;
	long long int rawTimeOverflow;
	double currentRealTime;
	double realTimeStep;

	void InitializeTime();
	void UpdateCurrentRawTime(void); //set this frame's time to system uptime GetTickCount()
	long long int GetCurrentRawTime(void); //return currentRawTime
	void UpdateRealTime(); //convert system uptime to decimal time
	double GetRealTime(); //return currentRealTime
	double GetRealTimeStep(); //return realTimeStep

	void IncrementAllClocks();
	void IncrementThisClock(YsClock& clk);

	YsListContainer <YsClock> clockList;
	YsListAllocator <YsClock> FsClockAllocator;

	YsClock CreateClock(YsString name); //Make a new clock and start it counting
	YSRESULT AddClock(YsClock& clk, int& idx); //Add clock object to list, get clock index in return
	YSRESULT DeleteClock(YsClock* clkPtr);
	YSRESULT GetClockByIndex(int idx, YsClock& clk);
	YSRESULT GetClockByName(YsString name, YsClock& clk);
	YSRESULT GetIndexByClock(YsClock& clk, int idx);
	YSBOOL CompareClocks(YsClock clk1, YsClock clk2);

	YsClock programClock;
	YsClock worldClock;
	YsClock flightClock;
};
