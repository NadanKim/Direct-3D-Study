#pragma once
#include "Timer.h"
#include "Shader.h"

class CScene
{
public:
	CScene();
	~CScene();

	//윈도우의 메시지(키보드, 마우스 입력)를 처리하는 함수이다.
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	// 게임에 사용될 오브젝트들을 생성하고 제거하는 함수이다.
	void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	void ReleaseObjects();

	// 실제 사용자의 입력에 대한 처리를 수행하는 함수이다.
	bool ProcessInput();
	// 오브젝트를 움직이고 그리는 함수이다.
	void AnimateObjects(float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);

	void ReleaseUploadBuffers();

	//그래픽 루트 시그너쳐를 생성한다.
	ID3D12RootSignature *CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	ID3D12RootSignature *GetGraphicsRootSignature();
	void SetGraphicsRootSignature(ID3D12GraphicsCommandList *pd3dCommandList)
	{ pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature); }

protected:
	//배치(Batch) 처리를 하기 위하여 씬을 셰이더들의 리스트로 표현한다.
	CObjectsShader *m_pShaders = NULL;
	int m_nShaders = 0;

	ID3D12RootSignature *m_pd3dGraphicsRootSignature = NULL;
};

