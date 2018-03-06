#include "stdafx.h"
#include "Scene.h"


CScene::CScene()
{
	m_pd3dGraphicsRootSignature = NULL;
}


CScene::~CScene()
{
}


void CScene::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);
	//������ Ȯ���� ������ �����̴�. x-��� z-���� 20��, y-���� 2�� Ȯ���Ѵ�.
	XMFLOAT3 xmf3Scale(20.0f, 2.0f, 20.0f);
	XMFLOAT4 xmf4Color(0.0f, 0.2f, 0.0f, 0.0f);
	//������ ���� �� �̹��� ����(HeightMap.raw)�� ����Ͽ� �����Ѵ�. ���� ���� ũ��� ����x����(257x257)�̴�.
#ifdef _WITH_TERRAIN_PARTITION
	/*�ϳ��� ���� �޽��� ũ��� ����x����(17x17)�̴�. ���� ��ü�� ���� �������� 16��, ���� �������� 16�� ���� ��
	���� ������. ������ �����ϴ� ���� �޽��� ������ �� 256(16x16)���� �ȴ�.*/
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList,
		m_pd3dGraphicsRootSignature, _T("Terrain/HeightMap.raw"), 257, 257, 17,
		17, xmf3Scale, xmf4Color);
#else
	//������ �ϳ��� ���� �޽�(257x257)�� �����Ѵ�.
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList,
		m_pd3dGraphicsRootSignature, _T("Terrain/HeightMap.raw"), 257, 257, 257,
		257, xmf3Scale, xmf4Color);
#endif

	m_nShaders = 5;
	m_pShaders[0] = new CMiroShader;
	m_pShaders[1] = new CRoofShader;
	m_pShaders[2] = new CEnemyShader;
	m_pShaders[3] = new CBulletShader;
	m_pShaders[4] = new CParticleShader;

	// map 1~4 �� ����
	srand((unsigned)time(NULL));
	int nFileNumber = rand() % 4 + 1;

	for (int i = 0; i < m_nShaders; ++i) {
		m_pShaders[i]->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
		m_pShaders[i]->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain, nFileNumber);
	}
	m_pShaders[2]->SetParticleShader(m_pShaders[4]);
}


void CScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();

	for (int i = 0; i < m_nShaders; i++)
	{
		m_pShaders[i]->ReleaseShaderVariables();
		m_pShaders[i]->ReleaseObjects();
		delete m_pShaders[i];
	}
	if (m_pTerrain) delete m_pTerrain;
}


void CScene::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_nShaders; i++) m_pShaders[i]->ReleaseUploadBuffers();
	if (m_pTerrain) m_pTerrain->ReleaseUploadBuffers();
}


ID3D12RootSignature *CScene::GetGraphicsRootSignature()
{
	return(m_pd3dGraphicsRootSignature);
}


void CScene::Collision()
{
	// �÷��̾� ������Ʈ�� ���� �浹üũ
	m_pShaders[0]->CollisionCheck(m_pPlayer);

	// ������ �� ������Ʈ�� ���� �浹üũ
	CGameObject** objects = m_pShaders[2]->GetObjects();
	int objectsCount = m_pShaders[2]->GetObjectsSize();
	for(int i=0; i<objectsCount; ++i)
		m_pShaders[0]->CollisionCheck(objects[i]);

	// ������ �Ѿ� ������Ʈ�� ���� �浹üũ
	objects = m_pShaders[3]->GetObjects();
	objectsCount = m_pShaders[3]->GetObjectsSize();
	for (int i = 0; i<objectsCount; ++i)
		m_pShaders[0]->CollisionCheck(objects[i], true);

	// ������ �Ѿ˰� �� ������Ʈ�� �浹üũ
	for (int i = 0; i<objectsCount; ++i)
		m_pShaders[2]->CollisionCheck(objects[i]);

	// �� ������Ʈ�� �÷��̾� �浹üũ
	m_pShaders[2]->CollisionCheck(m_pPlayer);
}


void CScene::FireBullet()
{
	m_pShaders[3]->SetBullet(m_pPlayer);
}


