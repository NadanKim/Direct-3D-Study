#include "stdafx.h"
#include "GameObject.h"
#include "Shader.h"


CGameObject::CGameObject()
{
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
}


CGameObject::~CGameObject()
{
	if (m_pMesh) m_pMesh->Release();
	if (m_pShader)
	{
		m_pShader->ReleaseShaderVariables();
		m_pShader->Release();
	}
}


void CGameObject::SetShader(CShader *pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}


void CGameObject::SetMesh(CMesh *pMesh)
{
	if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh;
	if (m_pMesh) m_pMesh->AddRef();
}


void CGameObject::ReleaseUploadBuffers()
{
	//정점 버퍼를 위한 업로드 버퍼를 소멸시킨다.
	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();
}


void CGameObject::Animate(float fTimeElapsed)
{
}


void CGameObject::OnPrepareRender()
{
}


void CGameObject::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	OnPrepareRender();

	//객체의 정보를 셰이더 변수(상수 버퍼)로 복사한다.
	UpdateShaderVariables(pd3dCommandList);

	if (m_pShader) m_pShader->Render(pd3dCommandList, pCamera);

	if (m_pMesh) m_pMesh->Render(pd3dCommandList);
 }


void CGameObject::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera,
	UINT nInstances)
{
	OnPrepareRender();

	if (m_pMesh) m_pMesh->Render(pd3dCommandList, nInstances);
}


void CGameObject::CreateShaderVariables(ID3D12Device *pd3dDevice,
	ID3D12GraphicsCommandList *pd3dCommandList)
{
}


void CGameObject::ReleaseShaderVariables()
{
}


void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
	//객체의 월드 변환 행렬을 루트 상수(32-비트 값)를 통하여 셰이더 변수(상수 버퍼)로 복사한다.
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);
}


void CGameObject::SetTotalSize(float fTotalLength)
{
	m_fTotalLength = fTotalLength;
}



void CGameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;
}


void CGameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}


XMFLOAT3 CGameObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}


//게임 객체의 로컬 z-축 벡터를 반환한다.
XMFLOAT3 CGameObject::GetLook()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32,
		m_xmf4x4World._33)));
}


//게임 객체의 로컬 y-축 벡터를 반환한다.
XMFLOAT3 CGameObject::GetUp()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22,
		m_xmf4x4World._23)));
}


//게임 객체의 로컬 x-축 벡터를 반환한다.
XMFLOAT3 CGameObject::GetRight()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12,
		m_xmf4x4World._13)));
}

//게임 객체를  Direction 방향으로 Speed로 이동한다.
void CGameObject::Move(XMFLOAT3& vDirection, float fSpeed)
{
	SetPosition(m_xmf4x4World._41 + vDirection.x * fSpeed, m_xmf4x4World._42 + vDirection.y * fSpeed, m_xmf4x4World._43 + vDirection.z * fSpeed);
}


//게임 객체를 로컬 x-축 방향으로 이동한다.
void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	CGameObject::SetPosition(xmf3Position);
}


//게임 객체를 로컬 y-축 방향으로 이동한다.
void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	CGameObject::SetPosition(xmf3Position);
}


//게임 객체를 로컬 z-축 방향으로 이동한다.
void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	CGameObject::SetPosition(xmf3Position);
}


void CGameObject::Rotate(XMFLOAT3 *pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis),
		XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}


//게임 객체를 주어진 각도로 회전한다.
void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch),
		XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}


// 바운딩 박스를 설정한다.
void CGameObject::SetOOBB(XMFLOAT3& xmCenter, XMFLOAT3& xmExtents, XMFLOAT4& xmOrientation)
{
	m_xmOOBBTransformed = m_xmOOBB = BoundingOrientedBox(xmCenter, xmExtents, xmOrientation);
}

/////////////////// RotateObject /////////////////////////

CRotatingObject::CRotatingObject()
{
	m_xmf3MovingDirection = XMFLOAT3(0.0f, 0.0f, -1.0f);
	m_fMovingSpeed = 30.0f;

	m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed = 90.0f;
}


CRotatingObject::CRotatingObject(XMFLOAT3 xmf3MoveDirection, float fMoveSpeed,
	XMFLOAT3 xmf3RotateAxis, float fRotateSpeed)
	:m_xmf3MovingDirection(xmf3MoveDirection), m_fMovingSpeed(fMoveSpeed),
	m_xmf3RotationAxis(xmf3RotateAxis), m_fRotationSpeed(fRotateSpeed)
{

}


CRotatingObject::~CRotatingObject()
{
}


void CRotatingObject::Reset(float curPlayerZ, float prePlayerZ)
{
	static uniform_int_distribution<> caseDst(1, 9);
	static uniform_real_distribution<float>normalDst(0.0f, 1.0f);
	static uniform_real_distribution<float>posDst(-45.0f, 45.0f);
	static uniform_real_distribution<float>speedDst(3.0f, 25.0f);
	static default_random_engine dre((unsigned)time(NULL));

	float playerZDiff = curPlayerZ - prePlayerZ;

	int kind = caseDst(dre);
	switch (kind)
	{
		// 가로
	case 1:
	case 2:
		m_dDirection = Directions::DX;
		SetMovingDirection(XMFLOAT3((kind % 2 == 0) ? 1.0f : -1.0f, 0.0f, 0.0f));
		SetDirectionKind(Directions::DX);
		break;
		// 세로
	case 3:
	case 4:
		m_dDirection = Directions::DY;
		SetMovingDirection(XMFLOAT3(0.0f, (kind % 2 == 0) ? 1.0f : -1.0f, 0.0f));
		SetDirectionKind(Directions::DY);
		break;
	default:
		m_dDirection = Directions::DRANDOM;
		color = XMFLOAT4(normalDst(dre), normalDst(dre), normalDst(dre), 1.0f);
		SetMovingDirection(XMFLOAT3(normalDst(dre), normalDst(dre), (playerZDiff>0)?-1.0f:1.0f));
		SetDirectionKind(Directions::DRANDOM);
		break;
	}
	SetPosition(posDst(dre), posDst(dre), curPlayerZ + m_fTotalLength * ((playerZDiff>0) ? 1.0f : -1.0f) );
	SetRotationAxis(XMFLOAT3(normalDst(dre), normalDst(dre), normalDst(dre)));
	SetMovingSpeed(speedDst(dre));
	SetRotationSpeed(speedDst(dre));

	m_bRender = true;
}


