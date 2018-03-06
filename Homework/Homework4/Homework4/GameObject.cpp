#include "stdafx.h"
#include "GameObject.h"
#include "Shader.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CMaterial::CMaterial()
{
	m_xmf4Albedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
}

CMaterial::~CMaterial()
{
	if (m_pShader)
	{
		m_pShader->ReleaseShaderVariables();
		m_pShader->Release();
	}
}

void CMaterial::SetShader(CShader *pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CGameObject::CGameObject(int nMeshes)
{
	m_xmf4x4World = Matrix4x4::Identity();

	m_nMeshes = nMeshes;
	m_ppMeshes = NULL;
	if (m_nMeshes > 0)
	{
		m_ppMeshes = new CMesh*[m_nMeshes];
		for (int i = 0; i < m_nMeshes; i++) m_ppMeshes[i] = NULL;
	}
}


CGameObject::~CGameObject()
{
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Release();
			m_ppMeshes[i] = NULL;
		}
		delete[] m_ppMeshes;
	}

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


void CGameObject::SetMesh(int nIndex, CMesh *pMesh)
{
	if (m_ppMeshes)
	{
		if (m_ppMeshes[nIndex]) m_ppMeshes[nIndex]->Release();
		m_ppMeshes[nIndex] = pMesh;
		if (pMesh) pMesh->AddRef();
	}
}


void CGameObject::SetMesh(CMesh *pMesh)
{
	if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh;
	if (m_pMesh) m_pMesh->AddRef();
}


void CGameObject::ReleaseUploadBuffers()
{
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->ReleaseUploadBuffers();
		}
	}
}

void CGameObject::Animate(float fTimeElapsed)
{
	m_xmOOBBTransformed.Transform(m_xmOOBB, XMLoadFloat4x4(&m_xmf4x4World));
	XMStoreFloat4(&m_xmOOBBTransformed.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBBTransformed.Orientation)));
}


void CGameObject::OnPrepareRender()
{

}


void CGameObject::OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList)
{
	if(m_d3dCbvGPUDescriptorHandle.ptr) pd3dCommandList->SetGraphicsRootDescriptorTable(2, m_d3dCbvGPUDescriptorHandle);
}


void CGameObject::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	//게임 객체가 카메라에 보이면 렌더링한다.
	if (IsVisible(pCamera))
	{
		UpdateShaderVariables(pd3dCommandList);
		if (m_pShader) {
			m_pShader->Render(pd3dCommandList, pCamera);
			m_pShader->UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World, m_nMaterial);
		}
		OnPrepareRender(pd3dCommandList);

		//게임 객체가 포함하는 모든 메쉬를 렌더링한다.
		if (m_ppMeshes)
		{
			for (int i = 0; i < m_nMeshes; i++)
			{
				if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList);
			}
		}
		
		if (m_pMesh) m_pMesh->Render(pd3dCommandList);
	}
}


void CGameObject::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, UINT nInstances)
{
	OnPrepareRender();

	if (m_pMesh) m_pMesh->Render(pd3dCommandList, nInstances);
}


void CGameObject::Rotate(XMFLOAT3 *pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis),
		XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}


void CGameObject::GenerateRayForPicking(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4&
	xmf4x4View, XMFLOAT3 *pxmf3PickRayOrigin, XMFLOAT3 *pxmf3PickRayDirection)
{
	XMFLOAT4X4 xmf4x4WorldView = Matrix4x4::Multiply(m_xmf4x4World, xmf4x4View);
	XMFLOAT4X4 xmf4x4Inverse = Matrix4x4::Inverse(xmf4x4WorldView);
	XMFLOAT3 xmf3CameraOrigin(0.0f, 0.0f, 0.0f);
	//카메라 좌표계의 원점을 모델 좌표계로 변환한다.
	*pxmf3PickRayOrigin = Vector3::TransformCoord(xmf3CameraOrigin, xmf4x4Inverse);
	//카메라 좌표계의 점(마우스 좌표를 역변환하여 구한 점)을 모델 좌표계로 변환한다.
	*pxmf3PickRayDirection = Vector3::TransformCoord(xmf3PickPosition, xmf4x4Inverse);
	//광선의 방향 벡터를 구한다.
	*pxmf3PickRayDirection = Vector3::Normalize(Vector3::Subtract(*pxmf3PickRayDirection,
		*pxmf3PickRayOrigin));
}


