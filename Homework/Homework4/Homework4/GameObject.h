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
	// �ٿ�� �ڽ�
	BoundingOrientedBox		m_xmOOBB;
	BoundingOrientedBox		m_xmOOBBTransformed;

	int m_nMaterial;

	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorHandle;

protected:
	//���� ��ü�� ���� ���� �޽��� �����ϴ� ��� ���� ��ü�� ������ �޽��鿡 ���� �����Ϳ� �� �����̴�.
	CMesh **m_ppMeshes = NULL;
	CMesh *m_pMesh = NULL;
	int m_nMeshes = 0;
	CShader *m_pShader = NULL;
	bool m_bRender = true;

	XMFLOAT3 m_xmf3Velocity;
	XMFLOAT3 m_xmf3RealVelocity;

	ObjType m_objectType;

	//������Ʈ�� ��ġ�� �ٲ� ������ ȣ��Ǵ� OnObjUpdateCallback() �Լ����� ����ϴ� �������̴�.
	LPVOID m_pObjUpdatedContext;

	// ������Ʈ�� �߷� �����ϱ� ���� ���� ����
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
	//�� ��ǥ���� ��ŷ ������ �����Ѵ�.
	void GenerateRayForPicking(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View,
		XMFLOAT3 *pxmf3PickRayOrigin, XMFLOAT3 *pxmf3PickRayDirection);
	//ī�޶� ��ǥ���� �� ���� ���� �� ��ǥ���� ��ŷ ������ �����ϰ� ��ü���� ������ �˻��Ѵ�.
	int PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View,
		float *pfHitDistance, CMesh* myMesh = NULL);
	// �� ��ǥ���� ������ �����Ѵ�. -> Collision �ϱ����� ���
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

	//���� ��ü�� ���� ��ȯ ��Ŀ��� ��ġ ���Ϳ� ����(x-��, y-��, z-��) ���͸� ��ȯ�Ѵ�.
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	//���� ��ü�� ��ġ�� �����Ѵ�.
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);
	void SetVelocity(XMFLOAT3& xmf3Velocity) { m_xmf3Velocity = xmf3Velocity; }

	//���� ��ü�� Direction �������� Speed �ӵ��� �̵��Ѵ�.
	void Move(XMFLOAT3& vDirection, float fSpeed);
	//���� ��ü�� ���� x-��, y-��, z-�� �������� �̵��Ѵ�.
	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	//���� ��ü�� ȸ��(x-��, y-��, z-��)�Ѵ�.
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);

	virtual void SetOOBB(XMFLOAT3& xmCenter, XMFLOAT3& xmExtents, XMFLOAT4& xmOrientation);

	//���� ��ü�� ī�޶� ���δ� ���� �˻��Ѵ�.
	virtual bool IsVisible(CCamera *pCamera = NULL);

public:
	//��� ���۸� �����Ѵ�.
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList);
	//��� ������ ������ �����Ѵ�.
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
	//������ ���� ������ ����� �̹����̴�.
	CHeightMapImage *m_pHeightMapImage;

	//���� ���� ���ο� ���� ũ���̴�.
	int m_nWidth;
	int m_nLength;

	//������ ������ �� �� Ȯ���� ���ΰ��� ��Ÿ���� ������ �����̴�.
	XMFLOAT3 m_xmf3Scale;

public:
	//������ ���̸� ����ϴ� �Լ��̴�(���� ��ǥ��). ���� ���� ���̿� �������� y�� ���� ���̴�.
	float GetHeight(float x, float z) {
		return(m_pHeightMapImage->GetHeight(x / m_xmf3Scale.x, z / m_xmf3Scale.z) * m_xmf3Scale.y);
	}
	//������ ���� ���͸� ����ϴ� �Լ��̴�(���� ��ǥ��). ���� ���� ���� ���͸� ����Ѵ�.
	XMFLOAT3 GetNormal(float x, float z) {
		return(m_pHeightMapImage->GetHeightMapNormal(int(x / m_xmf3Scale.x), int(z / m_xmf3Scale.z)));
	}

	int GetHeightMapWidth() { return(m_pHeightMapImage->GetHeightMapWidth()); }
	int GetHeightMapLength() { return(m_pHeightMapImage->GetHeightMapLength()); }

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	//������ ũ��(����/����)�� ��ȯ�Ѵ�. ���� ���� ũ�⿡ �������� ���� ���̴�.
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