void CRotatingObject::FarChecker(float curPlayerZ, float prePlayerZ)
{
	float playerZDiff = curPlayerZ - prePlayerZ;

	if (playerZDiff > 0.0f && GetPosition().z < curPlayerZ - m_fTotalLength ||
		playerZDiff <= 0.0f && GetPosition().z > curPlayerZ + m_fTotalLength)
	{
		Reset(curPlayerZ, prePlayerZ);
	}
}


void CRotatingObject::Animate(float fTimeElapsed)
{
	if (m_bRender == false)	return;
	CGameObject::Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);
	CGameObject::Move(m_xmf3MovingDirection, m_fMovingSpeed * fTimeElapsed);
	m_xmOOBBTransformed.Transform(m_xmOOBB, XMLoadFloat4x4(&m_xmf4x4World));
	XMStoreFloat4(&m_xmOOBBTransformed.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBBTransformed.Orientation)));
}

///////////////////////// Bullet Object //////////////////////////////////////////////

CBulletObject::CBulletObject()
{
}

CBulletObject::~CBulletObject() {}

void CBulletObject::Reset()
{
	SetColor(XMFLOAT4(1.0f, 0.99f, 0.5f, 1.0f));
	SetMovingSpeed(170.0f);
	SetRotationAxis(XMFLOAT3(1.0f, 1.0f, 1.0f));
	SetRotationSpeed(360.0f);
	SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.5f, 2.5f, 1.5f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

void CBulletObject::OnPrepareRender()
{
}

void CBulletObject::FarChecker(XMFLOAT3 playerPos)
{
	XMFLOAT3 bulletPos = GetPosition();
	XMStoreFloat3(&bulletPos, XMVector3LengthEst(XMLoadFloat3(&bulletPos) - XMLoadFloat3(&playerPos)));
	if (bulletPos.x > m_fTotalLength)
		m_bRender = false;
}

void CBulletObject::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	if(m_bRender) CGameObject::Render(pd3dCommandList, pCamera);
}

/////////////////////  Wall Object ///////////////////////////

CWallObject::CWallObject(){}

CWallObject::~CWallObject() {}

void CWallObject::Animate(XMFLOAT3* playerPos)
{
	if (m_xmf4x4World._43 <= playerPos->z - m_fTotalLength) {
		m_xmf4x4World._43 += 2 * m_fTotalLength;
	}
	else if (m_xmf4x4World._43 >=  playerPos->z + m_fTotalLength) {
		m_xmf4x4World._43 -= 2 * m_fTotalLength;
	}
}

void CWallObject::SetTotalSize(float fTotalLength)
{
	m_fTotalLength = fTotalLength;
}


///////////////////////////////// Particle Object ///////////////////////////////////////


CParticleObject::CParticleObject(CGameObject& data)
{
	static uniform_int_distribution<> caseDst(1, 9);
	static uniform_real_distribution<float>normalDst(-1.0f, 1.0f);
	static uniform_real_distribution<float>speedDst(30.0f, 360.0f);
	static default_random_engine dre((unsigned)time(NULL));

	begPos = data.GetPosition();
	Directions direct = data.GetDirectionKind();
	XMFLOAT4 color(0.0f, 0.0f, 0.0f, 0.0f);
	if (direct == Directions::DRANDOM)
		color = data.GetColor();

	for (int i = 0; i < PARTICLE_COUNT; ++i)
	{
		particles[i].SetDirectionKind(direct);
		particles[i].SetColor(color);
		particles[i].SetPosition(begPos);
		particles[i].SetMovingDirection(XMFLOAT3(normalDst(dre), normalDst(dre), normalDst(dre)));
		particles[i].SetMovingSpeed(70.0f);
		particles[i].SetRotationAxis(XMFLOAT3(normalDst(dre), normalDst(dre), normalDst(dre)));
		particles[i].SetRotationSpeed(speedDst(dre));
	}
}


CParticleObject::~CParticleObject()
{
}

void CParticleObject::Animate(float fTimeElapsed)
{
	for (int i = 0; i < PARTICLE_COUNT; ++i) {
		particles[i].Animate(fTimeElapsed);
	}
}


bool CParticleObject::FarChecker()
{
	XMFLOAT3 curPos = particles[0].GetPosition();
	XMStoreFloat3(&curPos, XMVector3LengthEst(XMLoadFloat3(&curPos) - XMLoadFloat3(&begPos)));

	if (curPos.x > PARTICLE_RANGE)
		m_bRender = false;
	return m_bRender;
}


XMFLOAT4X4 CParticleObject::GetParticleWorld(int n) {
	return particles[n].m_xmf4x4World;
}