ID3D12RootSignature *CScene::CreateGraphicsRootSignature(ID3D12Device *pd3dDevice)
{
	ID3D12RootSignature *pd3dGraphicsRootSignature = NULL;

	D3D12_ROOT_PARAMETER pd3dRootParameters[3];
	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[0].Constants.Num32BitValues = 16;
	pd3dRootParameters[0].Constants.ShaderRegister = 0;
	pd3dRootParameters[0].Constants.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[1].Constants.Num32BitValues = 32;
	pd3dRootParameters[1].Constants.ShaderRegister = 1;
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	pd3dRootParameters[2].Descriptor.ShaderRegister = 0; //t0
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 0;
	d3dRootSignatureDesc.pStaticSamplers = NULL;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob *pd3dSignatureBlob = NULL;
	ID3DBlob *pd3dErrorBlob = NULL;
	::D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		&pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(),
		pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void
			**)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
	lParam)
{
	return(false);
}


bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam,
	LPARAM lParam)
{
	return(false);
}


bool CScene::ProcessInput(UCHAR *pKeysBuffer)
{
	return(false);
}


void CScene::AnimateObjects(float fTimeElapsed)
{
	m_pShaders[2]->SetPlayerPos(m_pPlayer->GetPosition());
	for (int i = 0; i < m_nShaders; i++)
	{
		m_pShaders[i]->AnimateObjects(fTimeElapsed);
	}
}


void CScene::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	if (m_pTerrain) m_pTerrain->Render(pd3dCommandList, pCamera);

	for (int i = 0; i < m_nShaders; i++)
	{
		m_pShaders[i]->Render(pd3dCommandList, pCamera);
	}
}


CGameObject *CScene::PickObjectPointedByCursor(int xClient, int yClient, CCamera
	*pCamera)
{
	if (!pCamera) return(NULL);
	XMFLOAT4X4 xmf4x4View = pCamera->GetViewMatrix();
	XMFLOAT4X4 xmf4x4Projection = pCamera->GetProjectionMatrix();
	D3D12_VIEWPORT d3dViewport = pCamera->GetViewport();
	XMFLOAT3 xmf3PickPosition;
	/*ȭ�� ��ǥ���� �� (xClient, yClient)�� ȭ�� ��ǥ ��ȯ�� ����ȯ�� ���� ��ȯ�� ����ȯ�� �Ѵ�. �� ����� ī�޶�
	��ǥ���� ���̴�. ���� ����� ī�޶󿡼� z-������ �Ÿ��� 1�̹Ƿ� z-��ǥ�� 1�� �����Ѵ�.*/
	xmf3PickPosition.x = (((2.0f * xClient) / d3dViewport.Width) - 1) /
		xmf4x4Projection._11;
	xmf3PickPosition.y = -(((2.0f * yClient) / d3dViewport.Height) - 1) /
		xmf4x4Projection._22;
	xmf3PickPosition.z = 1.0f;

	int nIntersected = 0;
	float fHitDistance = FLT_MAX, fNearestHitDistance = FLT_MAX;
	CGameObject *pIntersectedObject = NULL, *pNearestObject = NULL;
	//���̴��� ��� ���� ��ü�鿡 ���� ���콺 ��ŷ�� �����Ͽ� ī�޶�� ���� ����� ���� ��ü�� ���Ѵ�.
	for (int i = 0; i < m_nShaders; i++)
	{
		pIntersectedObject = m_pShaders[i]->PickObjectByRayIntersection(xmf3PickPosition,
			xmf4x4View, &fHitDistance);

		if (pIntersectedObject && (fHitDistance < fNearestHitDistance) )
		{
			fNearestHitDistance = fHitDistance;
			pNearestObject = pIntersectedObject;
		}
	}
	// ���õ� ������Ʈ�� ���� �ƴѰ�� ��ŷ���� ����
	if (pNearestObject && pNearestObject->GetObjectType() != ObjType::ENEMY)
		pNearestObject = NULL;

	return(pNearestObject);
}


void CScene::Reset()
{

}