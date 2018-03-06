#pragma once
#include "Timer.h"
#include "Shader.h"

class CScene
{
public:
	CScene();
	~CScene();

	//������ ���콺�� Ű���� �޽����� ó���Ѵ�.
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam,
		LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam,
		LPARAM lParam);

	void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList, float fTotalSize);
	void ReleaseObjects();

	bool ProcessInput(UCHAR *pKeysBuffer);
	bool AnimateObjects(float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);

	bool Collision(XMFLOAT4 *pxmf4WallPlanes, CWallObject *pWalls, CPlayer *pPlayer,
		list<CBulletObject>& bullets, list<CParticleObject>& particles);

	void ReleaseUploadBuffers();

	void SetPlayerPos(XMFLOAT3 position);

	void SetParticleList(list<CParticleObject>* particleList) { m_pShaders[1]->SetParticleList(particleList); }
	void Reset(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) {
		m_pShaders[1]->Reset(pd3dDevice, pd3dCommandList); }

	//�׷��� ��Ʈ �ñ׳��ĸ� �����Ѵ�.
	ID3D12RootSignature *CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	ID3D12RootSignature *GetGraphicsRootSignature();

protected:
	//��ġ(Batch) ó���� �ϱ� ���Ͽ� ���� ���̴����� ����Ʈ�� ǥ���Ѵ�.
	CInstancingShader *m_pShaders[2];
	int m_nShaders = 0;

	ID3D12RootSignature *m_pd3dGraphicsRootSignature = NULL;
};