#include "stdafx.h"
#include "Scene.h"

#define Light_Sun m_pLights->m_pLights[0]
#define Light_Moon m_pLights->m_pLights[1]
#define Light_Flash m_pLights->m_pLights[2]

CScene::CScene()
{
	m_pd3dGraphicsRootSignature = NULL;
}


CScene::~CScene()
{
}

void CScene::BuildLightsAndMaterials(void *pContext, int nFileNumber)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	// 라이트 위치 불러오기
	int mapArr[20 * 20];
	string sFileName = "Map/map" + to_string(nFileNumber) + ".txt";

	ifstream in(sFileName);
	for (int i = 0; i < 20; ++i) {
		for (int j = 0; j < 20; ++j) {
			in >> mapArr[i * 20 + j];
		}
	}
	in.close();

	// 라이트 설정
	m_pLights = new LIGHTS;
	::ZeroMemory(m_pLights, sizeof(LIGHTS));

	m_pLights->m_xmf4GlobalAmbient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);

	m_pLights->m_pLights[0].m_bEnable = true;			// Sun
	m_pLights->m_pLights[0].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.95f, 0.35f, 0.35f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_pLights->m_pLights[0].m_xmf3Direction = XMFLOAT3(0.0f, -0.5f, 1.0f);

	m_pLights->m_pLights[1].m_bEnable = false;			// Moon
	m_pLights->m_pLights[1].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.6f, 0.3f, 0.15f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights->m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, -0.5f, -1.0f);

	m_pLights->m_pLights[2].m_bEnable = false;
	m_pLights->m_pLights[2].m_nType = SPOT_LIGHT;		// Player SpotLight
	m_pLights->m_pLights[2].m_fRange = 170.0f;
	m_pLights->m_pLights[2].m_xmf4Ambient = XMFLOAT4(0.2f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[2].m_xmf4Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_pLights->m_pLights[2].m_xmf4Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[2].m_xmf3Position = XMFLOAT3(-50.0f, 20.0f, -5.0f);
	m_pLights->m_pLights[2].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[2].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights->m_pLights[2].m_fFalloff = 7.0f;
	m_pLights->m_pLights[2].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	m_pLights->m_pLights[2].m_fTheta = (float)cos(XMConvertToRadians(20.0f));

	XMFLOAT3 startPos(175.0f, 210.0f, 250.0f);
	float fxzSize = 30.0f;
	float fySize = 13.0f;

	int xObjects = 20, yObjects = 1, zObjects = 20;

	int i = 5;
	pointLightState = OFF;
	for (int x = 0; x < xObjects; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			if (mapArr[x*xObjects + z] != Map_Light) continue;
			m_nPointLights++;
			float xPosition = startPos.x + x * fxzSize;
			float zPosition = startPos.z + z * fxzSize;
			float fHeight = pTerrain->GetHeight(xPosition, zPosition);
			m_pLights->m_pLights[i].m_bEnable = false;
			m_pLights->m_pLights[i].m_nType = POINT_LIGHT;
			m_pLights->m_pLights[i].m_fRange = 60.0f;
			m_pLights->m_pLights[i].m_xmf4Ambient = XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f);
			m_pLights->m_pLights[i].m_xmf4Diffuse = XMFLOAT4(0.8f, 0.0f, 0.0f, 1.0f);
			m_pLights->m_pLights[i].m_xmf4Specular = XMFLOAT4(0.4f, 0.1f, 0.1f, 0.0f);
			m_pLights->m_pLights[i].m_xmf3Position = XMFLOAT3(xPosition, fHeight + fySize / 2, zPosition);
			m_pLights->m_pLights[i].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
			m_pLights->m_pLights[i++].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);
		}
	}

	// 재질 설정
	m_pMaterials = new MATERIALS;
	::ZeroMemory(m_pMaterials, sizeof(MATERIALS));

	m_pMaterials->m_pReflections[0] =
	{ XMFLOAT4(0.3f, 0.15f, 0.15f, 1.0f), XMFLOAT4(1.0f, 0.8f, 0.8f, 1.0f), 
		XMFLOAT4(1.0f, 1.0f, 1.0f, 5.0f),XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };	// Player
	m_pMaterials->m_pReflections[1] = 
	{ XMFLOAT4(0.2f, 0.2f, 0.25f, 1.0f), XMFLOAT4(0.5f, 0.85f, 1.0f, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 10.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };	// Roof
	m_pMaterials->m_pReflections[2] = 
	{ XMFLOAT4(0.2f, 0.1f, 0.0f, 1.0f), XMFLOAT4(0.6f, 0.5f, 0.0f, 1.0f), 
		XMFLOAT4(1.0f, 1.0f, 1.0f, 10.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };	// Wall
	m_pMaterials->m_pReflections[3] = 
	{ XMFLOAT4(0.5f, 0.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.3f, 0.4f, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 20.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };	// Enemy
	m_pMaterials->m_pReflections[4] =
	{ XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
		XMFLOAT4(0.1f, 0.1f, 0.1f, 50.0f), XMFLOAT4(0.95f, 0.25f, 0.0f, 1.0f) };	// Sun
	m_pMaterials->m_pReflections[5] = 
	{ XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
		XMFLOAT4(0.1f, 0.1f, 0.1f, 30.0f), XMFLOAT4(1.0f, 0.9f, 0.0f, 1.0f) };	// Moon
	m_pMaterials->m_pReflections[6] =
	{ XMFLOAT4(0.1f, 0.2f, 0.2f, 1.0f), XMFLOAT4(0.1f, 0.5f, 0.1f, 1.0f),
		XMFLOAT4(0.8f, 0.8f, 0.8f, 4.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };	// Terrain
	m_pMaterials->m_pReflections[7] =
	{ XMFLOAT4(0.2f, 0.4f, 0.1f, 1.0f), XMFLOAT4(0.5f, 1.0f, 0.4f, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 25.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };	// Enemy2
}



void CScene::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);
	//지형을 확대할 스케일 벡터이다. x-축과 z-축은 20배, y-축은 2배 확대한다.
	XMFLOAT3 xmf3Scale(20.0f, 2.0f, 20.0f);
	XMFLOAT4 xmf4Color(0.0f, 0.2f, 0.0f, 0.0f);
	//지형을 높이 맵 이미지 파일(HeightMap.raw)을 사용하여 생성한다. 높이 맵의 크기는 가로x세로(257x257)이다.
#ifdef _WITH_TERRAIN_PARTITION
	/*하나의 격자 메쉬의 크기는 가로x세로(17x17)이다. 지형 전체는 가로 방향으로 16개, 세로 방향으로 16의 격자 메
	쉬를 가진다. 지형을 구성하는 격자 메쉬의 개수는 총 256(16x16)개가 된다.*/
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList,
		m_pd3dGraphicsRootSignature, _T("Terrain/HeightMap.raw"), 257, 257, 17,
		17, xmf3Scale);
	m_pTerrain->m_nMaterial = Material_Terrain;
#else
	//지형을 하나의 격자 메쉬(257x257)로 생성한다.
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList,
		m_pd3dGraphicsRootSignature, _T("Terrain/HeightMap.raw"), 257, 257, 257,
		257, xmf3Scale);
	m_pTerrain->m_nMaterial = Material_Terrain;
#endif

	m_nShaders = 4;
	m_ppShaders = new CObjectsShader*[m_nShaders];
	m_ppShaders[0] = new CMiroShader;
	m_ppShaders[1] = new CRoofShader;
	m_ppShaders[2] = new CEnemyShader;
	m_ppShaders[3] = new CDayCycleShader;

	// map 1~4 중 선택
	srand((unsigned)time(NULL));
	int nFileNumber = rand() % 4 + 1;

	for (int i = 0; i < m_nShaders; ++i) {
		m_ppShaders[i]->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
		m_ppShaders[i]->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain, nFileNumber);
	}

	m_pSunMoon = m_ppShaders[3]->GetObjects();

	BuildLightsAndMaterials(m_pTerrain, nFileNumber);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}


void CScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();

	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->ReleaseShaderVariables();
		m_ppShaders[i]->ReleaseObjects();
		delete m_ppShaders[i];
	} 
	if (m_ppShaders) delete[] m_ppShaders;

	ReleaseShaderVariables();

	if (m_pTerrain) delete m_pTerrain;
	if (m_pLights) delete m_pLights;
	if (m_pMaterials) delete m_pMaterials;
}


void CScene::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_nShaders; i++) m_ppShaders[i]->ReleaseUploadBuffers();
	if (m_pTerrain) m_pTerrain->ReleaseUploadBuffers();
}


ID3D12RootSignature *CScene::GetGraphicsRootSignature()
{
	return(m_pd3dGraphicsRootSignature);
}


void CScene::Collision()
{
	// 플레이어 오브젝트와 벽의 충돌체크
	m_ppShaders[0]->CollisionCheck(m_pPlayer);

	// 각각의 적 오브젝트와 벽의 충돌체크
	CGameObject** objects = m_ppShaders[2]->GetObjects();
	int objectsCount = m_ppShaders[2]->GetObjectsSize();
	for(int i=0; i<objectsCount; ++i)
		m_ppShaders[0]->CollisionCheck(objects[i]);

	// 적 오브젝트 각각의 충돌 체크
	for (int i = 0; i<objectsCount; ++i)
		m_ppShaders[2]->CollisionCheck();

	// 적 오브젝트와 플레이어 충돌체크
	m_ppShaders[2]->CollisionCheck(m_pPlayer);
}

