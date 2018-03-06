#pragma once
#include "Timer.h"
#include "Shader.h"

#define Material_Player 0
#define Material_Roof 1
#define Material_Wall 2
#define Material_Enemy 3
#define Material_Sun 4
#define Material_Moon 5
#define Material_Terrain 6
#define Material_Enemy2 7

struct LIGHT
{
	XMFLOAT4				m_xmf4Ambient;
	XMFLOAT4				m_xmf4Diffuse;
	XMFLOAT4				m_xmf4Specular;
	XMFLOAT3				m_xmf3Position;
	float 					m_fFalloff;
	XMFLOAT3				m_xmf3Direction;
	float 					m_fTheta; //cos(m_fTheta)
	XMFLOAT3				m_xmf3Attenuation;
	float					m_fPhi; //cos(m_fPhi)
	bool					m_bEnable;
	int						m_nType;
	float					m_fRange;
	float					padding;
};

struct LIGHTS
{
	LIGHT					m_pLights[MAX_LIGHTS];
	XMFLOAT4				m_xmf4GlobalAmbient;
};

struct MATERIALS
{
	MATERIAL				m_pReflections[MAX_MATERIALS];
};


class CObjectsShader;

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

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList);
	void ReleaseObjects();

	void BuildLightsAndMaterials(void *pContext, int nFileNumber);

	bool ProcessInput(UCHAR *pKeysBuffer);
	void AnimateObjects(float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);

	void ReleaseUploadBuffers();

	//�׷��� ��Ʈ �ñ׳��ĸ� �����Ѵ�.
	ID3D12RootSignature *CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	ID3D12RootSignature *GetGraphicsRootSignature();

	void Collision();

	void SetPlayer(CPlayer* pPlayer) { m_pPlayer = pPlayer; }

protected:
	//��ġ(Batch) ó���� �ϱ� ���Ͽ� ���� ���̴����� ����Ʈ�� ǥ���Ѵ�.
	CObjectsShader **m_ppShaders;
	int m_nShaders = 0;

	LIGHTS						*m_pLights = NULL;

	ID3D12Resource				*m_pd3dcbLights = NULL;
	LIGHTS						*m_pcbMappedLights = NULL;

	MATERIALS					*m_pMaterials = NULL;

	ID3D12Resource				*m_pd3dcbMaterials = NULL;
	MATERIAL					*m_pcbMappedMaterials = NULL;

	ID3D12RootSignature *m_pd3dGraphicsRootSignature = NULL;
	CHeightMapTerrain *m_pTerrain = NULL;

	CPlayer* m_pPlayer;

	CGameObject** m_pSunMoon;

	bool pointLightState;
	int m_nPointLights;

public:
	CHeightMapTerrain *GetTerrain() { return(m_pTerrain); }
	//���� ��� ���� ��ü�鿡 ���� ���콺 ��ŷ�� �����Ѵ�.
	CGameObject *PickObjectPointedByCursor(int xClient, int yClient, CCamera *pCamera);

	void FlashLight();

	void Reset();

private:
	// �÷��̾� ��ġ�� ���� �ܺ� ����, ����Ʈ ����Ʈ�� �Ѱ� ���� ���� �Լ�
	void checkPlayerLocation();

	void turnOnDirectLights();
	void turnOffDirectLights();

	void turnOnPointLights();
	void turnOffPointLights();

	// ��,���� �������� ���� ������Ʈ �� �ֱ� ���� �Լ�
	void directionalLightUpdate();
};