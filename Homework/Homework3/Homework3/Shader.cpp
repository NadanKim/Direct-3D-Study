#include "stdafx.h"
#include "Shader.h"


CShader::CShader()
{
}


CShader::~CShader()
{
	if (m_ppd3dPipelineStates)
	{
		for (int i = 0; i < m_nPipelineStates; i++) if (m_ppd3dPipelineStates[i])
			m_ppd3dPipelineStates[i]->Release();
		delete[] m_ppd3dPipelineStates;
	}
}


//�����Ͷ����� ���¸� �����ϱ� ���� ����ü�� ��ȯ�Ѵ�.
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


//����-���ٽ� �˻縦 ���� ���¸� �����ϱ� ���� ����ü�� ��ȯ�Ѵ�.
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


//���� ���¸� �����ϱ� ���� ����ü�� ��ȯ�Ѵ�.
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


//�Է� �����⿡�� ���� ������ ������ �˷��ֱ� ���� ����ü�� ��ȯ�Ѵ�.
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


//���̴� �ҽ� �ڵ带 �������Ͽ� ����Ʈ �ڵ� ����ü�� ��ȯ�Ѵ�.
D3D12_SHADER_BYTECODE CShader::CompileShaderFromFile(WCHAR *pszFileName, LPCSTR
	pszShaderName, LPCSTR pszShaderProfile, ID3DBlob **ppd3dShaderBlob)
{
	UINT nCompileFlags = 0;
#if defined(_DEBUG)
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	::D3DCompileFromFile(pszFileName, NULL, NULL, pszShaderName, pszShaderProfile,
		nCompileFlags, 0, ppd3dShaderBlob, NULL);

	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
	d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();

	return(d3dShaderByteCode);
}


//�׷��Ƚ� ���������� ���� ��ü�� �����Ѵ�.
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
	*pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);
}


void CShader::ReleaseShaderVariables()
{
}


void CShader::OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList)
{
	//���������ο� �׷��Ƚ� ���� ��ü�� �����Ѵ�.
	pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[0]);
}


void CShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	OnPrepareRender(pd3dCommandList);
}



//////////////////////// DiffusedShader //////////////////////

CPlayerShader::CPlayerShader()
{
}


CPlayerShader::~CPlayerShader()
{
}


D3D12_INPUT_LAYOUT_DESC CPlayerShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new
		D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}


D3D12_SHADER_BYTECODE CPlayerShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSDiffused", "vs_5_1",
		ppd3dShaderBlob));
}


D3D12_SHADER_BYTECODE CPlayerShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSDiffused", "ps_5_1",
		ppd3dShaderBlob));
}


void CPlayerShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature
	*pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}

//////////////////////ObjectsShader////////////////////////////

CObjectsShader::CObjectsShader()
{
}


CObjectsShader::~CObjectsShader()
{
}


D3D12_INPUT_LAYOUT_DESC CObjectsShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new
		D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}


D3D12_SHADER_BYTECODE CObjectsShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSDiffused", "vs_5_1",
		ppd3dShaderBlob));
}


D3D12_SHADER_BYTECODE CObjectsShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSDiffused", "ps_5_1",
		ppd3dShaderBlob));
}


void CObjectsShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature
	*pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
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


CTerrainShader::CTerrainShader()
{
}


CTerrainShader::~CTerrainShader()
{
}


D3D12_INPUT_LAYOUT_DESC CTerrainShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new
		D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}


D3D12_SHADER_BYTECODE CTerrainShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSDiffused", "vs_5_1",
		ppd3dShaderBlob));
}


D3D12_SHADER_BYTECODE CTerrainShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSDiffused", "ps_5_1",
		ppd3dShaderBlob));
}


void CTerrainShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature
	*pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}



//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// Miro Shader //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

CMiroShader::CMiroShader()
{
}


CMiroShader::~CMiroShader()
{
}


D3D12_INPUT_LAYOUT_DESC CMiroShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new
		D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	//���� ������ ���� �Է� �����̴�.
	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}


D3D12_SHADER_BYTECODE CMiroShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSInstancing", "vs_5_1",
		ppd3dShaderBlob));
}


D3D12_SHADER_BYTECODE CMiroShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSInstancing", "ps_5_1",
		ppd3dShaderBlob));
}


void CMiroShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature
	*pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}