ID3D12RootSignature *CScene::CreateGraphicsRootSignature(ID3D12Device *pd3dDevice)
{
	ID3D12RootSignature *pd3dGraphicsRootSignature = NULL;

	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[1];
	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	pd3dDescriptorRanges[0].NumDescriptors = 1;
	pd3dDescriptorRanges[0].BaseShaderRegister = 2; //Game Objects
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = 0;

	D3D12_ROOT_PARAMETER pd3dRootParameters[5];
	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 0; // Player
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[1].Descriptor.ShaderRegister = 1; //Camera
	pd3dRootParameters[1].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[2].DescriptorTable.NumDescriptorRanges = 1;//Objects
	pd3dRootParameters[2].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[0];
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[3].Descriptor.ShaderRegister = 3; //Materials
	pd3dRootParameters[3].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[4].Descriptor.ShaderRegister = 4; //Lights
	pd3dRootParameters[4].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

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

void CScene::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbLights->Map(0, NULL, (void **)&m_pcbMappedLights);

	UINT ncbMaterialBytes = ((sizeof(MATERIALS) + 255) & ~255); //256의 배수
	m_pd3dcbMaterials = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbMaterialBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbMaterials->Map(0, NULL, (void **)&m_pcbMappedMaterials);

	::memcpy(m_pcbMappedMaterials, m_pMaterials, sizeof(MATERIALS));
}

void CScene::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	::memcpy(m_pcbMappedLights, m_pLights, sizeof(LIGHTS));
}

void CScene::ReleaseShaderVariables()
{
	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		m_pd3dcbLights->Release();
	}
	if (m_pd3dcbMaterials)
	{
		m_pd3dcbMaterials->Unmap(0, NULL);
		m_pd3dcbMaterials->Release();
	}
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
	m_ppShaders[2]->SetPlayerPos(m_pPlayer->GetPosition());
	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->AnimateObjects(fTimeElapsed);
	}

	// 플레이어가 방에 있는지 밖에있는지에 따라 DirectionalLights, PointLights 켜고 끄는 함수
	checkPlayerLocation();
	directionalLightUpdate();

	if (m_pLights)
	{
		Light_Flash.m_xmf3Position = m_pPlayer->GetPosition();
		Light_Flash.m_xmf3Direction = m_pPlayer->GetLookVector();
	}
}


