#pragma once
#include "DirectX_3D.h"

const ULONG MAX_SAMPLE_COUNT = 50; // Maximum frame time sample count

class Timer{
private:
	double							TimeScale{};
	float							ElapsedTime{};

	__int64							BasePerformanceCounter{};
	__int64							PausedPerformanceCounter{};
	__int64							StopPerformanceCounter{};
	__int64							CurrentPerformanceCounter{};
	__int64							LastPerformanceCounter{};

	__int64							PerformanceFrequencyPerSec{};

	float							FrameTime[MAX_SAMPLE_COUNT]{};
	ULONG							SampleCount{};

	unsigned long					CurrentFrameRate{};
	unsigned long					FramesPerSecond{};
	float							FPSTimeElapsed{};

	bool							StopState{};


public:
	Timer();
	virtual ~Timer() {}
	void Tick(float LockFPS);
	unsigned long GetFrameRate(LPTSTR String, int Characters);
	float GetTimeElapsed();
	float GetTotalTime();
	void Reset();
	void Start();
	void Stop();
};
