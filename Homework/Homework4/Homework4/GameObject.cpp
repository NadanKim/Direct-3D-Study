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
	//���� ��ü�� ī�޶� ���̸� �������Ѵ�.
	if (IsVisible(pCamera))
	{
		UpdateShaderVariables(pd3dCommandList);
		if (m_pShader) {
			m_pShader->Render(pd3dCommandList, pCamera);
			m_pShader->UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World, m_nMaterial);
		}
		OnPrepareRender(pd3dCommandList);

		//���� ��ü�� �����ϴ� ��� �޽��� �������Ѵ�.
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
	//ī�޶� ��ǥ���� ������ �� ��ǥ��� ��ȯ�Ѵ�.
	*pxmf3PickRayOrigin = Vector3::TransformCoord(xmf3CameraOrigin, xmf4x4Inverse);
	//ī�޶� ��ǥ���� ��(���콺 ��ǥ�� ����ȯ�Ͽ� ���� ��)�� �� ��ǥ��� ��ȯ�Ѵ�.
	*pxmf3PickRayDirection = Vector3::TransformCoord(xmf3PickPosition, xmf4x4Inverse);
	//������ ���� ���͸� ���Ѵ�.
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
		//�� ��ǥ���� ������ �����Ѵ�.
		GenerateRayForPicking(xmf3PickPosition, xmf4x4View, &xmf3PickRayOrigin,
			&xmf3PickRayDirection);
		//�� ��ǥ���� ������ �޽��� ������ �˻��Ѵ�.
		nIntersected = myMesh->CheckRayIntersection(xmf3PickRayOrigin,
			xmf3PickRayDirection, pfHitDistance);   
	}
	return(nIntersected);
}

void CGameObject::GenerateRayForCollision(XMFLOAT3& xmf3ColliderPosition, 
	XMFLOAT3 *pxmf3PickRayOrigin, XMFLOAT3 *pxmf3PickRayDirection)
{
	XMFLOAT4X4 xmf4x4Inverse = Matrix4x4::Inverse(m_xmf4x4World);
	// �� ��ǥ��� ��ȯ�Ѵ�.
	*pxmf3PickRayOrigin = Vector3::TransformCoord(xmf3ColliderPosition, xmf4x4Inverse);
	*pxmf3PickRayDirection = Vector3::TransformCoord(GetPosition(), xmf4x4Inverse);
	//���� ���͸� �� ��ǥ��� ��ȯ�Ѵ�.
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
		//�� ��ǥ���� ������ �����Ѵ�.
		GenerateRayForCollision(xmf3ColliderPosition, &xmf3PickRayOrigin, &xmf3PickRayDirection);
		//�� ��ǥ���� ������ �޽��� ������ �˻��Ѵ�.
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
	////��ü�� ���� ��ȯ ����� ��Ʈ ���(32-��Ʈ ��)�� ���Ͽ� ���̴� ����(��� ����)�� �����Ѵ�.
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


//���� ��ü�� ���� z-�� ���͸� ��ȯ�Ѵ�.
XMFLOAT3 CGameObject::GetLook()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32,
		m_xmf4x4World._33)));
}


//���� ��ü�� ���� y-�� ���͸� ��ȯ�Ѵ�.
XMFLOAT3 CGameObject::GetUp()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22,
		m_xmf4x4World._23)));
}


//���� ��ü�� ���� x-�� ���͸� ��ȯ�Ѵ�.
XMFLOAT3 CGameObject::GetRight()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12,
		m_xmf4x4World._13)));
}


//���� ��ü��  Direction �������� Speed�� �̵��Ѵ�.
void CGameObject::Move(XMFLOAT3& vDirection, float fSpeed)
{
	SetPosition(m_xmf4x4World._41 + vDirection.x * fSpeed, m_xmf4x4World._42 + vDirection.y * fSpeed, m_xmf4x4World._43 + vDirection.z * fSpeed);
}


//���� ��ü�� ���� x-�� �������� �̵��Ѵ�.
void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	CGameObject::SetPosition(xmf3Position);
}


