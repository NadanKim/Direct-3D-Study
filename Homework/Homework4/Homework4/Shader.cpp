#include "stdafx.h"
#include "Shader.h"

CShader::~CShader()
{
	if (m_ppd3dPipelineStates)
	{
		for (int i = 0; i < m_nPipelineStates; i++) if (m_ppd3dPipelineStates[i])
			m_ppd3dPipelineStates[i]->Release();
		delete[] m_ppd3dPipelineStates;
	}
}


//래스터라이저 상태를 설정하기 위한 구조체를 반환한다.
D3D12_RASTERIZER_DESC CShader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return(d3dRasterizerDesc);
}


//깊이-스텐실 검사를 위한 상태를 설정하기 위한 구조체를 반환한다.
D3D12_DEPTH_STENCIL_DESC CShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = TRUE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}


//블렌딩 상태를 설정하기 위한 구조체를 반환한다.
D3D12_BLEND_DESC CShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}


//입력 조립기에게 정점 버퍼의 구조를 알려주기 위한 구조체를 반환한다.
D3D12_INPUT_LAYOUT_DESC CShader::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = NULL;
	d3dInputLayoutDesc.NumElements = 0;

	return(d3dInputLayoutDesc);
}


D3D12_SHADER_BYTECODE CShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}


D3D12_SHADER_BYTECODE CShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}


//셰이더 소스 코드를 컴파일하여 바이트 코드 구조체를 반환한다.
D3D12_SHADER_BYTECODE CShader::CompileShaderFromFile(WCHAR *pszFileName, LPCSTR
	pszShaderName, LPCSTR pszShaderProfile, ID3DBlob **ppd3dShaderBlob)
{
	UINT nCompileFlags = 0;
#if defined(_DEBUG)
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	HRESULT hResult = ::D3DCompileFromFile(pszFileName, NULL, NULL, pszShaderName, pszShaderProfile,
		nCompileFlags, 0, ppd3dShaderBlob, ppd3dShaderBlob);
	assert(SUCCEEDED(hResult) && " D3DCompileFromFile Failed!! ");

	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
	d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();

	return(d3dShaderByteCode);
}


//그래픽스 파이프라인 상태 객체를 생성한다.
void CShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature
	*pd3dGraphicsRootSignature)
{
	ID3DBlob *pd3dVertexShaderBlob = NULL, *pd3dPixelShaderBlob = NULL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;

	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
	d3dPipelineStateDesc.VS = CreateVertexShader(&pd3dVertexShaderBlob);
	d3dPipelineStateDesc.PS = CreatePixelShader(&pd3dPixelShaderBlob);
	d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
	d3dPipelineStateDesc.BlendState = CreateBlendState();
	d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	d3dPipelineStateDesc.InputLayout = CreateInputLayout();
	d3dPipelineStateDesc.SampleMask = UINT_MAX;
	d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineStateDesc.NumRenderTargets = 1;
	d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineStateDesc.SampleDesc.Count = 1;
	d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc,
		__uuidof(ID3D12PipelineState), (void **)&m_ppd3dPipelineStates[0]);

	if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
	if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();

	if (d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[]
		d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}


void CShader::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList)
{
}


void CShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
}


void CShader::UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT4X4
	*pxmf4x4World, UINT nMaterial)
{
}


void CShader::ReleaseShaderVariables()
{
}


void CShader::OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList)
{
	//파이프라인에 그래픽스 상태 객체를 설정한다.
	pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[0]);
}


void CShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	OnPrepareRender(pd3dCommandList);
}



//////////////////////// PlayerShader //////////////////////

D3D12_INPUT_LAYOUT_DESC CPlayerShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new
		D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}


D3D12_SHADER_BYTECODE CPlayerShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSPlayerLighting", "vs_5_1",
		ppd3dShaderBlob));
}


D3D12_SHADER_BYTECODE CPlayerShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSPlayerLighting", "ps_5_1",
		ppd3dShaderBlob));
}


void CPlayerShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature
	*pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}

void CPlayerShader::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_PLAYER_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbPlayer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbPlayer->Map(0, NULL, (void **)&m_pcbMappedPlayer);
}

void CPlayerShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
}

void CPlayerShader::UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, 
	XMFLOAT4X4 *pxmf4x4World, UINT nMaterial)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	::memcpy(&m_pcbMappedPlayer->m_xmf4x4World, &xmf4x4World, sizeof(XMFLOAT4X4));
	m_pcbMappedPlayer->m_nMaterial = nMaterial;
}

