#pragma once
#include "Mesh.h"
#include "Camera.h"

class CShader;

enum class ObjType
{
	WALL,
	ROOF,
	ENEMY
};

struct MATERIAL
{
	XMFLOAT4						m_xmf4Ambient;
	XMFLOAT4						m_xmf4Diffuse;
	XMFLOAT4						m_xmf4Specular; //(r,g,b,a=power)
	XMFLOAT4						m_xmf4Emissive;
};

class CMaterial
{
public:
	CMaterial();
	virtual ~CMaterial();

private:
	int								m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	XMFLOAT4						m_xmf4Albedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	UINT							m_nReflection = 0;
	CShader							*m_pShader = NULL;

	void SetAlbedo(XMFLOAT4& xmf4Albedo) { m_xmf4Albedo = xmf4Albedo; }
	void SetReflection(UINT nReflection) { m_nReflection = nReflection; }
	void SetShader(CShader *pShader);
};

class CGameObject
{
public:
	CGameObject(int nMeshes = 1);
	virtual ~CGameObject();

private:
	int m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

public :
	XMFLOAT4X4 m_xmf4x4World;
	// 바운딩 박스
	BoundingOrientedBox		m_xmOOBB;
	BoundingOrientedBox		m_xmOOBBTransformed;

	int m_nMaterial;

	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorHandle;

protected:
	//게임 객체는 여러 개의 메쉬를 포함하는 경우 게임 객체가 가지는 메쉬들에 대한 포인터와 그 개수이다.
	CMesh **m_ppMeshes = NULL;
	CMesh *m_pMesh = NULL;
	int m_nMeshes = 0;
	CShader *m_pShader = NULL;
	bool m_bRender = true;

	XMFLOAT3 m_xmf3Velocity;
	XMFLOAT3 m_xmf3RealVelocity;

	ObjType m_objectType;

	//오브젝트의 위치가 바뀔 때마다 호출되는 OnObjUpdateCallback() 함수에서 사용하는 데이터이다.
	LPVOID m_pObjUpdatedContext;

	// 오브젝트에 중력 적용하기 위한 방향 벡터
	XMFLOAT3 m_xmf3Gravity;

public:
	void ReleaseUploadBuffers();
	virtual void SetMesh(int nIndex, CMesh *pMesh);
	virtual void SetMesh(CMesh *pMesh);
	virtual void SetShader(CShader *pShader);
	virtual void Animate(float fTimeElapsed, XMFLOAT3 *xmf3PlayerPos){}
	virtual void Animate(float fTimeElapsed);
	virtual void OnPrepareRender();
	virtual void OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, UINT
		nInstances);
	//모델 좌표계의 픽킹 광선을 생성한다.
	void GenerateRayForPicking(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View,
		XMFLOAT3 *pxmf3PickRayOrigin, XMFLOAT3 *pxmf3PickRayDirection);
	//카메라 좌표계의 한 점에 대한 모델 좌표계의 픽킹 광선을 생성하고 객체와의 교차를 검사한다.
	int PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View,
		float *pfHitDistance, CMesh* myMesh = NULL);
	// 모델 좌표계의 광선을 생성한다. -> Collision 하기위해 사용
	void GenerateRayForCollision(XMFLOAT3& xmf3ColliderPosition,
		XMFLOAT3 *pxmf3PickRayOrigin, XMFLOAT3 *pxmf3PickRayDirection);
	XMFLOAT3 CollisionByRayIntersection(XMFLOAT3& xmf3ColliderPosition, CMesh* myMesh = NULL);

	void SetGravity(XMFLOAT3& xmf3Gravity) { m_xmf3Gravity = xmf3Gravity; }
	virtual void SetActive(bool bState) { m_bRender = bState; }
	bool GetActive() { return m_bRender; }

	CMesh* GetMesh() { return m_pMesh; }

	void SetObjUpdatedContext(LPVOID pContext) { m_pObjUpdatedContext = pContext; }

	void SetCbvGPUDescriptorHandle(UINT64 nCbvGPUDescriptorHandlePtr) { m_d3dCbvGPUDescriptorHandle.ptr = nCbvGPUDescriptorHandlePtr; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetCbvGPUDescriptorHandle() { return(m_d3dCbvGPUDescriptorHandle); }

public:
	void Rotate(XMFLOAT3 *pxmf3Axis, float fAngle);

	//게임 객체의 월드 변환 행렬에서 위치 벡터와 방향(x-축, y-축, z-축) 벡터를 반환한다.
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	//게임 객체의 위치를 설정한다.
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);
	void SetVelocity(XMFLOAT3& xmf3Velocity) { m_xmf3Velocity = xmf3Velocity; }

	//게임 객체를 Direction 방향으로 Speed 속도로 이동한다.
	void Move(XMFLOAT3& vDirection, float fSpeed);
	//게임 객체를 로컬 x-축, y-축, z-축 방향으로 이동한다.
	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	//게임 객체를 회전(x-축, y-축, z-축)한다.
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);

	virtual void SetOOBB(XMFLOAT3& xmCenter, XMFLOAT3& xmExtents, XMFLOAT4& xmOrientation);

	//게임 객체가 카메라에 보인는 가를 검사한다.
	virtual bool IsVisible(CCamera *pCamera = NULL);

