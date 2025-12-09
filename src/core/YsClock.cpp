#include "YsClock.h"

YsClock::YsClock()
{
	name = "";
	startTime = 0.0;
	currTime = 0.0;
	prevTime = 0.0;
	endTime = -1.0;
	elapsedTime = 0.0;
	timeStep = 0.0;
	tickSpeed = 1.0;
	state = IDLE;
}

void YsClock::Increment()
{
}


void YsClock::InitializeClock()
{
	YsClock();
}

void YsClock::StartClock()
{
	state = START;
}

void YsClock::PauseClock()
{
	state = PAUSE;
}

void YsClock::UnpauseClock()
{
	state = RUN;
}

void YsClock::StopClock()
{
	state = STOP;
}

void YsClock::ResetClock()
{
	state = RESET;
}

void YsClock::SetClockSpeed(double mult)
{
	tickSpeed = mult;
}