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



//////////////////////// PlayerShader //////////////////////

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
	*pd3dCommandList, float fTotalSize)
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


bool CObjectsShader::AnimateObjects(float fTimeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(fTimeElapsed);
	}
	return false;
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

	//for (int j =1; j < m_nObjects; j++)
	//{
	//	if (m_ppObjects[j])
	//	{
	//		m_ppObjects[j]->Render(pd3dCommandList, pCamera);
	//	}
	//}
}


void CObjectsShader::CollisionWalls(XMFLOAT4 *pxmf4WallPlanes, CWallObject *pWalls) 
{
	//���� ��ü �浹 �˻�
	for (int i = 0; i < m_nObjects; i++)
	{
		ContainmentType containType = pWalls[0].m_xmOOBB.Contains(m_ppObjects[i]->m_xmOOBB);
		switch (containType)
		{
		case DISJOINT:
		{
			int nPlaneIndex = -1;
			for (int j = 0; j < 4; j++)
			{
				PlaneIntersectionType intersectType = m_ppObjects[i]->m_xmOOBB.Intersects(XMLoadFloat4(&pxmf4WallPlanes[j]));
				if (intersectType == BACK)
				{
					nPlaneIndex = j;
					break;
				}
			}
			if (nPlaneIndex != -1)
			{
				XMVECTOR xmvNormal = XMVectorSet(pxmf4WallPlanes[nPlaneIndex].x, pxmf4WallPlanes[nPlaneIndex].y, pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
				XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(&m_ppObjects[i]->GetMovingDirection()), xmvNormal);
				XMFLOAT3 xmf3Temp;
				XMStoreFloat3(&xmf3Temp, xmvReflect);
				m_ppObjects[i]->SetMovingDirection(xmf3Temp);
			}
			break;
		}
		case INTERSECTS:
		{
			int nPlaneIndex = -1;
			for (int j = 0; j < 4; j++)
			{
				PlaneIntersectionType intersectType = m_ppObjects[i]->m_xmOOBB.Intersects(XMLoadFloat4(&pxmf4WallPlanes[j]));
				if (intersectType == INTERSECTING)
				{
					nPlaneIndex = j;
					break;
				}
			}
			if (nPlaneIndex != -1)
			{
				XMVECTOR xmvNormal = XMVectorSet(pxmf4WallPlanes[nPlaneIndex].x, pxmf4WallPlanes[nPlaneIndex].y, pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
				XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(&m_ppObjects[i]->GetMovingDirection()), xmvNormal);
				XMFLOAT3 xmf3Temp;
				XMStoreFloat3(&xmf3Temp, xmvReflect);
				m_ppObjects[i]->SetMovingDirection(xmf3Temp);
			}
			break;
		}
		case CONTAINS:
			break;
		}
	}
}


bool CObjectsShader::CollisionPlayer(CPlayer *pPlayer, list<CParticleObject>& particles)
{
	if (pPlayer->m_bRender == false) 	return false;

	bool result = false;
	for (int i = 0; i < m_nObjects; i++)
	{
		if (m_ppObjects[i]->m_bRender == FALSE)
			continue;
		if (m_ppObjects[i]->m_xmOOBB.Intersects(pPlayer->m_xmOOBB))
		{
			//m_lParticles.push_back(CParticle(m_ppObjects[i], 35.0f));
			//m_lParticles.push_back(CParticle(pPlayer, 35.0f));

			m_ppObjects[i]->m_bRender = FALSE;
			pPlayer->m_bRender = FALSE;

			pPlayer->ChangeModel();

			particles.push_back(CParticleObject(*m_ppObjects[i]));
			particles.push_back(CParticleObject(*pPlayer));

			result = true;
			break;
		}
	}
	return result;
}

bool CObjectsShader::CollisionBullets(list<CBulletObject>& bullets, list<CParticleObject>& particles)
{
	bool result = false;
	for (CBulletObject& bullet : bullets) {
		if (bullet.m_bRender == false)
			continue;

		for (int i = 0; i < m_nObjects; i++)
		{
			if (m_ppObjects[i]->m_bRender == false)
				continue; 
			if (m_ppObjects[i]->m_xmOOBB.Intersects(bullet.m_xmOOBB))
			{
				//m_lParticles.push_back(CParticle(&m_pObjects[i], 15.0f));

				m_ppObjects[i]->m_bRender = false;
				bullet.m_bRender = false;

				particles.push_back(CParticleObject(*m_ppObjects[i]));

				result = true;
				break;
			}
			else if (m_ppObjects[i]->m_xmOOBB.Contains(bullet.m_xmOOBB) == (CONTAINS))
			{
				//m_lParticles.push_back(CParticle(&m_pObjects[i], 15.0f));

				m_ppObjects[i]->m_bRender = false;
				bullet.m_bRender = false;

				particles.push_back(CParticleObject(*m_ppObjects[i]));

				result = true;
				break;
			}
		}
	}
	return result;
}

