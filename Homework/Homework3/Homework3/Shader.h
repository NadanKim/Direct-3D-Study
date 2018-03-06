#pragma once
#include "GameObject.h"
#include "Camera.h"
#include "Player.h"

//인스턴스 정보(게임 객체의 월드 변환 행렬과 객체의 색상)를 위한 구조체이다.
struct VS_VB_INSTANCE
{
	XMFLOAT4X4 m_xmf4x4Transform;
	XMFLOAT4 m_xmcColor;
};

enum MAP{	WALL = 1, ENEMY };

//게임 객체의 정보를 셰이더에게 넘겨주기 위한 구조체(상수 버퍼)이다.
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


//“CObjectsShader” 클래스는 게임 객체들을 포함하는 셰이더 객체이다.
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

	//셰이더에 포함되어 있는 모든 게임 객체들에 대한 마우스 픽킹을 수행한다.
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
	//인스턴스 데이터를 포함하는 버퍼와 포인터이다.
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

// 총알은 최대 20발 까지 발사가능 한번에 화면에 출력될 수 있는 한계치
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

// 최대 동시에 터지는 오브젝트 수 : 5개, 파티클의 구성 200개( 5 * 200 = 1000 )
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