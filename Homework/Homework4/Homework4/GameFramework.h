#pragma once
#include "Timer.h"
#include "Scene.h"
#include "Player.h"

class CGameFramework
{
private:
	HINSTANCE m_hInstance;
	HWND m_hWnd;

	int m_nWndClientWidth;
	int m_nWndClientHeight;

//다음은 게임 프레임 워크에서 사용할 타이머 이다.
	CGameTimer m_GameTimer;

//다음은 프레임 레이트를 주 윈도우의 캡션에 출력하기 위한 문자열이다.
	_TCHAR m_pszFrameRate[50];

//DXGI 팩토리 인터페이스에 대한 포인터
	IDXGIFactory4 *m_pdxgiFactory;

//스왑 체인 인터페이스에 대한 포인터, 주로 디스플레이를 제어하기 위하여 필요
	IDXGISwapChain3 *m_pdxgiSwapChain;

//Direct3D 디바이스 인터페이스에 대한 포인터, 주로 리소스를 생성하기 위하여 필요
	ID3D12Device *m_pd3dDevice;


//MSAA 다중 샘플링을 활성화 하고 다중 샘플링 레벨을 설정한다.
	bool m_bMsaa4xEnable = false;
	UINT m_nMsaa4xQualityLevels = 0;


//스왑 체인의 후면 버퍼의 개수이다.
	static const UINT m_nSwapChainBuffers = 2;

//현재 스왑 체인의 후면 버퍼 인덱스 이다.
	UINT m_nSwapChainBufferIndex;


//렌더 타겟 버퍼, 서술자 힙 인터페이스 포인터, 렌더 타겟 서술자 원소의 크기이다.
	ID3D12Resource *m_ppd3dRenderTargetBuffers[m_nSwapChainBuffers];
	ID3D12DescriptorHeap *m_pd3dRtvDescriptorHeap;
	UINT m_nRtvDescriptorIncrementSize;


//깊이-스텐실 버퍼, 서술자 힙 인터페이스 포인터, 깊이-스텐실 서술자 원소의 크기이다.
	ID3D12Resource *m_pd3dDepthStencilBuffer;
	ID3D12DescriptorHeap *m_pd3dDsvDescriptorHeap;
	UINT m_nDsvDescriptorIncrementSize;


//명령 큐, 명령 할당자, 명령 리스트 인터페이스 포인터이다.
	ID3D12CommandQueue *m_pd3dCommandQueue;
	ID3D12CommandAllocator *m_pd3dCommandAllocator;
	ID3D12GraphicsCommandList *m_pd3dCommandList;


//펜스 인터페이스 포인터, 펜스의 값, 이벤트 핸들이다.
	ID3D12Fence *m_pd3dFence;
	UINT64 m_nFenceValues[m_nSwapChainBuffers];
	HANDLE m_hFenceEvent;


#if defined(_DEBUG)
	ID3D12Debug *m_pd3dDebugController;
#endif

	CScene *m_pScene;
public: 
	CCamera *m_pCamera = NULL;

	//플레이어 객체에 대한 포인터이다.
	CPlayer *m_pPlayer = NULL;

	//마지막으로 마우스 버튼을 클릭할 때의 마우스 커서의 위치이다.
	POINT m_ptOldCursorPos;

	CGameObject *m_pSelectedObject = NULL;

public:
	CGameFramework();
	~CGameFramework();

	void MoveToNextFrame();


//프레임 워크를 초기화 하는 함수이다.(주 윈도우가 생성되면 호출된다.)
	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestory();


//스왑 체인, 디바이스, 서술자 힙, 명령 큐/할당자/리스트를 생성하는 함수이다.
	void CreateSwapChain();
	void CreateDirect3DDevice();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateCommandQueueAndList();
	void CreateRenderTargetView();
	void CreateDepthStencilView();


//렌더링 할 메쉬와 게임 객체를 생성하고 소멸하는 함수이다.
	void BuildObjects();
	void ReleaseObjects();


	// 충돌체크 함수들을 모아두는 것
	void Collisions();


//프레임 워크의 핵심(사용자 입력, 애니메이션, 렌더링)을 구성하는 함수이다.
	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();


//CPU와 GPU를 동기화 하는 함수이다.
	void WaitForGpuComplete();


//윈도우의 메시지(키보드, 마우스 입력)를 처리하는 함수이다.
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam,
		LPARAM lParam);
	void OnResizeBackBuffers();

	void ProcessSelectedObject();
};