public:
	//상수 버퍼를 생성한다.
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList);
	//상수 버퍼의 내용을 갱신한다.
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();	

	virtual XMFLOAT3& GetVelocity() { return m_xmf3Velocity; }
	virtual XMFLOAT3& GetRealVelocity() {	return m_xmf3RealVelocity;}
	virtual bool IsMove() { return true; }
	virtual void SetDirection(XMFLOAT3 xmf3Direction) {}
	virtual void SetParticles() {}
	virtual XMFLOAT4X4 GetWorldTransform(int n) { return m_xmf4x4World; }
	virtual void OnObjUpdateCallback() {}

	virtual void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) {	}
	virtual XMFLOAT3& GetRotationoAxis() { return XMFLOAT3(); }

	void SetObjectType(ObjType objectType) { m_objectType = objectType; }
	ObjType GetObjectType() { return m_objectType; }
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

class CRotatingObject : public CGameObject
{
public:
	CRotatingObject(int nMeshes = 1);
	virtual ~CRotatingObject();

protected:
	XMFLOAT3 m_xmf3RotationAxis;
	float m_fRotationSpeed;

public:
	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	virtual void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) {
		m_xmf3RotationAxis = xmf3RotationAxis;
	}
	virtual XMFLOAT3& GetRotationoAxis() { return m_xmf3RotationAxis; }

	virtual void Animate(float fTimeElapsed, XMFLOAT3 *xmf3PlayerPos=nullptr);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

class CHeightMapTerrain : public CGameObject
{
public:
	CHeightMapTerrain(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, LPCTSTR pFileName, int
		nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale);
	virtual ~CHeightMapTerrain();

private:
	//지형의 높이 맵으로 사용할 이미지이다.
	CHeightMapImage *m_pHeightMapImage;

	//높이 맵의 가로와 세로 크기이다.
	int m_nWidth;
	int m_nLength;

	//지형을 실제로 몇 배 확대할 것인가를 나타내는 스케일 벡터이다.
	XMFLOAT3 m_xmf3Scale;

public:
	//지형의 높이를 계산하는 함수이다(월드 좌표계). 높이 맵의 높이에 스케일의 y를 곱한 값이다.
	float GetHeight(float x, float z) {
		return(m_pHeightMapImage->GetHeight(x / m_xmf3Scale.x, z / m_xmf3Scale.z) * m_xmf3Scale.y);
	}
	//지형의 법선 벡터를 계산하는 함수이다(월드 좌표계). 높이 맵의 법선 벡터를 사용한다.
	XMFLOAT3 GetNormal(float x, float z) {
		return(m_pHeightMapImage->GetHeightMapNormal(int(x / m_xmf3Scale.x), int(z / m_xmf3Scale.z)));
	}

	int GetHeightMapWidth() { return(m_pHeightMapImage->GetHeightMapWidth()); }
	int GetHeightMapLength() { return(m_pHeightMapImage->GetHeightMapLength()); }

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	//지형의 크기(가로/세로)를 반환한다. 높이 맵의 크기에 스케일을 곱한 값이다.
	float GetWidth() { return(m_nWidth * m_xmf3Scale.x); }
	float GetLength() { return(m_nLength * m_xmf3Scale.z); }
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

class CEnemyObject : public CRotatingObject
{
public:
	CEnemyObject(float fRadius = 12.5, int nMeshes = 1);
	virtual ~CEnemyObject();

private:
	float m_fSearchRange = 120.0f;
	float m_fMoveSpeed = 0.0f;
	bool m_bMove = false;

public:
	virtual void Animate(float fTimeElapsed, XMFLOAT3 *xmf3PlayerPos = nullptr);
	virtual bool IsMove() { return m_bMove; }
	virtual void OnObjUpdateCallback();
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

class CBulletObject : public CRotatingObject
{
public:
	CBulletObject(int nMeshes = 1);
	virtual ~CBulletObject();

private:
	XMFLOAT3 m_xmf3BegPos;
	float m_fActiveRange = 800.0f;
	XMFLOAT3 m_xmf3MoveDirection;
	float m_fMoveSpeed = 180.0f;

public:
	virtual void Animate(float fTimeElapsed);
	virtual void SetDirection(XMFLOAT3 xmf3Direction);
	void SetActiveRange(float fActiveRange) { m_fActiveRange = fActiveRange; }
	void SetMoveSpeed(float fMoveSpeed) { m_fMoveSpeed = fMoveSpeed; }
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
#define PARTICLE_COUNT 200

class CParticleObject : public CRotatingObject
{
public:
	CParticleObject(int nMeshes = 1);
	virtual ~CParticleObject();
	
private:
	CBulletObject particles[PARTICLE_COUNT];
	XMFLOAT3 m_xmf3MoveDirection;

	float m_fActiveRange = 150.0f;
	float m_fMoveSpeed = 120.0f;

public:
	virtual void Animate(float fTimeElapsed);
	virtual void SetParticles();
	virtual XMFLOAT4X4 GetWorldTransform(int n) { return particles[n].m_xmf4x4World; }
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
class CRevolvingObject : public CGameObject
{
public:
	CRevolvingObject();
	virtual ~CRevolvingObject();

protected:
	XMFLOAT3					m_xmf3RevolutionAxis;
	float						m_fRevolutionSpeed;

public:
	void SetRevolutionSpeed(float fRevolutionSpeed) { m_fRevolutionSpeed = fRevolutionSpeed; }
	void SetRevolutionAxis(XMFLOAT3 xmf3RevolutionAxis) { m_xmf3RevolutionAxis = xmf3RevolutionAxis; }

	virtual void Animate(float fTimeElapsed);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
class COrbitObject : public CRevolvingObject
{
protected:
	float m_iCenter;
	float m_iRadius;
	float m_fDegree;

public:
	void SetCenter(float center) { m_iCenter = center; }
	void SetDegree(float degree) { m_fDegree = degree; }
	void SetRadius(float radius) { m_iRadius = radius; }

	virtual bool IsVisible(CCamera *pCamera = NULL);

	virtual void Animate(float fTimeElapsed);

public:
	void setRenderState();
};


