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

//������ ���� ������ ��ũ���� ����� Ÿ�̸� �̴�.
	CGameTimer m_GameTimer;

//������ ������ ����Ʈ�� �� �������� ĸ�ǿ� ����ϱ� ���� ���ڿ��̴�.
	_TCHAR m_pszFrameRate[50];

//DXGI ���丮 �������̽��� ���� ������
	IDXGIFactory4 *m_pdxgiFactory;

//���� ü�� �������̽��� ���� ������, �ַ� ���÷��̸� �����ϱ� ���Ͽ� �ʿ�
	IDXGISwapChain3 *m_pdxgiSwapChain;

//Direct3D ����̽� �������̽��� ���� ������, �ַ� ���ҽ��� �����ϱ� ���Ͽ� �ʿ�
	ID3D12Device *m_pd3dDevice;


//MSAA ���� ���ø��� Ȱ��ȭ �ϰ� ���� ���ø� ������ �����Ѵ�.
	bool m_bMsaa4xEnable = false;
	UINT m_nMsaa4xQualityLevels = 0;


//���� ü���� �ĸ� ������ �����̴�.
	static const UINT m_nSwapChainBuffers = 2;

//���� ���� ü���� �ĸ� ���� �ε��� �̴�.
	UINT m_nSwapChainBufferIndex;


//���� Ÿ�� ����, ������ �� �������̽� ������, ���� Ÿ�� ������ ������ ũ���̴�.
	ID3D12Resource *m_ppd3dRenderTargetBuffers[m_nSwapChainBuffers];
	ID3D12DescriptorHeap *m_pd3dRtvDescriptorHeap;
	UINT m_nRtvDescriptorIncrementSize;


//����-���ٽ� ����, ������ �� �������̽� ������, ����-���ٽ� ������ ������ ũ���̴�.
	ID3D12Resource *m_pd3dDepthStencilBuffer;
	ID3D12DescriptorHeap *m_pd3dDsvDescriptorHeap;
	UINT m_nDsvDescriptorIncrementSize;


//���� ť, ���� �Ҵ���, ���� ����Ʈ �������̽� �������̴�.
	ID3D12CommandQueue *m_pd3dCommandQueue;
	ID3D12CommandAllocator *m_pd3dCommandAllocator;
	ID3D12GraphicsCommandList *m_pd3dCommandList;


//�潺 �������̽� ������, �潺�� ��, �̺�Ʈ �ڵ��̴�.
	ID3D12Fence *m_pd3dFence;
	UINT64 m_nFenceValues[m_nSwapChainBuffers];
	HANDLE m_hFenceEvent;


#if defined(_DEBUG)
	ID3D12Debug *m_pd3dDebugController;
#endif

	CScene *m_pScene;
public: 
	CCamera *m_pCamera = NULL;

	//�÷��̾� ��ü�� ���� �������̴�.
	CPlayer							*m_pPlayer = NULL;

	// �� ��ü�� ���� ������
	CWallObject					*m_pWalls;

	// ���� �� ����
	float				fTotalWallsLength;

	// ���� �浹 �˻縦 ���� ����
	XMFLOAT4					m_pxmf4WallPlanes[4];

	// ���� ���� �� ����
	int m_nWallCount;

	//���������� ���콺 ��ư�� Ŭ���� ���� ���콺 Ŀ���� ��ġ�̴�.
	POINT m_ptOldCursorPos;

	// �Ѿ� �� �Ѿ� �����̳�
	CBulletMeshDiffused	*m_bulletMesh;
	list<CBulletObject>	m_lBullets;

	// �Ѿ� �߻� �������ֱ� ���� ����
	float					bulletDelay = 0.0f;

	// ��ƼŬ �����̳�
	list<CParticleObject> m_lParticles;

public:
	CGameFramework();
	~CGameFramework();

	void MoveToNextFrame();


//������ ��ũ�� �ʱ�ȭ �ϴ� �Լ��̴�.(�� �����찡 �����Ǹ� ȣ��ȴ�.)
	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestory();


//���� ü��, ����̽�, ������ ��, ���� ť/�Ҵ���/����Ʈ�� �����ϴ� �Լ��̴�.
	void CreateSwapChain();
	void CreateDirect3DDevice();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateCommandQueueAndList();
	void CreateRenderTargetView();
	void CreateDepthStencilView();


//������ �� �޽��� ���� ��ü�� �����ϰ� �Ҹ��ϴ� �Լ��̴�.
	void BuildObjects();
	void ReleaseObjects();


//������ ��ũ�� �ٽ�(����� �Է�, �ִϸ��̼�, ������)�� �����ϴ� �Լ��̴�.
	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();

	//�浹üũ �� �Լ�
	void CollisionChecker();


//CPU�� GPU�� ����ȭ �ϴ� �Լ��̴�.
	void WaitForGpuComplete();


//�������� �޽���(Ű����, ���콺 �Է�)�� ó���ϴ� �Լ��̴�.
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam,
		LPARAM lParam);
	void OnResizeBackBuffers();
};
