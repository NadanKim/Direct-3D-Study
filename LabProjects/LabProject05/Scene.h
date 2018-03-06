#pragma once
#include "Timer.h"

class CScene
{
public:
	CScene();
	~CScene();

	//윈도우의 메시지(키보드, 마우스 입력)를 처리하는 함수이다.
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	// 게임에 사용될 오브젝트들을 생성하고 제거하는 함수이다.
	void BuildObjects(ID3D12Device *pd3dDevice);
	void ReleaseObjects();

	// 실제 사용자의 입력에 대한 처리를 수행하는 함수이다.
	bool ProcessInput();
	// 오브젝트를 움직이고 그리는 함수이다.
	void AnimateObjects(float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList *pd3dCommandList);

	//루트 시그너쳐를 나타내는 인터페이스 포인터이다.
	ID3D12RootSignature *m_pd3dGraphicsRootSignature;
	//파이프라인 상태를 나타내는 인터페이스 포인터이다.
	ID3D12PipelineState *m_pd3dPipelineState;
};

