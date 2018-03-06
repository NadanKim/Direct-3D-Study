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

struct CB_PLAYER_INFO
{
	XMFLOAT4X4						m_xmf4x4World;
	UINT							m_nMaterial;
};

struct CB_GAMEOBJECT_INFO
{
	XMFLOAT4X4						m_xmf4x4World;
	UINT							m_nMaterial;
};

enum MAP { WALL = 1, Map_Light };

class CShader
{
public:
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
		XMFLOAT4X4 *pxmf4x4World, UINT nMaterial);

	virtual void OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);

protected:
	ID3D12PipelineState **m_ppd3dPipelineStates = NULL;
	int m_nPipelineStates = 0;

	ID3D12DescriptorHeap			*m_pd3dCbvSrvDescriptorHeap = NULL;

	ID3D12Resource					*m_pd3dcbGameObjects = NULL;
	UINT8							*m_pcbMappedGameObjects = NULL;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

class CPlayerShader : public CShader
{
public:

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);

	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature
		*pd3dGraphicsRootSignature);

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, 
		XMFLOAT4X4 *pxmf4x4World, UINT nMaterial);

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);

protected:
	ID3D12Resource					*m_pd3dcbPlayer = NULL;
	CB_PLAYER_INFO					*m_pcbMappedPlayer = NULL;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

//“CObjectsShader” 클래스는 게임 객체들을 포함하는 셰이더 객체이다.
class CObjectsShader : public CShader
{
public:

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList, void *pContext = NULL, int nFileNumber = 1);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void ReleaseObjects();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);

	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature
		*pd3dGraphicsRootSignature);

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);

	//셰이더에 포함되어 있는 모든 게임 객체들에 대한 마우스 픽킹을 수행한다.
	virtual CGameObject *PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition,
		XMFLOAT4X4& xmf4x4View, float *pfNearHitDistance);

	virtual void CollisionCheck() {}
	virtual void CollisionCheck(CGameObject *pObject) {}
	virtual void CollisionCheck(CPlayer *pPlayer) {}

	virtual void SetPlayerPos(XMFLOAT3 &playerPos) {}

	virtual CGameObject** GetObjects() { return m_ppObjects; }
	virtual int GetObjectsSize() { return m_nObjects; }

protected:
	CGameObject **m_ppObjects = NULL;
	int m_nObjects = 0;

public:
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle() {
		return m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart(); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle() {
		return m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();}
};


////////////////////////////////// Terrain Shader /////////////////////////////////////////

class CTerrainShader : public CObjectsShader
{
public:
	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList, void *pContext = NULL, int nFileNumber = 1);
	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT4X4
		*pxmf4x4World, UINT nMaterial);
};


//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// Miro Shader //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

class CMiroShader : public CObjectsShader
{
public:
	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList, void *pContext, int nFileNumber = 1);

	virtual void CollisionCheck(CPlayer *pPlayer);
	virtual void CollisionCheck(CGameObject *pObject);
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

};



//////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// Enemy Shader //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

class CEnemyShader : public CObjectsShader
{
protected:
	XMFLOAT3 m_xmf3PlayerPos;
	CObjectsShader* m_pParticles;
public:
	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList, void *pContext, int nFileNumber = 1);

	virtual void CollisionCheck();
	virtual void CollisionCheck(CPlayer *pPlayer);

	virtual void AnimateObjects(float fTimeElapsed);

	virtual void SetPlayerPos(XMFLOAT3 &playerPos) { m_xmf3PlayerPos = playerPos; }

	virtual void SetParticleShader(CObjectsShader *pParticles) { m_pParticles = pParticles; }
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
class CDayCycleShader : public CObjectsShader
{
public:
	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList, void *pContext, int nFileNumber = 1);
};