void CMiroShader::CreateShaderVariables(ID3D12Device *pd3dDevice,
	ID3D12GraphicsCommandList *pd3dCommandList)
{
	//�ν��Ͻ� ������ ������ ���� ���۸� ���ε� �� �������� �����Ѵ�.
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL,
		sizeof(VS_VB_INSTANCE) * m_nObjects, D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_GENERIC_READ, NULL);

	//���� ����(���ε� ��)�� ���� �����͸� �����Ѵ�.
	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}


void CMiroShader::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects) m_pd3dcbGameObjects->Unmap(0, NULL);
	if (m_pd3dcbGameObjects) m_pd3dcbGameObjects->Release();
}


//�ν��Ͻ� ����(��ü�� ���� ��ȯ ��İ� ����)�� ���� ���ۿ� �����Ѵ�.
void CMiroShader::UpdateShaderVariables(ID3D12GraphicsCommandList
	*pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootShaderResourceView(2,
		m_pd3dcbGameObjects->GetGPUVirtualAddress());
	for (int j = 0; j < m_nObjects; j++)
	{
		m_pcbMappedGameObjects[j].m_xmcColor = (j % 2) ? XMFLOAT4(0.5f, 0.0f, 0.0f, 0.0f) :
			XMFLOAT4(0.0f, 0.0f, 0.5f, 0.0f);
		XMStoreFloat4x4(&m_pcbMappedGameObjects[j].m_xmf4x4Transform,
			XMMatrixTranspose(XMLoadFloat4x4(&m_ppObjects[j]->m_xmf4x4World)));
	}
}


void CMiroShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList, void *pContext, int nFileNumber)
{
	// �� ����

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

	int wallCount = 0;
	for (int i = 0; i < 20*20; ++i)
			if (mapArr[i] == WALL)
				++wallCount;

	XMFLOAT3 startPos(175.0f, 210.0f, 250.0f);
	float fxzSize = 30.0f;
	float fySize = 200.0f;

	//������ü�� ���� ǥ�鿡 �׸��� �������� ���� ��ġ�� ���Ͽ��� ���� ��� ��ġ�Ѵ�.
	int xObjects = 20, yObjects = 1, zObjects = 20;
	m_nObjects = wallCount;
	m_ppObjects = new CGameObject*[m_nObjects];

	CGameObject *pGameObject = NULL;
	for (int i = 0, x = 0; x < xObjects; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			if (mapArr[x*xObjects + z] != 1) continue;
			pGameObject = new CGameObject(1);
			float xPosition = startPos.x + x * fxzSize;
			float zPosition = startPos.z + z * fxzSize;
			float fHeight = pTerrain->GetHeight(xPosition, zPosition);
			pGameObject->SetPosition(xPosition, fHeight + fySize / 2,  zPosition);
			pGameObject->SetObjectType(ObjType::WALL);
			pGameObject->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(15.0f, 100.0f, 15.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
			m_ppObjects[i++] = pGameObject;
		}
	}

	//�ν��Ͻ��� ����Ͽ� �������ϱ� ���Ͽ� �ϳ��� ���� ��ü�� �޽��� ������.
	CCubeMeshDiffused *pCubeMesh = new CCubeMeshDiffused(pd3dDevice,
		pd3dCommandList, 30.0f, 200.0f, 30.0f);
	m_ppObjects[0]->SetMesh(pCubeMesh);

	//�ν��Ͻ��� ���� ����(Structured Buffer)�� �����Ѵ�.
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}


void CMiroShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CShader::Render(pd3dCommandList, pCamera);

	//��� ���� ��ü�� �ν��Ͻ� �����͸� ���ۿ� �����Ѵ�.
	UpdateShaderVariables(pd3dCommandList);

	//�ϳ��� ���� �����͸� ����Ͽ� ��� ���� ��ü(�ν��Ͻ�)���� �������Ѵ�.
	m_ppObjects[0]->Render(pd3dCommandList, pCamera, m_nObjects);
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
			// �÷��̾��� ���� ��ġ�� ������Ʈ�� ��ġ�� ������ ����� 
			// ������Ʈ�� � ��� ���� ���� �������� �˻��Ͽ�  ��ֺ��͸� ���´�.
			XMFLOAT3 xmfNormal = m_ppObjects[i]->CollisionByRayIntersection(pPlayer->GetPosition(), m_ppObjects[0]->GetMesh());
			XMVECTOR xmvNormal = XMLoadFloat3(&xmfNormal);
			XMVECTOR xmvProj = XMVector3Dot(XMLoadFloat3(&pPlayer->GetVelocity()), xmvNormal)*xmvNormal;
			XMStoreFloat(&reflectSpeed, XMVector3Length(xmvProj));

			pPlayer->Move(Vector3::ScalarProduct(xmfNormal, reflectSpeed));
		}
	}
}


