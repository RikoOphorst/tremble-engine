#pragma once
#include "utilities.h"

namespace tremble
{
	/**
	* @class tremble::Timer
	* @brief Class, used to measure delta time and time since startup
	*
	* @author Anton Gavrilov
	*/
	class Timer
	{
	public:
		friend class GameManager;

		Timer(); //!< Initializes time of the startup
		double GetDeltaT() { return std::min(deltaT, 0.5); } //!< Get time, that it took to run the last frame
		double GetTimeSinceStartup() { return timeSinceStartup; } //!< Get time passed from startup to the end of the last frame

		void UpdateTimer(); //!< Recalculates delta time. Meant to be run at the start of each frame
	private:
		void Reset();
		double deltaT; //!< Time, that it took to run the last frame
		double timeSinceStartup; //!< Time passed from startup to the end of the last frame
		u64 startFrame; //!< Time at the end of the last frame
		u64 startTimerInit; //!< Time at construction of timer
		u64 ticks_per_second; //!< ticks per second, used to measure time from ticks passed
	};
}