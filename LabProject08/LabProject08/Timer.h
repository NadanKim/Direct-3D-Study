#pragma once

//50ȸ�� ������ ó���ð��� �����Ͽ� ����Ѵ�.
const ULONG MAX_SAMPLE_COUNT = 50;

class CGameTimer
{
private:
	bool m_bHardwareHasPerformanceCounter;
	float m_fTimeScale;
	float m_fTimeElapsed;

	__int64 m_nCurrentTime;
	__int64 m_nLastTime;

	__int64 m_nBaseTime;
	__int64 m_nPausedTime;
	__int64 m_nStopTime;

	__int64 m_nPerformanceFrequency;

	float m_fFrameTime[MAX_SAMPLE_COUNT];
	ULONG m_nSampleCount;

	unsigned long m_nCurrentFrameRate;
	unsigned long m_nFramesPerSecond;
	float m_fFPSTimeElapsed;

	bool m_bStopped;
public:
	CGameTimer();
	virtual ~CGameTimer();

	//Ÿ�̸� �ð� �����Ѵ�.
	void Tick(float fLockFPS = 0.0f);
	void Reset();
	void Start();
	void Stop();
	//������ ����Ʈ �� ��ȯ�Ѵ�.
	unsigned long GetFrameRate(LPTSTR lpszString = NULL, int nCharacters = 0);
	//�������� ��� ��� �ð��� ��ȯ�Ѵ�.
	float GetTimeElapsed();
	float GetTotalTime();
};