void CObjectsShader::CollisionSelf()
{
	// �浹�� ���� �ٲٱ� ���ؼ� �ʿ��� ������
	XMFLOAT3 xmf3MovingDirection;
	float fMovingSpeed;
	Directions directionKind;
	XMFLOAT4 xmf4Color;

	for (int i = 0; i < m_nObjects; i++)
	{
		if (m_ppObjects[i]->m_bRender == false)
			continue;
		for (int j = (i + 1); j < m_nObjects; j++)
		{
			if (m_ppObjects[j]->m_bRender == false)
				continue;
			if (m_ppObjects[i]->m_xmOOBB.Intersects(m_ppObjects[j]->m_xmOOBB))
			{
				xmf3MovingDirection = m_ppObjects[j]->GetMovingDirection();
				fMovingSpeed = m_ppObjects[j]->GetMovingSpeed();
				directionKind = m_ppObjects[j]->GetDirectionKind();
				xmf4Color = m_ppObjects[j]->GetColor();

				m_ppObjects[j]->SetMovingDirection(m_ppObjects[i]->GetMovingDirection());
				m_ppObjects[j]->SetMovingSpeed(m_ppObjects[i]->GetMovingSpeed());
				m_ppObjects[j]->SetDirectionKind(m_ppObjects[i]->GetDirectionKind());
				m_ppObjects[j]->SetColor(m_ppObjects[i]->GetColor());

				m_ppObjects[i]->SetMovingDirection(xmf3MovingDirection);
				m_ppObjects[i]->SetMovingSpeed(fMovingSpeed);
				m_ppObjects[i]->SetDirectionKind(directionKind);
				m_ppObjects[i]->SetColor(xmf4Color);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////
///////////////////////// Instancing Shader /////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

CInstancingShader::CInstancingShader()
{
	prePlayerPos = curPlayerPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
}


CInstancingShader::~CInstancingShader()
{
}


bool CInstancingShader::AnimateObjects(float fTimeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(fTimeElapsed);
		m_ppObjects[j]->FarChecker(curPlayerPos.z, prePlayerPos.z);
	}
	return false;
}


D3D12_INPUT_LAYOUT_DESC CInstancingShader::CreateInputLayout()
{
	UINT nInputElementDescs = 3;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new
		D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	//���� ������ ���� �Է� �����̴�.
	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "RENDER", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, 28,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}


D3D12_SHADER_BYTECODE CInstancingShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSInstancing", "vs_5_1",
		ppd3dShaderBlob));
}


D3D12_SHADER_BYTECODE CInstancingShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSInstancing", "ps_5_1",
		ppd3dShaderBlob));
}


void CInstancingShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature
	*pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}


void CInstancingShader::CreateShaderVariables(ID3D12Device *pd3dDevice,
	ID3D12GraphicsCommandList *pd3dCommandList)
{
	//�ν��Ͻ� ������ ������ ���� ���۸� ���ε� �� �������� �����Ѵ�.
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL,
		sizeof(VS_VB_INSTANCE) * m_nObjects, D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_GENERIC_READ, NULL);

	//���� ����(���ε� ��)�� ���� �����͸� �����Ѵ�.
	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}


void CInstancingShader::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects) m_pd3dcbGameObjects->Unmap(0, NULL);
	if (m_pd3dcbGameObjects) m_pd3dcbGameObjects->Release();
}


//�ν��Ͻ� ����(��ü�� ���� ��ȯ ��İ� ����)�� ���� ���ۿ� �����Ѵ�.
void CInstancingShader::UpdateShaderVariables(ID3D12GraphicsCommandList
	*pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootShaderResourceView(2,
		m_pd3dcbGameObjects->GetGPUVirtualAddress());

	for (int j = 0; j < m_nObjects; j++)
	{
		m_pcbMappedGameObjects[j].m_bRender = m_ppObjects[j]->m_bRender;
		if (m_ppObjects[j]->GetDirectionKind() == Directions::DX)
			m_pcbMappedGameObjects[j].m_xmcColor = XMFLOAT4(0.0f, 0.0f, 0.7f, 0.0f);

		else if (m_ppObjects[j]->GetDirectionKind() == Directions::DY)
			m_pcbMappedGameObjects[j].m_xmcColor = XMFLOAT4(0.7f, 0.0f, 0.0f, 0.0f);

		else if (m_ppObjects[j]->GetDirectionKind() == Directions::DRANDOM)
			m_pcbMappedGameObjects[j].m_xmcColor = m_ppObjects[j]->GetColor();

		XMStoreFloat4x4(&m_pcbMappedGameObjects[j].m_xmf4x4Transform,
			XMMatrixTranspose(XMLoadFloat4x4(&m_ppObjects[j]->m_xmf4x4World)));
	}
}


void CInstancingShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList, float fTotalSize)
{
	uniform_real_distribution<float>zDst(-fTotalSize, fTotalSize);
	default_random_engine dre((unsigned)time(NULL));

	m_nObjects = 80;

	m_ppObjects = new CRotatingObject*[m_nObjects];

	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0; i < m_nObjects; ++i)
	{
		pRotatingObject = new CRotatingObject();
		pRotatingObject->SetTotalSize(fTotalSize);
		pRotatingObject->Reset();
		XMFLOAT3 pos = pRotatingObject->GetPosition();
		pos.z = zDst(dre);
		pRotatingObject->SetPosition(pos);
		pRotatingObject->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(4.0f, 4.0f, 4.0f), XMFLOAT4(0.0f, 0.0f, 0.0f,1.0f));
		m_ppObjects[i] = pRotatingObject;
	}

	//�ν��Ͻ��� ����Ͽ� �������ϱ� ���Ͽ� �ϳ��� ���� ��ü�� �޽��� ������.
	CCubeMeshDiffused *pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList,
		8.0f, 8.0f, 8.0f);
	m_ppObjects[0]->SetMesh(pCubeMesh);

	//�ν��Ͻ��� ���� ����(Structured Buffer)�� �����Ѵ�.
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}


void CInstancingShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera
	*pCamera)
{
	CObjectsShader::Render(pd3dCommandList, pCamera);

	//��� ���� ��ü�� �ν��Ͻ� �����͸� ���ۿ� �����Ѵ�.
	UpdateShaderVariables(pd3dCommandList);

	//�ϳ��� ���� �����͸� ����Ͽ� ��� ���� ��ü(�ν��Ͻ�)���� �������Ѵ�.
	m_ppObjects[0]->Render(pd3dCommandList, pCamera, m_nObjects);
}


void CInstancingShader::SetPlayerPos(XMFLOAT3 position)
{
	prePlayerPos = curPlayerPos;
	curPlayerPos = position;
}


//////////////// Particle Shader ////////////////////////////


CParticleShader::CParticleShader()
{
}


CParticleShader::~CParticleShader()
{
}


bool CParticleShader::AnimateObjects(float fTimeElapsed)
{
	bool result = false;

	for (CParticleObject& part : *m_plParticles)
	{
		part.Animate(fTimeElapsed);
		result |= part.FarChecker();
	}
	m_plParticles->remove_if([](CParticleObject& part) { return part.m_bRender==false; });
	return result;
}


void CParticleShader::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList)
{
	if (!m_plParticles) return;

	//�ν��Ͻ� ������ ������ ���� ���۸� ���ε� �� �������� �����Ѵ�.
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL,
		sizeof(VS_VB_INSTANCE) * m_plParticles->size() * PARTICLE_COUNT, D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_GENERIC_READ, NULL);

	//���� ����(���ε� ��)�� ���� �����͸� �����Ѵ�.
	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}


void CParticleShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	if (!m_pd3dcbGameObjects) return;

	pd3dCommandList->SetGraphicsRootShaderResourceView(2,
		m_pd3dcbGameObjects->GetGPUVirtualAddress());
	int i = 0;

	for (CParticleObject& part : *m_plParticles)
	{
		for (int j = 0; j < PARTICLE_COUNT; ++j)
		{
			m_pcbMappedGameObjects[i * PARTICLE_COUNT + j].m_bRender = true;
			if (part.GetDirectionKind(j) == Directions::DX)
				m_pcbMappedGameObjects[i * PARTICLE_COUNT + j].m_xmcColor = XMFLOAT4(0.0f, 0.0f, 0.7f, 0.0f);

			else if (part.GetDirectionKind(j) == Directions::DY)
				m_pcbMappedGameObjects[i * PARTICLE_COUNT + j].m_xmcColor = XMFLOAT4(0.7f, 0.0f, 0.0f, 0.0f);

			else if (part.GetDirectionKind(j) == Directions::DRANDOM)
				m_pcbMappedGameObjects[i * PARTICLE_COUNT + j].m_xmcColor = part.GetColor(j);

			XMStoreFloat4x4(&m_pcbMappedGameObjects[i * PARTICLE_COUNT + j].m_xmf4x4Transform,
				XMMatrixTranspose(XMLoadFloat4x4(&part.GetParticleWorld(j))));
		}
		++i;
	}
}


void CParticleShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList, float fTotalSize)
{
	CCubeMeshDiffused *pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList,
		2.0f, 2.0f, 2.0f);

	ParticleRenderer.SetMesh(pCubeMesh);
}


void CParticleShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	if (!m_pd3dcbGameObjects) return;

	CObjectsShader::Render(pd3dCommandList, pCamera);

	//��� ���� ��ü�� �ν��Ͻ� �����͸� ���ۿ� �����Ѵ�.
	UpdateShaderVariables(pd3dCommandList);

	//�ϳ��� ���� �����͸� ����Ͽ� ��� ���� ��ü(�ν��Ͻ�)���� �������Ѵ�.
	ParticleRenderer.Render(pd3dCommandList, pCamera, m_plParticles->size() * PARTICLE_COUNT);
}


void CParticleShader::Reset(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	ReleaseShaderVariables();
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}