void CMiroShader::CollisionCheck(CGameObject *pObject, bool Bullet)
{
	if (pObject->GetActive() == false) 	return;

	// �ٸ� ������Ʈ���� �˻��� ���
	if (Bullet == false) 
	{
		int i = 0;
		for (; i < m_nObjects; ++i)
		{
			if (m_ppObjects[i]->m_xmOOBB.Intersects(pObject->m_xmOOBB))
			{
				float reflectSpeed;
				// �÷��̾��� ���� ��ġ�� ������Ʈ�� ��ġ�� ������ ����� 
				// ������Ʈ�� � ��� ���� ���� �������� �˻��Ͽ�  ��ֺ��͸� ���´�.
				XMFLOAT3 xmfNormal = m_ppObjects[i]->CollisionByRayIntersection(pObject->GetPosition(), m_ppObjects[0]->GetMesh());
				XMVECTOR xmvNormal = XMLoadFloat3(&xmfNormal);
				XMVECTOR xmvProj = XMVector3Dot(XMLoadFloat3(&pObject->GetVelocity()), xmvNormal)*xmvNormal;
				XMStoreFloat(&reflectSpeed, XMVector3Length(xmvProj));

				pObject->Move(xmfNormal, reflectSpeed);
			}
		}
	}
	// �Ѿ˰��� �˻�
	else
	{
		for (int i = 0; i < m_nObjects; ++i)
		{
			if (m_ppObjects[i]->m_xmOOBB.Intersects(pObject->m_xmOOBB))
			{
				pObject->SetPosition(0.0f, -1000.0f, 0.0f);
				pObject->SetActive(false);
				break;
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// Roof Shader ////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

void CRoofShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList, void *pContext, int nFileNumber)
{
	Roof = new CGameObject();
	CRoofMeshDiffussed *RoofMesh = new CRoofMeshDiffussed(pd3dDevice, pd3dCommandList);
	Roof->SetMesh(RoofMesh);
	Roof->SetObjectType(ObjType::ROOF);
	Roof->SetPosition(475.0f, 370.0f, 550.0f);

	//�ν��Ͻ��� ���� ����(Structured Buffer)�� �����Ѵ�.
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}


void CRoofShader::ReleaseObjects()
{
	Roof->Release();
}


void CRoofShader::ReleaseUploadBuffers()
{
	Roof->ReleaseUploadBuffers();
}

void CRoofShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CShader::Render(pd3dCommandList, pCamera);

	if (Roof) Roof->Render(pd3dCommandList, pCamera);
}


//////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// Enemy Shader //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

CEnemyShader::CEnemyShader()
{
}

CEnemyShader::~CEnemyShader()
{
}


void CEnemyShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootShaderResourceView(2,
		m_pd3dcbGameObjects->GetGPUVirtualAddress());
	for (int j = 0; j < m_nObjects; j++)
	{
		m_pcbMappedGameObjects[j].m_xmcColor = (j % 2) ? XMFLOAT4(0.5f, 0.5f, 0.0f, 0.0f) :
			XMFLOAT4(0.0f, 0.5f, 0.5f, 0.0f);
		XMStoreFloat4x4(&m_pcbMappedGameObjects[j].m_xmf4x4Transform,
			XMMatrixTranspose(XMLoadFloat4x4(&m_ppObjects[j]->m_xmf4x4World)));
	}
}


void CEnemyShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList, void *pContext, int nFileNumber)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;
	float fTerrainWidth = pTerrain->GetWidth(), fTerrainLength = pTerrain->GetLength();

	int mapArr[20 * 20];

	string sFileName = "Map/map" + to_string(nFileNumber) + ".txt";

	ifstream in(sFileName);
	for (int i = 0; i < 20; ++i) {
		for (int j = 0; j < 20; ++j) {
			in >> mapArr[i * 20 + j];
		}
	}
	in.close();

	int enemyCount = 0;
	for (int i = 0; i < 20 * 20; ++i)
		if (mapArr[i] == ENEMY)
			++enemyCount;

	XMFLOAT3 startPos(175.0f, 210.0f, 250.0f);
	float fxzSize = 30.0f;
	float fySize = 25.0f;

	//������ü�� ���� ǥ�鿡 �׸��� �������� ���� ��ġ�� ���Ͽ��� ���� ��� ��ġ�Ѵ�.
	int xObjects = 20, yObjects = 1, zObjects = 20;
	m_nObjects = enemyCount;
	m_ppObjects = new CGameObject*[m_nObjects];

	CEnemyObject *pGameObject = NULL;
	for (int i = 0, x = 0; x < xObjects; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			if (mapArr[x*xObjects + z] != 2) continue;
			pGameObject = new CEnemyObject(1);
			float xPosition = startPos.x + x * fxzSize;
			float zPosition = startPos.z + z * fxzSize;
			float fHeight = pTerrain->GetHeight(xPosition, zPosition);
			pGameObject->SetPosition(xPosition, fHeight + fySize / 2, zPosition);
			pGameObject->SetObjUpdatedContext(pContext);
			pGameObject->SetGravity(XMFLOAT3(0.0f, -250.0f, 0.0f));	// �÷��̾�� ������ �߷� ����
			pGameObject->SetObjectType(ObjType::ENEMY);
			pGameObject->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(12.5f, 12.5f, 12.5f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
			m_ppObjects[i++] = pGameObject;
		}
	}

	//�ν��Ͻ��� ����Ͽ� �������ϱ� ���Ͽ� �ϳ��� ���� ��ü�� �޽��� ������.
	CSphereMeshDiffused *pSphereMesh = new CSphereMeshDiffused(pd3dDevice,
		pd3dCommandList, 12.5f, 20, 20);
	m_ppObjects[0]->SetMesh(pSphereMesh);

	//�ν��Ͻ��� ���� ����(Structured Buffer)�� �����Ѵ�.
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}