void CScene::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(4, d3dcbLightsGpuVirtualAddress); //Lights

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbMaterialsGpuVirtualAddress = m_pd3dcbMaterials->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(3, d3dcbMaterialsGpuVirtualAddress);

	if (m_pTerrain) m_pTerrain->Render(pd3dCommandList, pCamera);

	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->Render(pd3dCommandList, pCamera);
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
	/*화면 좌표계의 점 (xClient, yClient)를 화면 좌표 변환의 역변환과 투영 변환의 역변환을 한다. 그 결과는 카메라
	좌표계의 점이다. 투영 평면이 카메라에서 z-축으로 거리가 1이므로 z-좌표는 1로 설정한다.*/
	xmf3PickPosition.x = (((2.0f * xClient) / d3dViewport.Width) - 1) /
		xmf4x4Projection._11;
	xmf3PickPosition.y = -(((2.0f * yClient) / d3dViewport.Height) - 1) /
		xmf4x4Projection._22;
	xmf3PickPosition.z = 1.0f;

	int nIntersected = 0;
	float fHitDistance = FLT_MAX, fNearestHitDistance = FLT_MAX;
	CGameObject *pIntersectedObject = NULL, *pNearestObject = NULL;
	//셰이더의 모든 게임 객체들에 대한 마우스 픽킹을 수행하여 카메라와 가장 가까운 게임 객체를 구한다.
	for (int i = 0; i < m_nShaders; i++)
	{
		pIntersectedObject = m_ppShaders[i]->PickObjectByRayIntersection(xmf3PickPosition,
			xmf4x4View, &fHitDistance);

		if (pIntersectedObject && (fHitDistance < fNearestHitDistance) )
		{
			fNearestHitDistance = fHitDistance;
			pNearestObject = pIntersectedObject;
		}
	}
	// 선택된 오브젝트가 적이 아닌경우 피킹하지 않음
	if (pNearestObject && pNearestObject->GetObjectType() != ObjType::ENEMY)
		pNearestObject = NULL;

	return(pNearestObject);
}

void CScene::FlashLight()
{
	Light_Flash.m_bEnable = !Light_Flash.m_bEnable;
}


void CScene::Reset()
{

}


void CScene::checkPlayerLocation()
{
	if (m_pPlayer->isInHouse())
	{
		if (pointLightState == ON) return;
		turnOffDirectLights();
		turnOnPointLights();
		pointLightState = ON;
	}
	else
	{
		if (pointLightState == OFF) return;
		turnOnDirectLights();
		turnOffPointLights();
		pointLightState = OFF;
	}
}


void CScene::turnOnDirectLights()
{
	Light_Sun.m_bEnable = true;
}

void CScene::turnOffDirectLights()
{
	Light_Sun.m_bEnable = false;
}

void CScene::turnOnPointLights()
{
	Light_Sun.m_bEnable = false;
	Light_Moon.m_bEnable = false;
	for (int i = 5; i < 5 + m_nPointLights; ++i)
	{
		m_pLights->m_pLights[i].m_bEnable = true;
	}
}

void CScene::turnOffPointLights()
{
	for (int i = 5; i < 5 + m_nPointLights; ++i)
	{
		m_pLights->m_pLights[i].m_bEnable = false;
	}
}

void CScene::directionalLightUpdate()
{
	if (pointLightState == ON) return;
	if (m_pSunMoon)
	{
		Light_Sun.m_bEnable = m_pSunMoon[0]->GetActive();
		Light_Sun.m_xmf3Direction = Vector3::ScalarProduct(m_pSunMoon[0]->GetPosition(), -1);

		Light_Moon.m_bEnable = m_pSunMoon[1]->GetActive();
		Light_Moon.m_xmf3Direction = Vector3::ScalarProduct(m_pSunMoon[1]->GetPosition(), -1);
	}
}