void CPlayerShader::ReleaseShaderVariables()
{
	if (m_pd3dcbPlayer)
	{
		m_pd3dcbPlayer->Unmap(0, NULL);
		m_pd3dcbPlayer->Release();
	}
}

void CPlayerShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CShader::Render(pd3dCommandList, pCamera);

	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbPlayer->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(0, d3dGpuVirtualAddress);
}

//////////////////////ObjectsShader////////////////////////////

D3D12_INPUT_LAYOUT_DESC CObjectsShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new
		D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}


D3D12_SHADER_BYTECODE CObjectsShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSLighting", "vs_5_1",
		ppd3dShaderBlob));
}


D3D12_SHADER_BYTECODE CObjectsShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSLighting", "ps_5_1",
		ppd3dShaderBlob));
}


void CObjectsShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature
	*pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}


void CObjectsShader::CreateShaderVariables(ID3D12Device *pd3dDevice,
	ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbGameObjectBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList,
		NULL, ncbGameObjectBytes * m_nObjects,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	//정점 버퍼(업로드 힙)에 대한 포인터를 저장한다.
	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);

	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	d3dDescriptorHeapDesc.NumDescriptors = m_nObjects;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dCbvSrvDescriptorHeap);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvSrvCPUDescriptorStartHandle = GetCPUDescriptorHandle();
	D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvSrvGPUDescriptorStartHandle = GetGPUDescriptorHandle();

	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbGameObjects->GetGPUVirtualAddress();
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dcbvDesc;
	d3dcbvDesc.SizeInBytes = ncbGameObjectBytes;
	for (int j = 0; j < m_nObjects; j++)
	{
		d3dcbvDesc.BufferLocation = d3dGpuVirtualAddress + (ncbGameObjectBytes * j);
		D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
		d3dCbvCPUDescriptorHandle.ptr = d3dCbvSrvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * j);
		pd3dDevice->CreateConstantBufferView(&d3dcbvDesc, d3dCbvCPUDescriptorHandle);
	}
}


void CObjectsShader::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects) m_pd3dcbGameObjects->Unmap(0, NULL);
	if (m_pd3dcbGameObjects) m_pd3dcbGameObjects->Release();
	if (m_pd3dCbvSrvDescriptorHeap) m_pd3dCbvSrvDescriptorHeap->Release();
}


//인스턴싱 정보(객체의 월드 변환 행렬과 색상)를 정점 버퍼에 복사한다.
void CObjectsShader::UpdateShaderVariables(ID3D12GraphicsCommandList
	*pd3dCommandList)
{
	UINT ncbGameObjectBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	for (int j = 0; j < m_nObjects; j++)
	{
		CB_GAMEOBJECT_INFO *pbMappedcbGameObject = (CB_GAMEOBJECT_INFO *)(m_pcbMappedGameObjects + (j * ncbGameObjectBytes));
		XMStoreFloat4x4(&pbMappedcbGameObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_ppObjects[j]->m_xmf4x4World)));
		pbMappedcbGameObject->m_nMaterial = m_ppObjects[j]->m_nMaterial;
	}
}

void CObjectsShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList, void *pContext, int nFileNumber)
{
}


void CObjectsShader::ReleaseObjects()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
		{
			if (m_ppObjects[j]) delete m_ppObjects[j];
		}
		delete[] m_ppObjects;
	}
}


void CObjectsShader::AnimateObjects(float fTimeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(fTimeElapsed);
	}
}


void CObjectsShader::ReleaseUploadBuffers()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) m_ppObjects[j]->ReleaseUploadBuffers();
	}
}


void CObjectsShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CShader::Render(pd3dCommandList, pCamera);

	UpdateShaderVariables(pd3dCommandList);

	pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);

	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
		{
			m_ppObjects[j]->Render(pd3dCommandList, pCamera);
		}
	}
}


