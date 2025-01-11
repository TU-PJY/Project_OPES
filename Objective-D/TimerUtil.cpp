#include "TimerUtil.h"

// 타이머. 랩 프로젝트의 타이머와 동일하다

Timer::Timer() {
	::QueryPerformanceFrequency((LARGE_INTEGER*)&PerformanceFrequencyPerSec);
	::QueryPerformanceCounter((LARGE_INTEGER*)&LastPerformanceCounter);
	TimeScale = 1.0 / (double)PerformanceFrequencyPerSec;
}

void Timer::Tick(float LockFPS) {
	if (StopState) {
		ElapsedTime = 0.0f;
		return;
	}

	float DeltaTime;

	::QueryPerformanceCounter((LARGE_INTEGER*)&CurrentPerformanceCounter);
	DeltaTime = float((CurrentPerformanceCounter - LastPerformanceCounter) * TimeScale);

	if (LockFPS > 0.0f) {
		while (DeltaTime < (1.0f / LockFPS)) {
			::QueryPerformanceCounter((LARGE_INTEGER*)&CurrentPerformanceCounter);
			DeltaTime = float((CurrentPerformanceCounter - LastPerformanceCounter) * TimeScale);
		}
	}

	LastPerformanceCounter = CurrentPerformanceCounter;

	if (fabsf(DeltaTime - ElapsedTime) < 1.0f) {
		::memmove(&FrameTime[1], FrameTime, (MAX_SAMPLE_COUNT - 1) * sizeof(float));
		FrameTime[0] = DeltaTime;

		if (SampleCount < MAX_SAMPLE_COUNT)
			SampleCount++;
	}

	FramesPerSecond++;
	FPSTimeElapsed += DeltaTime;

	if (FPSTimeElapsed > 1.0f) {
		CurrentFrameRate = FramesPerSecond;
		FramesPerSecond = 0;
		FPSTimeElapsed = 0.0f;
	}

	ElapsedTime = 0.0f;

	for (ULONG i = 0; i < SampleCount; i++)
		ElapsedTime += FrameTime[i];

	if (SampleCount > 0)
		ElapsedTime /= SampleCount;
}

unsigned long Timer::GetFrameRate(LPTSTR String, int Characters) {
	if (String) {
		_itow_s(CurrentFrameRate, String, Characters, 10);
		wcscat_s(String, Characters, _T(" FPS)"));
	}

	return(CurrentFrameRate);
}

float Timer::GetTimeElapsed() {
	return(ElapsedTime);
}

float Timer::GetTotalTime() {
	if (StopState)
		return(float(((StopPerformanceCounter - PausedPerformanceCounter) - BasePerformanceCounter) * TimeScale));

	return(float(((CurrentPerformanceCounter - PausedPerformanceCounter) - BasePerformanceCounter) * TimeScale));
}

void Timer::Reset() {
	__int64 PerformanceCounter;
	::QueryPerformanceCounter((LARGE_INTEGER*)&PerformanceCounter);

	BasePerformanceCounter = PerformanceCounter;
	LastPerformanceCounter = PerformanceCounter;
	StopPerformanceCounter = 0;
	StopState = false;
}

void Timer::Start() {
	__int64 PerformanceCounter;
	::QueryPerformanceCounter((LARGE_INTEGER*)&PerformanceCounter);

	if (StopState) {
		PausedPerformanceCounter += (PerformanceCounter - StopPerformanceCounter);
		LastPerformanceCounter = PerformanceCounter;
		StopPerformanceCounter = 0;
		StopState = false;
	}
}

void Timer::Stop() {
	if (!StopState) {
		::QueryPerformanceCounter((LARGE_INTEGER*)&StopPerformanceCounter);
		StopState = true;
	}
}