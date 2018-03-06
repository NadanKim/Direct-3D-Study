#pragma once
#include "Timer.h"
#include "Shader.h"

class CScene
{
public:
	CScene();
	~CScene();

	//�������� �޽���(Ű����, ���콺 �Է�)�� ó���ϴ� �Լ��̴�.
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	// ���ӿ� ���� ������Ʈ���� �����ϰ� �����ϴ� �Լ��̴�.
	void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	void ReleaseObjects();

	// ���� ������� �Է¿� ���� ó���� �����ϴ� �Լ��̴�.
	bool ProcessInput();
	// ������Ʈ�� �����̰� �׸��� �Լ��̴�.
	void AnimateObjects(float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);

	void ReleaseUploadBuffers();

	//�׷��� ��Ʈ �ñ׳��ĸ� �����Ѵ�.
	ID3D12RootSignature *CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	ID3D12RootSignature *GetGraphicsRootSignature();
	void SetGraphicsRootSignature(ID3D12GraphicsCommandList *pd3dCommandList)
	{ pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature); }

protected:
	//���� ���� ��ü���� �����̴�. ���� ��ü�� ���̴��� �����Ѵ�.
	CGameObject **m_ppObjects = NULL;
	int m_nObjects = 0;

	ID3D12RootSignature *m_pd3dGraphicsRootSignature = NULL;
};