// �÷��̾�� �浹 �˻�
void CEnemyShader::CollisionCheck(CPlayer *pPlayer)
{
	if (pPlayer->GetActive() == false) 	return;

	for (int i = 0; i < m_nObjects; i++)
	{
		if (m_ppObjects[i]->GetActive() == false) continue;
		if (m_ppObjects[i]->m_xmOOBB.Intersects(pPlayer->m_xmOOBB))
		{
			m_pParticles->SetParticle(m_ppObjects[i]);
			m_pParticles->SetParticle(pPlayer);

			m_ppObjects[i]->SetPosition(0.0f, -1000.0f, 0.0f);

			m_ppObjects[i]->SetActive(false);
			pPlayer->SetActive(false);
			break;
		}
	}
}


// �Ѿ˰� �浹 �˻�
void CEnemyShader::CollisionCheck(CGameObject *pObject, bool Bullet)
{
	if (pObject->GetActive() == false) 	return;

	for (int i = 0; i < m_nObjects; i++)
	{
		if (m_ppObjects[i]->GetActive() == false) continue;
		if (m_ppObjects[i]->m_xmOOBB.Intersects(pObject->m_xmOOBB))
		{
			m_pParticles->SetParticle(m_ppObjects[i]);

			m_ppObjects[i]->SetPosition(0.0f, -1000.0f, 0.0f);
			pObject->SetPosition(0.0f, -1000.0f, 0.0f);

			m_ppObjects[i]->SetActive(false);
			pObject->SetActive(false);
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

/////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// Bullet Shader ///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

CBulletShader::CBulletShader()
{

}


CBulletShader::~CBulletShader()
{
}


void CBulletShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootShaderResourceView(2,
		m_pd3dcbGameObjects->GetGPUVirtualAddress());
	for (int j = 0; j < m_nObjects; j++)
	{
		m_pcbMappedGameObjects[j].m_xmcColor = (j % 2) ? XMFLOAT4(0.5f, 0.2f, 0.2f, 0.0f) :
			XMFLOAT4(0.46f, 0.45f, 0.0f, 0.0f);
		XMStoreFloat4x4(&m_pcbMappedGameObjects[j].m_xmf4x4Transform,
			XMMatrixTranspose(XMLoadFloat4x4(&m_ppObjects[j]->m_xmf4x4World)));
	}
}


void CBulletShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList, void *pContext, int nFileNumber)
{
	m_nObjects = BULLET_MAX;
	m_ppObjects = new CGameObject*[m_nObjects];

	CBulletObject *pGameObject = NULL;
	for (int i = 0; i < BULLET_MAX; ) {
		pGameObject = new CBulletObject(1);
		// ���ʿ� ������ �ʵ��� ����
		pGameObject->SetPosition(0.0f, -1000.0f, 0.0f);
		pGameObject->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(2.5f, 2.5f, 2.5f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
		m_ppObjects[i++] = pGameObject;
	}

	//�ν��Ͻ��� ����Ͽ� �������ϱ� ���Ͽ� �ϳ��� ���� ��ü�� �޽��� ������.
	CFragmentMeshDiffused *pBulletMesh = new CFragmentMeshDiffused(pd3dDevice,
		pd3dCommandList, 5.0f, 5.0f, 5.0f);
	m_ppObjects[0]->SetMesh(pBulletMesh);

	//�ν��Ͻ��� ���� ����(Structured Buffer)�� �����Ѵ�.
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}


void CBulletShader::SetBullet(CPlayer *pPlayer)
{
	m_ppObjects[m_nCurrentBullet]->SetPosition(pPlayer->GetPosition());
	m_ppObjects[m_nCurrentBullet]->SetDirection(pPlayer->GetLookVector());
	m_ppObjects[m_nCurrentBullet++]->SetActive(true);

	if (m_nCurrentBullet >= BULLET_MAX)
		m_nCurrentBullet = 0;
}



/////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// Particle Shader /////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

CParticleShader::CParticleShader()
{

}


CParticleShader::~CParticleShader()
{
}


void CParticleShader::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList)
{
	//�ν��Ͻ� ������ ������ ���� ���۸� ���ε� �� �������� �����Ѵ�.
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL,
		sizeof(VS_VB_INSTANCE) * m_nObjects * PARTICLE_COUNT, D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_GENERIC_READ, NULL);

	//���� ����(���ε� ��)�� ���� �����͸� �����Ѵ�.
	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}


void CParticleShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootShaderResourceView(2,
		m_pd3dcbGameObjects->GetGPUVirtualAddress());
	for (int i=0;i<m_nObjects; ++i)
	{
		for (int j = 0; j < PARTICLE_COUNT; ++j) {
			m_pcbMappedGameObjects[i*PARTICLE_COUNT +j].m_xmcColor = RANDOM_COLOR;
			XMStoreFloat4x4(&m_pcbMappedGameObjects[i*PARTICLE_COUNT + j].m_xmf4x4Transform,
				XMMatrixTranspose(XMLoadFloat4x4(&m_ppObjects[i]->GetWorldTransform(j))));
		}
	}
}


void CParticleShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList, void *pContext, int nFileNumber)
{
	// �ִ� ���� ������ ��ƼŬ �� 5 * �� �� 200�� = 1000�� �̹Ƿ� ������ 1000���� �ؼ� ���۸� ����
	m_nObjects = PARTICLE_MAX;
	m_ppObjects = new CGameObject*[m_nObjects];

	CParticleObject *pGameObject = NULL;
	for (int i = 0; i < m_nObjects; ) {
		pGameObject = new CParticleObject(1);
		// ���ʿ� ������ �ʵ��� ����
		pGameObject->SetPosition(0.0f, -1000.0f, 0.0f);
		m_ppObjects[i++] = pGameObject;
	}

	//�ν��Ͻ��� ����Ͽ� �������ϱ� ���Ͽ� �ϳ��� ���� ��ü�� �޽��� ������.
	CFragmentMeshDiffused *pParticleMesh = new CFragmentMeshDiffused(pd3dDevice,
		pd3dCommandList, 3.5f, 3.0f, 3.5f);
	ParticleRenderer.SetMesh(pParticleMesh);

	//�ν��Ͻ��� ���� ����(Structured Buffer)�� �����Ѵ�.
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}


void CParticleShader::SetParticle(CGameObject *pObject)
{
	m_ppObjects[m_nCurrentParticle]->SetPosition(pObject->GetPosition());
	m_ppObjects[m_nCurrentParticle]->SetParticles();
	m_ppObjects[m_nCurrentParticle++]->SetActive(true);

	if (m_nCurrentParticle >= PARTICLE_MAX)
		m_nCurrentParticle = 0;
}


void CParticleShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CShader::Render(pd3dCommandList, pCamera);

	//��� ���� ��ü�� �ν��Ͻ� �����͸� ���ۿ� �����Ѵ�.
	UpdateShaderVariables(pd3dCommandList);

	//�ϳ��� ���� �����͸� ����Ͽ� ��� ���� ��ü(�ν��Ͻ�)���� �������Ѵ�.
	ParticleRenderer.Render(pd3dCommandList, pCamera, m_nObjects * PARTICLE_COUNT);
}