CGameObject *CObjectsShader::PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition,
	XMFLOAT4X4& xmf4x4View, float *pfNearHitDistance)
{
	int nIntersected = 0;
	*pfNearHitDistance = FLT_MAX;
	float fHitDistance = FLT_MAX;
	CGameObject *pSelectedObject = NULL;
	for (int j = 0; j < m_nObjects; j++)
	{
		nIntersected = m_ppObjects[j]->PickObjectByRayIntersection(xmf3PickPosition,
			xmf4x4View, &fHitDistance, m_ppObjects[0]->GetMesh());
		if ((nIntersected > 0) && (fHitDistance < *pfNearHitDistance))
		{
			*pfNearHitDistance = fHitDistance;
			pSelectedObject = m_ppObjects[j];
		}
	}
	return(pSelectedObject);
}


/////////////////////////////// Terrain Shader //////////////////////////////////////////

void CTerrainShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList, void *pContext, int nFileNumber)
{
	m_nObjects = 1;
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	m_nObjects = 0;
}

void CTerrainShader::UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT4X4
	*pxmf4x4World, UINT nMaterial)
{
	CB_GAMEOBJECT_INFO *pbMappedcbGameObject = (CB_GAMEOBJECT_INFO *)(m_pcbMappedGameObjects);
	XMStoreFloat4x4(&pbMappedcbGameObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	pbMappedcbGameObject->m_nMaterial = nMaterial;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// Miro Shader //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

void CMiroShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList, void *pContext, int nFileNumber)
{
	// 벽 설정

	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;
	float fTerrainWidth = pTerrain->GetWidth(), fTerrainLength = pTerrain->GetLength();

	int mapArr[20 * 20];

	string sFileName = "Map/map" + to_string(nFileNumber) + ".txt";

	ifstream in(sFileName);
	for (int i = 0; i < 20; ++i) {
		for (int j = 0; j < 20; ++j) {
			in>>mapArr[i * 20 + j];
		}
	}
	in.close();

	int wallCount = 0;
	for (int i = 0; i < 20*20; ++i)
			if (mapArr[i] == WALL)
				++wallCount;

	CCubeMeshIlluminated *pCubeMesh = new CCubeMeshIlluminated(pd3dDevice,
		pd3dCommandList, 30.0f, 200.0f, 30.0f);

	XMFLOAT3 startPos(175.0f, 210.0f, 250.0f);
	float fxzSize = 30.0f;
	float fySize = 200.0f;

	//직육면체를 지형 표면에 그리고 지형보다 높은 위치에 파일에서 읽은 대로 배치한다.
	int xObjects = 20, yObjects = 1, zObjects = 20;
	m_nObjects = wallCount;
	m_ppObjects = new CGameObject*[m_nObjects];

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvSrvGPUDescriptorStartHandle = GetGPUDescriptorHandle();

	CGameObject *pGameObject = NULL;
	for (int i = 0, x = 0; x < xObjects; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			if (mapArr[x*xObjects + z] != WALL) continue;
			pGameObject = new CGameObject(1);
			float xPosition = startPos.x + x * fxzSize;
			float zPosition = startPos.z + z * fxzSize;
			float fHeight = pTerrain->GetHeight(xPosition, zPosition);
			pGameObject->SetMesh(pCubeMesh);
			pGameObject->SetPosition(xPosition, fHeight + fySize / 2,  zPosition);
			pGameObject->SetObjectType(ObjType::WALL);
			pGameObject->m_nMaterial = Material_Wall;
			pGameObject->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(15.0f, 100.0f, 15.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
			pGameObject->SetCbvGPUDescriptorHandle(d3dCbvSrvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
			m_ppObjects[i++] = pGameObject;
		}
	}
}

void CMiroShader::CollisionCheck(CPlayer *pPlayer)
{
	if (pPlayer->GetActive() == false) 	return;

	int i = 0;
	for (; i < m_nObjects; ++i)
	{
		if (m_ppObjects[i]->m_xmOOBB.Intersects(pPlayer->m_xmOOBB))
		{
			float reflectSpeed;
			// 플레이어의 현재 위치와 오브젝트의 위치로 광선을 만들고 
			// 오브젝트의 어떤 면과 가장 먼저 만나는지 검사하여  노멀벡터를 얻어온다.
			XMFLOAT3 xmfNormal = m_ppObjects[i]->CollisionByRayIntersection(pPlayer->GetPosition(), m_ppObjects[i]->GetMesh());
			XMVECTOR xmvNormal = XMLoadFloat3(&xmfNormal);
			XMVECTOR xmvProj = XMVector3Dot(XMLoadFloat3(&pPlayer->GetRealVelocity()), xmvNormal)*xmvNormal;
			XMStoreFloat(&reflectSpeed, XMVector3Length(xmvProj));

			pPlayer->Move(Vector3::ScalarProduct(xmfNormal, reflectSpeed));
		}
	}
}


void CMiroShader::CollisionCheck(CGameObject *pObject)
{
	if (pObject->GetActive() == false) 	return;

	// 다른 오브젝트와의 검사인 경우
	int i = 0;
	for (; i < m_nObjects; ++i)
	{
		if (m_ppObjects[i]->m_xmOOBB.Intersects(pObject->m_xmOOBB))
		{
			float reflectSpeed;
			// 플레이어의 현재 위치와 오브젝트의 위치로 광선을 만들고 
			// 오브젝트의 어떤 면과 가장 먼저 만나는지 검사하여  노멀벡터를 얻어온다.
			XMFLOAT3 xmfNormal = m_ppObjects[i]->CollisionByRayIntersection(pObject->GetPosition(), m_ppObjects[0]->GetMesh());
			XMVECTOR xmvNormal = XMLoadFloat3(&xmfNormal);
			XMVECTOR xmvProj = XMVector3Dot(XMLoadFloat3(&pObject->GetRealVelocity()), xmvNormal)*xmvNormal;
			XMStoreFloat(&reflectSpeed, XMVector3Length(xmvProj));

			pObject->Move(xmfNormal, reflectSpeed);
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// Roof Shader ////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////



void CRoofShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList, void *pContext, int nFileNumber)
{
	m_nObjects = 1;
	m_ppObjects = new CGameObject*[m_nObjects];
	m_ppObjects[0] = new CGameObject();

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvSrvGPUDescriptorStartHandle = GetGPUDescriptorHandle();

	CRoofMeshIlluminated *RoofMesh = new CRoofMeshIlluminated(pd3dDevice, pd3dCommandList);
	m_ppObjects[0]->SetMesh(RoofMesh);
	m_ppObjects[0]->m_nMaterial = Material_Roof;
	m_ppObjects[0]->SetObjectType(ObjType::ROOF);
	m_ppObjects[0]->SetPosition(475.0f, 370.0f, 550.0f);
	m_ppObjects[0]->SetCbvGPUDescriptorHandle(d3dCbvSrvGPUDescriptorStartHandle.ptr);
}


//////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// Enemy Shader //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

void CEnemyShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList, void *pContext, int nFileNumber)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;
	float fTerrainWidth = pTerrain->GetWidth(), fTerrainLength = pTerrain->GetLength();

	XMFLOAT3 startPos(1450.0f, 210.0f, 1900.0f);
	float fySize = 25.0f;

	default_random_engine dre(time(NULL));
	uniform_real_distribution<float> frd(-2.5f, 2.5f);

	//직육면체를 지형 표면에 그리고 지형보다 높은 위치에 파일에서 읽은 대로 배치한다.
	int xObjects = 20, yObjects = 1, zObjects = 20;
	m_nObjects = 50;
	m_ppObjects = new CGameObject*[m_nObjects];

	CSphereMeshIlluminated *pSphereMesh = new CSphereMeshIlluminated(pd3dDevice,
		pd3dCommandList, 12.5f, 20, 20);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvSrvGPUDescriptorStartHandle = GetGPUDescriptorHandle();

	CEnemyObject *pGameObject = NULL;
	XMFLOAT3 xmf3Up(0.0f, 1.0f, 0.0f), xmf3RotationAxis;
	for (int i = 0; i < m_nObjects; ++i)
	{
		pGameObject = new CEnemyObject();
		float xPosition = startPos.x + rand() % 500;
		float zPosition = startPos.z + rand() % 500;
		float fHeight = pTerrain->GetHeight(xPosition, zPosition);
		while (Vector3::Length(pGameObject->GetVelocity()) < 0.5f)
			pGameObject->SetVelocity(XMFLOAT3(frd(dre),0.0f, frd(dre)));
		
		
		// 진행 방향과 up벡터를 외적하여 회전축을 얻는다.
		XMStoreFloat3(&xmf3RotationAxis, XMVector3Cross(XMLoadFloat3(&pGameObject->GetVelocity()), XMLoadFloat3(&xmf3Up)));
		xmf3RotationAxis = Vector3::Normalize(xmf3RotationAxis);

		// 구한 회전축을 로컬 좌표계로 변경한다.
		pGameObject->SetRotationAxis(Vector3::TransformNormal(Vector3::Normalize(xmf3RotationAxis),
			XMLoadFloat4x4(&Matrix4x4::Inverse(pGameObject->GetWorldTransform(0)))));
		pGameObject->SetMesh(pSphereMesh);
		pGameObject->SetPosition(xPosition, fHeight + fySize / 2, zPosition);
		pGameObject->SetObjUpdatedContext(pContext);
		pGameObject->SetGravity(XMFLOAT3(0.0f, -250.0f, 0.0f));	// 플레이어와 동일한 중력 적용
		pGameObject->SetObjectType(ObjType::ENEMY);
		pGameObject->m_nMaterial = (i % 2 == 0) ? Material_Enemy : Material_Enemy2;
		pGameObject->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(12.5f, 12.5f, 12.5f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
		pGameObject->SetCbvGPUDescriptorHandle(d3dCbvSrvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
		m_ppObjects[i] = pGameObject;
	}
}


// 적 오브젝트간 충돌 검사
void CEnemyShader::CollisionCheck()
{
	XMFLOAT3 xmf3Temp;
	for (int i = 0; i < m_nObjects-1; i++)
	{
		if (m_ppObjects[i]->GetActive() == false) continue;
		for (int j = i+1; j < m_nObjects; j++)
		{ 
			if (m_ppObjects[j]->GetActive() == false) continue;
			// 완전 탄성 충돌을 가정
			if (m_ppObjects[i]->m_xmOOBB.Intersects(m_ppObjects[j]->m_xmOOBB))
			{
				// 속도 교환
				xmf3Temp = m_ppObjects[i]->GetVelocity();
				m_ppObjects[i]->SetVelocity(m_ppObjects[j]->GetVelocity());
				m_ppObjects[j]->SetVelocity(xmf3Temp);
				// 회전 축 교환
				xmf3Temp = m_ppObjects[i]->GetRotationoAxis();
				m_ppObjects[i]->SetRotationAxis(m_ppObjects[j]->GetRotationoAxis());
				m_ppObjects[j]->SetRotationAxis(xmf3Temp);
				break;
			}
		}
	}
}


// 플레이어와 충돌 검사
void CEnemyShader::CollisionCheck(CPlayer *pPlayer)
{
	if (pPlayer->GetActive() == false) 	return;

	for (int i = 0; i < m_nObjects; i++)
	{
		if (m_ppObjects[i]->GetActive() == false) continue;
		if (m_ppObjects[i]->m_xmOOBB.Intersects(pPlayer->m_xmOOBB))
		{
			// 플레이어와 부딪히면 튕긴다
			XMFLOAT3 playerSpeed = pPlayer->GetRealVelocity();
			playerSpeed.y = 0.0f;
			m_ppObjects[i]->SetVelocity(Vector3::Add(playerSpeed, m_ppObjects[i]->GetVelocity()));
			break;
		}
	}
}


void CEnemyShader::AnimateObjects(float fTimeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(fTimeElapsed, &m_xmf3PlayerPos);
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////


void CDayCycleShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList, void *pContext, int nFileNumber)
{
	m_nObjects = 2;
	m_ppObjects = new CGameObject*[m_nObjects];

	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	CSphereMeshIlluminated *pSphereMesh = new CSphereMeshIlluminated(pd3dDevice,
		pd3dCommandList, 100.0f, 20, 20);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvSrvGPUDescriptorStartHandle = GetGPUDescriptorHandle();

	COrbitObject *pGameObject = NULL;
	// 공전할 해, 달 설정
	for (int i = 0; i < 2; ++i)
	{
		pGameObject = new COrbitObject();

		// 구한 회전축을 로컬 좌표계로 변경한다.
		pGameObject->SetRevolutionAxis(XMFLOAT3(0.0f, 0.0f, 1.0f));
		pGameObject->SetPosition(0, 0, pTerrain->GetLength() * 0.5f);
		pGameObject->SetCenter(pTerrain->GetWidth() * 0.5f);
		pGameObject->SetRadius(pTerrain->GetWidth() * 0.55f);
		pGameObject->SetDegree(180.0f * i);
		pGameObject->SetRevolutionSpeed(30.0f);
		pGameObject->SetMesh(pSphereMesh);
		pGameObject->m_nMaterial = (i==0?Material_Sun:Material_Moon);
		pGameObject->SetCbvGPUDescriptorHandle(d3dCbvSrvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
		m_ppObjects[i] = pGameObject;
	}
}