int CGameObject::PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4&
	xmf4x4View, float *pfHitDistance, CMesh* myMesh)
{
	int nIntersected = 0;
	if (m_pMesh) myMesh = m_pMesh;
	if (myMesh)
	{
		XMFLOAT3 xmf3PickRayOrigin, xmf3PickRayDirection;
		//모델 좌표계의 광선을 생성한다.
		GenerateRayForPicking(xmf3PickPosition, xmf4x4View, &xmf3PickRayOrigin,
			&xmf3PickRayDirection);
		//모델 좌표계의 광선과 메쉬의 교차를 검사한다.
		nIntersected = myMesh->CheckRayIntersection(xmf3PickRayOrigin,
			xmf3PickRayDirection, pfHitDistance);   
	}
	return(nIntersected);
}

void CGameObject::GenerateRayForCollision(XMFLOAT3& xmf3ColliderPosition, 
	XMFLOAT3 *pxmf3PickRayOrigin, XMFLOAT3 *pxmf3PickRayDirection)
{
	XMFLOAT4X4 xmf4x4Inverse = Matrix4x4::Inverse(m_xmf4x4World);
	// 모델 좌표계로 변환한다.
	*pxmf3PickRayOrigin = Vector3::TransformCoord(xmf3ColliderPosition, xmf4x4Inverse);
	*pxmf3PickRayDirection = Vector3::TransformCoord(GetPosition(), xmf4x4Inverse);
	//방향 벡터를 모델 좌표계로 변환한다.
	*pxmf3PickRayDirection = Vector3::Normalize(Vector3::Subtract(*pxmf3PickRayDirection,
		*pxmf3PickRayOrigin));
}


XMFLOAT3 CGameObject::CollisionByRayIntersection(XMFLOAT3& xmf3ColliderPosition, CMesh* myMesh)
{
	XMFLOAT3 returnVector(0.0f, 0.0f, 0.0f);
	if (!myMesh) myMesh = m_pMesh;
	if (myMesh)
	{
		XMFLOAT3 xmf3PickRayOrigin, xmf3PickRayDirection;
		//모델 좌표계의 광선을 생성한다.
		GenerateRayForCollision(xmf3ColliderPosition, &xmf3PickRayOrigin, &xmf3PickRayDirection);
		//모델 좌표계의 광선과 메쉬의 교차를 검사한다.
		returnVector = myMesh->CheckRayIntersection(xmf3PickRayOrigin, xmf3PickRayDirection);
	}
	return (returnVector);
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
	//XMFLOAT4X4 xmf4x4World;
	//XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
	////객체의 월드 변환 행렬을 루트 상수(32-비트 값)를 통하여 셰이더 변수(상수 버퍼)로 복사한다.
//	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);
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
	m_xmOOBBTransformed.Transform(m_xmOOBB, XMLoadFloat4x4(&m_xmf4x4World));
	XMStoreFloat4(&m_xmOOBBTransformed.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBBTransformed.Orientation)));
}


bool CGameObject::IsVisible(CCamera *pCamera)
{
	OnPrepareRender();

	if (!m_pMesh) return true;
	bool bIsVisible = false;
	BoundingOrientedBox xmBoundingBox = m_pMesh->GetBoundingBox();
	//모델 좌표계의 바운딩 박스를 월드 좌표계로 변환한다.
	xmBoundingBox.Transform(xmBoundingBox, XMLoadFloat4x4(&m_xmf4x4World));
	if (pCamera) bIsVisible = pCamera->IsInFrustum(m_xmOOBB);
	return(bIsVisible);
}


/////////////////// RotateObject /////////////////////////

CRotatingObject::CRotatingObject(int nMeshes) : CGameObject(nMeshes)
{
	m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed = 15.0f;
}


CRotatingObject::~CRotatingObject()
{
}


void CRotatingObject::Animate(float fTimeElapsed, XMFLOAT3 *xmf3PlayerPos)
{
	if (m_bRender == false) return;
	CGameObject::Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);

	m_xmOOBBTransformed.Transform(m_xmOOBB, XMLoadFloat4x4(&m_xmf4x4World));
	XMStoreFloat4(&m_xmOOBBTransformed.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBBTransformed.Orientation)));
}

/////////////////////////// Height Map Terrain ///////////////////////////////////////