//���� ��ü�� ���� y-�� �������� �̵��Ѵ�.
void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	CGameObject::SetPosition(xmf3Position);
}


//���� ��ü�� ���� z-�� �������� �̵��Ѵ�.
void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	CGameObject::SetPosition(xmf3Position);
}


//���� ��ü�� �־��� ������ ȸ���Ѵ�.
void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch),
		XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}


// �ٿ�� �ڽ��� �����Ѵ�.
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
	//�� ��ǥ���� �ٿ�� �ڽ��� ���� ��ǥ��� ��ȯ�Ѵ�.
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
	//������ ����� ���� ���� ����, ������ ũ���̴�.
	m_nWidth = nWidth;
	m_nLength = nLength;

	/*���� ��ü�� ���� �޽����� �迭�� ���� ���̴�. nBlockWidth, nBlockLength�� ���� �޽� �ϳ��� ����, ���� ũ
	���̴�. cxQuadsPerBlock, czQuadsPerBlock�� ���� �޽��� ���� ����� ���� ���� �簢���� �����̴�.*/
	int cxQuadsPerBlock = nBlockWidth - 1;
	int czQuadsPerBlock = nBlockLength - 1;

	//xmf3Scale�� ������ ������ �� �� Ȯ���� ���ΰ��� ��Ÿ����.
	m_xmf3Scale = xmf3Scale;

	//������ ����� ���� ���� �����Ѵ�.
	m_pHeightMapImage = new CHeightMapImage(pFileName, nWidth, nLength, xmf3Scale);

	//�������� ���� ����, ���� �������� ���� �޽��� �� ���� �ִ� ���� ��Ÿ����.
	long cxBlocks = (m_nWidth - 1) / cxQuadsPerBlock;
	long czBlocks = (m_nLength - 1) / czQuadsPerBlock;

	//���� ��ü�� ǥ���ϱ� ���� ���� �޽��� �����̴�.
	m_nMeshes = cxBlocks * czBlocks;
	//���� ��ü�� ǥ���ϱ� ���� ���� �޽��� ���� ������ �迭�� �����Ѵ�.
	m_ppMeshes = new CMesh*[m_nMeshes];
	for (int i = 0; i < m_nMeshes; i++) m_ppMeshes[i] = NULL;
	CHeightMapGridMesh *pHeightMapGridMesh = NULL;
	for (int z = 0, zStart = 0; z < czBlocks; z++)
	{
		for (int x = 0, xStart = 0; x < cxBlocks; x++)
		{
			//������ �Ϻκ��� ��Ÿ���� ���� �޽��� ���� ��ġ(��ǥ)�̴�.
			xStart = x * (nBlockWidth - 1);
			zStart = z * (nBlockLength - 1);
			//������ �Ϻκ��� ��Ÿ���� ���� �޽��� �����Ͽ� ���� �޽��� �����Ѵ�.
			pHeightMapGridMesh = new CHeightMapGridMesh(pd3dDevice, pd3dCommandList, xStart,
				zStart, nBlockWidth, nBlockLength, xmf3Scale, m_pHeightMapImage);
			SetMesh(x + (z*cxBlocks), pHeightMapGridMesh);
		}
	}

	//������ �������ϱ� ���� ���̴��� �����Ѵ�.
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

	// �߷� ����
	SetPosition(Vector3::Add(GetPosition(), Vector3::ScalarProduct(m_xmf3Gravity, fTimeElapsed, false)));

	if (m_pObjUpdatedContext) OnObjUpdateCallback();

	// ȸ�������� ȸ�� ��Ų��.
	Rotate(&Vector3::Normalize(m_xmf3RotationAxis), - m_fRotationSpeed * fTimeElapsed);

	// �������� �̵��Ѵ�.
	Move(m_xmf3Velocity, m_fMoveSpeed * fTimeElapsed);

	// Velocity�� �����Ѵ� (������ �浹�� �ݻ������ ���ϴ� �뵵)
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