#pragma once
#include <ysstring.h>
class YsClock
{
	public:

		enum CLOCKSTATE {
			START,
			RUN,
			PAUSE,
			STOP,
			STOPPED,
			RESET,
			IDLE
		};

		YsString name{};
		double startTime{};
		double endTime{};
		double prevTime{};
		double currTime{};
		double elapsedTime{};
		double timeStep{};
		double tickSpeed{};
		CLOCKSTATE state{};

		YsClock();

		void Increment();
		void InitializeClock();
		void StartClock();
		void PauseClock();
		void UnpauseClock();
		void StopClock();
		void ResetClock();
		void SetClockSpeed(double mult);
};