CHeightMapTerrain::CHeightMapTerrain(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, LPCTSTR pFileName, int
	nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale) : CGameObject(0)
{
	//지형에 사용할 높이 맵의 가로, 세로의 크기이다.
	m_nWidth = nWidth;
	m_nLength = nLength;

	/*지형 객체는 격자 메쉬들의 배열로 만들 것이다. nBlockWidth, nBlockLength는 격자 메쉬 하나의 가로, 세로 크
	기이다. cxQuadsPerBlock, czQuadsPerBlock은 격자 메쉬의 가로 방향과 세로 방향 사각형의 개수이다.*/
	int cxQuadsPerBlock = nBlockWidth - 1;
	int czQuadsPerBlock = nBlockLength - 1;

	//xmf3Scale는 지형을 실제로 몇 배 확대할 것인가를 나타낸다.
	m_xmf3Scale = xmf3Scale;

	//지형에 사용할 높이 맵을 생성한다.
	m_pHeightMapImage = new CHeightMapImage(pFileName, nWidth, nLength, xmf3Scale);

	//지형에서 가로 방향, 세로 방향으로 격자 메쉬가 몇 개가 있는 가를 나타낸다.
	long cxBlocks = (m_nWidth - 1) / cxQuadsPerBlock;
	long czBlocks = (m_nLength - 1) / czQuadsPerBlock;

	//지형 전체를 표현하기 위한 격자 메쉬의 개수이다.
	m_nMeshes = cxBlocks * czBlocks;
	//지형 전체를 표현하기 위한 격자 메쉬에 대한 포인터 배열을 생성한다.
	m_ppMeshes = new CMesh*[m_nMeshes];
	for (int i = 0; i < m_nMeshes; i++) m_ppMeshes[i] = NULL;
	CHeightMapGridMesh *pHeightMapGridMesh = NULL;
	for (int z = 0, zStart = 0; z < czBlocks; z++)
	{
		for (int x = 0, xStart = 0; x < cxBlocks; x++)
		{
			//지형의 일부분을 나타내는 격자 메쉬의 시작 위치(좌표)이다.
			xStart = x * (nBlockWidth - 1);
			zStart = z * (nBlockLength - 1);
			//지형의 일부분을 나타내는 격자 메쉬를 생성하여 지형 메쉬에 저장한다.
			pHeightMapGridMesh = new CHeightMapGridMesh(pd3dDevice, pd3dCommandList, xStart,
				zStart, nBlockWidth, nBlockLength, xmf3Scale, m_pHeightMapImage);
			SetMesh(x + (z*cxBlocks), pHeightMapGridMesh);
		}
	}

	//지형을 렌더링하기 위한 셰이더를 생성한다.
	CTerrainShader *pShader = new CTerrainShader();
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pShader->BuildObjects(pd3dDevice, pd3dCommandList);
	SetShader(pShader);

	SetCbvGPUDescriptorHandle(pShader->GetGPUDescriptorHandle().ptr);
}


CHeightMapTerrain::~CHeightMapTerrain(void)
{
	if (m_pHeightMapImage) delete m_pHeightMapImage;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// Enemy Object ///////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

CEnemyObject::CEnemyObject(float fRadius, int nMeshes) {
	m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed = 100.0f;
	m_fMoveSpeed = XMConvertToRadians(m_fRotationSpeed) / 2 * fRadius;
}


CEnemyObject::~CEnemyObject()
{

}


void CEnemyObject::Animate(float fTimeElapsed, XMFLOAT3 *xmf3PlayerPos)
{
	if (m_bRender == false)
		return;

	// 중력 적용
	SetPosition(Vector3::Add(GetPosition(), Vector3::ScalarProduct(m_xmf3Gravity, fTimeElapsed, false)));

	if (m_pObjUpdatedContext) OnObjUpdateCallback();

	// 회전축으로 회전 시킨다.
	Rotate(&Vector3::Normalize(m_xmf3RotationAxis), - m_fRotationSpeed * fTimeElapsed);

	// 방향으로 이동한다.
	Move(m_xmf3Velocity, m_fMoveSpeed * fTimeElapsed);

	// Velocity를 저장한다 (벽과의 충돌시 반사방향을 구하는 용도)
	m_xmf3RealVelocity = Vector3::ScalarProduct(m_xmf3Velocity, m_fMoveSpeed * fTimeElapsed, false);

	m_xmOOBBTransformed.Transform(m_xmOOBB, XMLoadFloat4x4(&m_xmf4x4World));
	XMStoreFloat4(&m_xmOOBBTransformed.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBBTransformed.Orientation)));
}


