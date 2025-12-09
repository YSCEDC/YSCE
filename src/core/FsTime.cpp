#include "FsTime.h"

YSBOOL FsTime::CompareClocks(YsClock clk1, YsClock clk2)
{
	if (clk1.startTime == clk2.startTime &&
		clk1.currTime == clk2.currTime &&
		clk1.prevTime == clk2.prevTime &&
		clk1.endTime == clk2.endTime &&
		clk1.elapsedTime == clk2.elapsedTime &&
		clk1.timeStep == clk2.timeStep &&
		clk1.tickSpeed == clk2.tickSpeed &&
		clk1.state == clk2.state &&
		clk1.name == clk2.name)
	{
		return YSTRUE;
	}
	YSFALSE;
}

FsTime::FsTime() : clockList(FsClockAllocator)
{
}

void FsTime::InitializeTime()
{
	currentRawTime = 0;
	rawTimeOverflow = 0;
	currentRealTime = 0.0;
	realTimeStep = 0.0;
	timeInitialized = YSTRUE;
	programClock.InitializeClock();
	worldClock.InitializeClock();
}

void FsTime::UpdateCurrentRawTime(void)
{
	long long int tickCount = GetTickCount();

	if (timeInitialized != YSTRUE)
	{
		InitializeTime();
	}

	if (tickCount < currentRawTime) // Underflow.  49.7 days have passed.
	{
		rawTimeOverflow += 0x100000000LL;
	}
	currentRawTime = tickCount;
}

long long int FsTime::GetCurrentRawTime()
{
	return rawTimeOverflow + currentRawTime;
}

void FsTime::UpdateRealTime()
{
	double timeSinceLastUpdate;
	UpdateCurrentRawTime();
	timeSinceLastUpdate = GetCurrentRawTime() / 1000.0 - currentRealTime;

	if (timeSinceLastUpdate >= 0.010)
	{
		double prev = currentRealTime;
		UpdateCurrentRawTime();
		currentRealTime = GetCurrentRawTime() / 1000.0;
		realTimeStep = currentRealTime - prev;
	}
	else //Frame time is too low, sleep a while
	{
		Sleep(5);
		UpdateRealTime();
	}
}

double FsTime::GetRealTime()
{
	return currentRealTime;
}

double FsTime::GetRealTimeStep()
{
	return realTimeStep;
}

void FsTime::IncrementAllClocks()
{
	UpdateRealTime();

	for (int i = 0; i < clockList.GetN(); i++)
	{
		YsListItem <YsClock>* neo;
		neo = clockList.GetItemFromId(i);
		IncrementThisClock(neo->dat);
	}
}

void FsTime::IncrementThisClock(YsClock& clk)
{
	switch (clk.state)
	{
	case YsClock::CLOCKSTATE::START:
		clk.startTime = GetRealTime();
		clk.currTime = clk.startTime;
		clk.prevTime = clk.startTime;
		clk.endTime = -1;
		clk.elapsedTime = 0;
		clk.timeStep = 0;
		clk.tickSpeed = 1.0;
		clk.state = YsClock::CLOCKSTATE::RUN;
		break;

	case YsClock::CLOCKSTATE::RUN:
		clk.prevTime = clk.currTime;
		clk.currTime += realTimeStep * clk.tickSpeed;
		clk.elapsedTime = clk.currTime - clk.startTime;
		clk.timeStep = clk.currTime - clk.prevTime;
		break;

	case YsClock::CLOCKSTATE::STOP:
		clk.endTime = GetRealTime();
		clk.elapsedTime = clk.endTime - clk.startTime;
		clk.currTime = clk.endTime;
		clk.state == YsClock::CLOCKSTATE::STOPPED;
		break;

	case YsClock::CLOCKSTATE::RESET:
		clk.startTime = 0;
		clk.currTime = 0;
		clk.prevTime = 0;
		clk.endTime = -1;
		clk.elapsedTime = 0;
		clk.timeStep = 0;
		clk.tickSpeed = 1.0;
		clk.state = YsClock::CLOCKSTATE::IDLE;
		break;

	case YsClock::CLOCKSTATE::PAUSE:
	case YsClock::CLOCKSTATE::IDLE:
		break;
	}

}

//--------------------------------------
YsClock FsTime::CreateClock(YsString name)
{
	YsClock clk{};

	clk.name = name;

	return clk;
}

YSRESULT FsTime::AddClock(YsClock& clk, int& idx)
{
	YsListItem <YsClock>* item;
	item = clockList.Create();
	if (item != NULL)
	{
		item->dat = clk;
		idx = clockList.GetIdFromItem(item);
		return YSOK;
	}

	return YSERR;
}

YSRESULT FsTime::DeleteClock(YsClock* clkPtr)
{
	/*
	if (clkPtr->thisInTheList->GetContainer() == &clockList)
	{
		int listIdx;
		listIdx = clkPtr->thisInTheList->GetContainer()->GetIdFromItem(clkPtr->thisInTheList);
		clockList.Delete(clockList.GetItemFromId(listIdx));

		if (clkPtr->thisInTheList->GetContainer() != &clockList)
		{
			return YSOK;
		}
	}
	return YSERR;
	*/

	return YSOK;
}

YSRESULT FsTime::GetClockByIndex(int idx, YsClock& clk)
{
	YsClock nullclock;
	nullclock.InitializeClock();
	YsString nullstr = "";
	YsListItem <YsClock>* neo;

	neo = clockList.GetItemFromId(idx);
	if (neo->dat.name != nullstr)
	{
		clk = neo->dat;
		return YSOK;
	}
	return YSERR;
}

YSRESULT FsTime::GetClockByName(YsString name, YsClock& clk)
{
	for (int i = 0; i < clockList.GetN(); i++)
	{
		YsListItem <YsClock>* neo;
		neo = clockList.GetItemFromId(i);

		if (neo->dat.name == name)
		{
			clk = neo->dat;
			return YSOK;
		}
	}
	return YSERR;
}

YSRESULT FsTime::GetIndexByClock(YsClock& clk, int idx)
{
	for (int i = 0; i < clockList.GetN(); i++)
	{
		YsListItem <YsClock>* neo;
		neo = clockList.GetItemFromId(i);

		if (CompareClocks(neo->dat, clk) == YSTRUE)
		{
			idx = i;
			return YSOK;
		}
	}
	return YSERR;
}