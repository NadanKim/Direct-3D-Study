#pragma once
#include "Mesh.h"
#include "Camera.h"

class CShader;

enum class Directions
{
	DX,
	DY,
	DRANDOM
};

class CGameObject
{
public:
	CGameObject();
	virtual ~CGameObject();

private:
	int m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

protected:
	CMesh *m_pMesh = NULL;
	CShader *m_pShader = NULL;
	float m_fTotalLength;

public:
	XMFLOAT4X4 m_xmf4x4World;
	// �ٿ�� �ڽ�
	BoundingOrientedBox		m_xmOOBB;
	BoundingOrientedBox		m_xmOOBBTransformed;

	bool m_bRender = true;

public:
	void ReleaseUploadBuffers();
	virtual void SetMesh(CMesh *pMesh);
	virtual void SetShader(CShader *pShader);
	virtual void Animate(float fTimeElapsed);
	virtual void OnPrepareRender();
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, UINT
		nInstances);

public:
	//���� ��ü�� ���� ��ȯ ��Ŀ��� ��ġ ���Ϳ� ����(x-��, y-��, z-��) ���͸� ��ȯ�Ѵ�.
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	//���� ��ü�� ��ġ�� �����Ѵ�.
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);

	void SetTotalSize(float fTotalSize);

	//���� ��ü�� Direction �������� Speed �ӵ��� �̵��Ѵ�.
	void Move(XMFLOAT3& vDirection, float fSpeed);
	//���� ��ü�� ���� x-��, y-��, z-�� �������� �̵��Ѵ�.
	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	//���� ��ü�� ȸ��(x-��, y-��, z-��)�Ѵ�.
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(XMFLOAT3 *pxmf3Axis, float fAngle);

	// �ٿ�� �ڽ� ����
	void SetOOBB(XMFLOAT3& xmCenter, XMFLOAT3& xmExtents, XMFLOAT4& xmOrientation);

public:
	//��� ���۸� �����Ѵ�.
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList);
	//��� ������ ������ �����Ѵ�.
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();	
	virtual void SetDirectionKind(Directions kind) {  }
	virtual Directions GetDirectionKind() { return Directions::DRANDOM; }
	virtual void SetColor(XMFLOAT4 newColor) { };
	virtual XMFLOAT4 GetColor() { return XMFLOAT4(0.0f, 0.7f, 0.0f, 0.0f); }
	virtual XMFLOAT4X4 GetParticleWorld(int n) { return m_xmf4x4World; }
};

//////////////////////////////////////////////////////////////////////

class CRotatingObject : public CGameObject
{
public:
	CRotatingObject();
	CRotatingObject(XMFLOAT3 xmf3MoveDirection, float fMoveSpeed,
		XMFLOAT3 xmf3RotateAxis, float fRotateSpeed);
	virtual ~CRotatingObject();

private:
	XMFLOAT3				m_xmf3MovingDirection;
	float							m_fMovingSpeed;

	XMFLOAT3 m_xmf3RotationAxis;
	float m_fRotationSpeed;
	
	Directions		m_dDirection;

	XMFLOAT4		color;

public:
	void SetMovingDirection(XMFLOAT3& xmf3MovingDirection) { m_xmf3MovingDirection = Vector3::Normalize(xmf3MovingDirection); }
	void SetMovingSpeed(float fSpeed) { m_fMovingSpeed = fSpeed; }

	XMFLOAT3 GetMovingDirection() { return m_xmf3MovingDirection; }
	float GetMovingSpeed() { return m_fMovingSpeed; }

	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) {
		m_xmf3RotationAxis = xmf3RotationAxis;
	}

	virtual void SetDirectionKind(Directions kind) {	m_dDirection = kind; }
	virtual Directions GetDirectionKind() { return m_dDirection; }

	virtual void SetColor(XMFLOAT4 newColor) { color = newColor; };
	virtual XMFLOAT4 GetColor() { return color; }

	virtual void Reset(float curPlayerZ = 0.0f, float prePlayerZ = 0.0f);
	virtual void FarChecker(float curPlayerZ = 0.0f, float prePlayerZ = 0.0f);

	virtual void Animate(float fTimeElapsed);
};

///////////////////////////////////////////////////////////////////////////////////

class CBulletObject : public CRotatingObject
{
public:
	CBulletObject();
	virtual ~CBulletObject();
	virtual void Reset();
	virtual void OnPrepareRender();
	virtual void FarChecker(XMFLOAT3 playerPos);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
};

//////////////////////////////////////////////////////////////////////

class CWallObject : public CGameObject
{
public:
	CWallObject();
	virtual ~CWallObject();

private:
	float m_fTotalLength;

public:
	virtual void Animate(XMFLOAT3* playerPos);
	void SetTotalSize(float fTotalSize);
};


/////////////////////////////////////////////////////////////////////////////////
#define PARTICLE_COUNT 200
#define PARTICLE_RANGE 200

class CParticleObject : public CRotatingObject
{
public:
	CParticleObject() {}
	CParticleObject(CGameObject& data);
	~CParticleObject();
	
private:
	CRotatingObject particles[PARTICLE_COUNT];
	XMFLOAT3 begPos;

public:
	virtual void Animate(float fTimeElapsed);
	virtual bool FarChecker();
	virtual XMFLOAT4X4 GetParticleWorld(int n);
	virtual Directions GetDirectionKind(int n) { return  particles[n].GetDirectionKind(); }
	virtual XMFLOAT4 GetColor(int n = 0) { return particles[n].GetColor(); }
};