void CEnemyObject::OnObjUpdateCallback()
{
	XMFLOAT3 xmf3ObjPosition = GetPosition();
	static CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)m_pObjUpdatedContext;

	float fHeight = pTerrain->GetHeight(xmf3ObjPosition.x, xmf3ObjPosition.z) +12.5f;

	if (xmf3ObjPosition.y < fHeight)
	{
		xmf3ObjPosition.y = fHeight;
		SetPosition(xmf3ObjPosition);
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// Bullet Object /////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

CBulletObject::CBulletObject(int nMeshes)
{
	m_xmf3RotationAxis = XMFLOAT3(0.3f, 0.5f, 0.4f);
	m_fRotationSpeed = 370.0f;
	m_bRender = false;
}


CBulletObject::~CBulletObject(){}


void CBulletObject::Animate(float fTimeElapsed)
{
	if (m_bRender == false)
		return;

	CGameObject::Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);
	Move(m_xmf3MoveDirection, m_fMoveSpeed * fTimeElapsed);

	m_xmOOBBTransformed.Transform(m_xmOOBB, XMLoadFloat4x4(&m_xmf4x4World));
	XMStoreFloat4(&m_xmOOBBTransformed.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBBTransformed.Orientation)));

	if (Vector3::Length(Vector3::Subtract(m_xmf3BegPos, GetPosition())) > m_fActiveRange) {
		SetPosition(0.0f, -1000.0f, 0.0f);
		m_bRender = false;
	}
}


void CBulletObject::SetDirection(XMFLOAT3 xmf3Direction)
{
	m_xmf3BegPos = GetPosition();
	m_xmf3MoveDirection = xmf3Direction;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// Particle Object ///////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

CParticleObject::CParticleObject(int nMeshes)
{
	for (int i = 0; i < PARTICLE_COUNT; ++i) {
		particles[i].SetPosition(0.0f, -1000.0f, 0.0f);
		particles[i].SetActiveRange(m_fActiveRange);
		particles[i].SetMoveSpeed(m_fMoveSpeed);
	}
}


CParticleObject::~CParticleObject()
{
}


void CParticleObject::Animate(float fTimeElapsed)
{
	if (m_bRender == false)
		return;

	for (int i = 0; i < PARTICLE_COUNT; ++i) {
		particles[i].Animate(fTimeElapsed);
	}

	if (particles[0].GetActive() == false) {
		for (int i = 0; i < PARTICLE_COUNT; ++i) {
			particles[i].SetPosition(0.0f, -1000.0f, 0.0f);
			particles[i].SetActive(false);
		}
		m_bRender = false;
	}
}


void CParticleObject::SetParticles()
{
	static uniform_real_distribution<float>normalDst(-1.0f, 1.0f);
	static default_random_engine dre((unsigned)time(NULL));

	XMFLOAT3 originPosition = GetPosition();
	for (int i = 0; i < PARTICLE_COUNT; ++i) {
		particles[i].SetPosition(originPosition);
		particles[i].SetActive(true);
		particles[i].SetDirection(XMFLOAT3(normalDst(dre), normalDst(dre), normalDst(dre)));
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
CRevolvingObject::CRevolvingObject()
{
	m_xmf3RevolutionAxis = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_fRevolutionSpeed = 0.0f;
}

CRevolvingObject::~CRevolvingObject()
{
}

void CRevolvingObject::Animate(float fTimeElapsed)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3RevolutionAxis), XMConvertToRadians(m_fRevolutionSpeed * fTimeElapsed));
	m_xmf4x4World = Matrix4x4::Multiply(m_xmf4x4World, mtxRotate);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//

bool COrbitObject::IsVisible(CCamera *pCamera)
{
	return true;
}

void COrbitObject::Animate(float fTimeElapsed)
{
	m_fDegree += m_fRevolutionSpeed * fTimeElapsed;
	if (m_fDegree > 360.0f) m_fDegree -= 360.0f;
	setRenderState();

	float rad = XMConvertToRadians(m_fDegree);

	m_xmf4x4World._41 = m_iCenter + m_iRadius * cos(rad);
	m_xmf4x4World._42 = m_iRadius * sin(rad);
}

void COrbitObject::setRenderState()
{
	if (0.0f <= m_fDegree && m_fDegree < 180.0f) m_bRender = true;
	if (180.0f <= m_fDegree && m_fDegree < 360.0f) m_bRender = false;
}