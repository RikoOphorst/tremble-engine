#include "timer.h"

namespace tremble
{
	Timer::Timer()
	{
		Reset();
	}

	void Timer::UpdateTimer()
	{
		u64 end;
		QueryPerformanceCounter((LARGE_INTEGER*)&end);

		deltaT = (end - startFrame) / (double)ticks_per_second;
		timeSinceStartup = (end - startTimerInit) / (double)ticks_per_second;
		startFrame = end;
	}

	void Timer::Reset()
	{
		QueryPerformanceFrequency((LARGE_INTEGER*)&ticks_per_second);
		QueryPerformanceCounter((LARGE_INTEGER*)&startTimerInit);
		QueryPerformanceCounter((LARGE_INTEGER*)&startFrame);
	}
}