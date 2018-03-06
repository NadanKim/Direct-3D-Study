#pragma once
#include "GameObject.h"
#include "Camera.h"
#include "Player.h"

//�ν��Ͻ� ����(���� ��ü�� ���� ��ȯ ��İ� ��ü�� ����)�� ���� ����ü�̴�.
struct VS_VB_INSTANCE
{
	XMFLOAT4X4 m_xmf4x4Transform;
	XMFLOAT4 m_xmcColor;
};

enum MAP{	WALL = 1, ENEMY };

//���� ��ü�� ������ ���̴����� �Ѱ��ֱ� ���� ����ü(��� ����)�̴�.
struct CB_GAMEOBJECT_INFO
{
	XMFLOAT4X4 m_xmf4x4World;
};

class CShader
{
public:
	CShader();
	virtual ~CShader();

private:
	int m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
	D3D12_SHADER_BYTECODE CompileShaderFromFile(WCHAR *pszFileName, LPCSTR pszShaderName,
		LPCSTR pszShaderProfile, ID3DBlob **ppd3dShaderBlob);

	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature
		*pd3dGraphicsRootSignature);

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList,
		XMFLOAT4X4 *pxmf4x4World);

	virtual void OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);

protected:
	ID3D12PipelineState **m_ppd3dPipelineStates = NULL;
	int m_nPipelineStates = 0;
};


class CPlayerShader : public CShader
{
public:
	CPlayerShader();
	virtual ~CPlayerShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);

	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature
		*pd3dGraphicsRootSignature);
};


//��CObjectsShader�� Ŭ������ ���� ��ü���� �����ϴ� ���̴� ��ü�̴�.
class CObjectsShader : public CShader
{
public:
	CObjectsShader();
	virtual ~CObjectsShader();

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList, void *pContext, int nFileNumber = 1);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void ReleaseObjects();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);

	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature
		*pd3dGraphicsRootSignature);

	virtual void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);

	//���̴��� ���ԵǾ� �ִ� ��� ���� ��ü�鿡 ���� ���콺 ��ŷ�� �����Ѵ�.
	virtual CGameObject *PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition,
		XMFLOAT4X4& xmf4x4View, float *pfNearHitDistance);

	virtual void CollisionCheck(CPlayer *pPlayer) {}
	virtual void CollisionCheck(CGameObject *pObject, bool Bullet = false) {}

	virtual void SetPlayerPos(XMFLOAT3 &playerPos) {}

	virtual CGameObject** GetObjects() { return m_ppObjects; }
	virtual int GetObjectsSize() { return m_nObjects; }
	virtual void SetBullet(CPlayer *pPlayer) {}
	virtual void SetParticle(CGameObject *pObject) {}
	virtual void SetParticleShader(CObjectsShader *pParticles) { }

protected:
	CGameObject **m_ppObjects = NULL;
	int m_nObjects = 0;
};


////////////////////////////////// Terrain Shader /////////////////////////////////////////

class CTerrainShader : public CShader
{
public:
	CTerrainShader();
	virtual ~CTerrainShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);

	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature
		*pd3dGraphicsRootSignature);
};


//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// Miro Shader //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

class CMiroShader : public CObjectsShader
{
public:
	CMiroShader();
	virtual ~CMiroShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);

	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature
		*pd3dGraphicsRootSignature);

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList, void *pContext, int nFileNumber = 1);

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);

	virtual void CollisionCheck(CPlayer *pPlayer);
	virtual void CollisionCheck(CGameObject *pObject, bool Bullet = false);

protected:
	//�ν��Ͻ� �����͸� �����ϴ� ���ۿ� �������̴�.
	ID3D12Resource *m_pd3dcbGameObjects = NULL;
	VS_VB_INSTANCE *m_pcbMappedGameObjects = NULL;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// Roof Shader //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

class CRoofShader : public CObjectsShader
{
public:
	CRoofShader() {}
	virtual ~CRoofShader() {}

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList, void *pContext, int nFileNumber = 1);
	virtual void ReleaseObjects();

	virtual void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);

private:
	CGameObject *Roof;
};



//////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// Enemy Shader //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

class CEnemyShader : public CMiroShader
{
protected:
	XMFLOAT3 m_xmf3PlayerPos;
	CObjectsShader* m_pParticles;
public:
	CEnemyShader();
	virtual ~CEnemyShader();

	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList, void *pContext, int nFileNumber = 1);

	virtual void CollisionCheck(CPlayer *pPlayer);
	virtual void CollisionCheck(CGameObject *pObject, bool Bullet = true);

	virtual void AnimateObjects(float fTimeElapsed);


	virtual void SetPlayerPos(XMFLOAT3 &playerPos) { m_xmf3PlayerPos = playerPos; }

	virtual void SetParticleShader(CObjectsShader *pParticles) { m_pParticles = pParticles; }
};


/////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// Bullet Shader ///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

// �Ѿ��� �ִ� 20�� ���� �߻簡�� �ѹ��� ȭ�鿡 ��µ� �� �ִ� �Ѱ�ġ
#define BULLET_MAX 20

class CBulletShader : public CMiroShader
{
private:
	int m_nCurrentBullet = 0;
public:
	CBulletShader();
	virtual ~CBulletShader();

	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList, void *pContext, int nFileNumber = 1);

	virtual void SetBullet(CPlayer *pPlayer);
};


/////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// Particle Shader /////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

// �ִ� ���ÿ� ������ ������Ʈ �� : 5��, ��ƼŬ�� ���� 200��( 5 * 200 = 1000 )
#define PARTICLE_MAX 5

class CParticleShader : public CMiroShader
{
private:
	int m_nCurrentParticle = 0;
	CBulletObject ParticleRenderer;
public:
	CParticleShader();
	virtual ~CParticleShader();

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList, void *pContext, int nFileNumber = 1);

	virtual void SetParticle(CGameObject *pObject);

	void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
};