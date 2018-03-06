//-----------------------------------------------------------------------------
// File: CGameObject.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Object.h"
#include "Shader.h"

#define WayPointRadius 10

XMFLOAT3 g_wayPoint[10]
{
	{ 625.0f, 347.0f, 683.0f },
	{ 538.0f, 368.0f, 603.0f },
	{ 663.0f, 370.0f, 573.0f },
	{ 713.0f, 363.0f, 415.0f },
	{ 611.0f, 348.0f, 272.0f },
	{ 468.0f, 326.0f, 359.0f },
	{ 347.0f, 338.0f, 390.0f },
	{ 245.0f, 302.0f, 468.0f },
	{ 107.0f, 304.0f, 489.0f },
	{ 12.0f, 300.0f, 379.0f }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CTexture::CTexture(int nTextures, UINT nTextureType, int nSamplers)
{
	m_nTextureType = nTextureType;
	m_nTextures = nTextures;
	if (m_nTextures > 0)
	{
		m_pRootArgumentInfos = new SRVROOTARGUMENTINFO[m_nTextures];
		m_ppd3dTextureUploadBuffers = new ID3D12Resource*[m_nTextures];
		m_ppd3dTextures = new ID3D12Resource*[m_nTextures];
	}

	m_nSamplers = nSamplers;
	if (m_nSamplers > 0) m_pd3dSamplerGpuDescriptorHandles = new D3D12_GPU_DESCRIPTOR_HANDLE[m_nSamplers];
}

CTexture::~CTexture()
{
	if (m_ppd3dTextures)
	{
		for (int i = 0; i < m_nTextures; i++) if (m_ppd3dTextures[i]) m_ppd3dTextures[i]->Release();
	}

	if (m_pRootArgumentInfos)
	{
		delete[] m_pRootArgumentInfos;
	}

	if (m_pd3dSamplerGpuDescriptorHandles) delete[] m_pd3dSamplerGpuDescriptorHandles;
}

void CTexture::SetRootArgument(int nIndex, UINT nRootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle)
{
	m_pRootArgumentInfos[nIndex].m_nRootParameterIndex = nRootParameterIndex;
	m_pRootArgumentInfos[nIndex].m_d3dSrvGpuDescriptorHandle = d3dSrvGpuDescriptorHandle;
}

void CTexture::SetSampler(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSamplerGpuDescriptorHandle)
{
	m_pd3dSamplerGpuDescriptorHandles[nIndex] = d3dSamplerGpuDescriptorHandle;
}

void CTexture::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	if (m_nTextureType == RESOURCE_TEXTURE2D_ARRAY)
	{
		pd3dCommandList->SetGraphicsRootDescriptorTable(m_pRootArgumentInfos[0].m_nRootParameterIndex, m_pRootArgumentInfos[0].m_d3dSrvGpuDescriptorHandle);
	}
	else
	{
		for (int i = 0; i < m_nTextures; i++)
		{
			pd3dCommandList->SetGraphicsRootDescriptorTable(m_pRootArgumentInfos[i].m_nRootParameterIndex, m_pRootArgumentInfos[i].m_d3dSrvGpuDescriptorHandle);
		}
	}
}

void CTexture::UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, int nIndex)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(m_pRootArgumentInfos[nIndex].m_nRootParameterIndex, m_pRootArgumentInfos[nIndex].m_d3dSrvGpuDescriptorHandle);
}

void CTexture::ReleaseUploadBuffers()
{
	if (m_ppd3dTextureUploadBuffers)
	{
		for (int i = 0; i < m_nTextures; i++) if (m_ppd3dTextureUploadBuffers[i]) m_ppd3dTextureUploadBuffers[i]->Release();
		delete[] m_ppd3dTextureUploadBuffers;
		m_ppd3dTextureUploadBuffers = NULL;
	}
}

void CTexture::ReleaseShaderVariables()
{
}

void CTexture::LoadTextureFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, wchar_t *pszFileName, UINT nIndex)
{
	m_ppd3dTextures[nIndex] = ::CreateTextureResourceFromFile(pd3dDevice, pd3dCommandList, pszFileName, &m_ppd3dTextureUploadBuffers[nIndex], D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CMaterial::CMaterial()
{
}

CMaterial::~CMaterial()
{
	if (m_pTexture) m_pTexture->Release();
	if (m_pShader) m_pShader->Release();
}

void CMaterial::SetTexture(CTexture *pTexture)
{
	if (m_pTexture) m_pTexture->Release();
	m_pTexture = pTexture;
	if (m_pTexture) m_pTexture->AddRef();
}

void CMaterial::SetShader(CShader *pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}

void CMaterial::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	if (m_pTexture) m_pTexture->UpdateShaderVariables(pd3dCommandList);
}

void CMaterial::ReleaseShaderVariables()
{
	if (m_pShader) m_pShader->ReleaseShaderVariables();
	if (m_pTexture) m_pTexture->ReleaseShaderVariables();
}

void CMaterial::ReleaseUploadBuffers()
{
	if (m_pTexture) m_pTexture->ReleaseUploadBuffers();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CGameObject::CGameObject(int nMeshes)
{
	m_xmf4x4ToParentTransform = Matrix4x4::Identity();
	m_xmf4x4World = Matrix4x4::Identity();

	m_nMeshes = nMeshes;
	m_ppMeshes = NULL;
	if (m_nMeshes > 0)
	{
		m_ppMeshes = new CMesh*[m_nMeshes];
		for (int i = 0; i < m_nMeshes; i++)	m_ppMeshes[i] = NULL;
	}
}

CGameObject::~CGameObject()
{
	ReleaseShaderVariables();

	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Release();
			m_ppMeshes[i] = NULL;
		}
		delete[] m_ppMeshes;
	}
	if (m_pMaterial) m_pMaterial->Release();

	if (m_pSibling) delete m_pSibling;
	if (m_pChild) delete m_pChild;
}

void CGameObject::ResizeMeshes(int nMeshes)
{
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Release();
			m_ppMeshes[i] = NULL;
		}
		delete[] m_ppMeshes;
		m_ppMeshes = NULL;
	}

	m_nMeshes = nMeshes;
	m_ppMeshes = NULL;
	if (m_nMeshes > 0)
	{
		m_ppMeshes = new CMesh*[m_nMeshes];
		for (int i = 0; i < m_nMeshes; i++)	m_ppMeshes[i] = NULL;
	}
}

void CGameObject::SetChild(CGameObject *pChild)
{
	if (m_pChild)
	{
		if (pChild) pChild->m_pSibling = m_pChild->m_pSibling;
		m_pChild->m_pSibling = pChild;
	}
	else
	{
		m_pChild = pChild;
	}
	if (pChild) pChild->m_pParent = this;
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

CMesh* CGameObject::GetMesh(int nIndex)
{
	if (!m_ppMeshes) return NULL;
	return m_ppMeshes[nIndex];
}

void CGameObject::SetShader(CShader *pShader)
{
	if (!m_pMaterial)
	{
		CMaterial *pMaterial = new CMaterial();
		SetMaterial(pMaterial);
	}
	if (m_pMaterial) m_pMaterial->SetShader(pShader);
}

void CGameObject::SetMaterial(CMaterial *pMaterial)
{
	if (m_pMaterial) m_pMaterial->Release();
	m_pMaterial = pMaterial;
	if (m_pMaterial) m_pMaterial->AddRef();
}

ID3D12Resource *CGameObject::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObject = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObject->Map(0, NULL, (void **)&m_pcbMappedGameObject);

	return(m_pd3dcbGameObject);
}

void CGameObject::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObject)
	{
		m_pd3dcbGameObject->Unmap(0, NULL);
		m_pd3dcbGameObject->Release();
	}
	if (m_pMaterial) m_pMaterial->ReleaseShaderVariables();
}

void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	XMStoreFloat4x4(&m_pcbMappedGameObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
	if (m_pMaterial) m_pcbMappedGameObject->m_nMaterial = m_pMaterial->m_nReflection;
}

void CGameObject::Animate(float fTimeElapsed)
{
	if (m_pSibling) m_pSibling->Animate(fTimeElapsed);
	if (m_pChild) m_pChild->Animate(fTimeElapsed);
}

void CGameObject::SetRootParameter(ID3D12GraphicsCommandList *pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(ROOT_PARAMETER_OBJECT, m_d3dCbvGPUDescriptorHandle);
}

void CGameObject::OnPrepareRender()
{
}

void CGameObject::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, UINT nInstances)
{
	if (!m_bActive) return;

	OnPrepareRender();

	if (m_pMaterial)
	{
		if (m_pMaterial->m_pShader)
		{
			m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
			m_pMaterial->m_pShader->UpdateShaderVariables(pd3dCommandList);

			UpdateShaderVariables(pd3dCommandList);
		}
		if (m_pMaterial->m_pTexture)
		{
			m_pMaterial->m_pTexture->UpdateShaderVariables(pd3dCommandList);
		}
	}

	if (m_ppMeshes && (m_nMeshes > 0))
	{
		SetRootParameter(pd3dCommandList);

		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList, nInstances);
		}
	}

	if (m_pSibling) m_pSibling->Render(pd3dCommandList, pCamera);
	if (m_pChild) m_pChild->Render(pd3dCommandList, pCamera);
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

	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();

	if (m_pSibling) m_pSibling->ReleaseUploadBuffers();
	if (m_pChild) m_pChild->ReleaseUploadBuffers();
}

void CGameObject::UpdateTransform(XMFLOAT4X4 *pxmf4x4Parent)
{
	m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4ToParentTransform, *pxmf4x4Parent) : m_xmf4x4ToParentTransform;

	if (m_pSibling) m_pSibling->UpdateTransform(pxmf4x4Parent);
	if (m_pChild) m_pChild->UpdateTransform(&m_xmf4x4World);
}

void CGameObject::LookAtNextPos()
{
	XMFLOAT3 curPos(m_xmf4x4ToParentTransform._41, m_xmf4x4ToParentTransform._42, m_xmf4x4ToParentTransform._43);
	XMFLOAT3 nextPos = g_wayPoint[m_nNextPos];
	XMFLOAT3 newLook = Vector3::Subtract(nextPos, curPos, true);
	XMFLOAT3 newRight = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), newLook);
	XMFLOAT3 NEWuP = Vector3::CrossProduct(newLook, newRight);

	m_xmf4x4ToParentTransform._11 = newRight.x; m_xmf4x4ToParentTransform._12 = newRight.y; m_xmf4x4ToParentTransform._13 = newRight.z;
	m_xmf4x4ToParentTransform._31 = newLook.x; m_xmf4x4ToParentTransform._32 = newLook.y; m_xmf4x4ToParentTransform._33 = newLook.z;
}

void CGameObject::SetNextPos(int n)
{
	if (n == -1)
		m_nNextPos++;
	else
		m_nNextPos = n; 
}

void CGameObject::SetWorldTransform(XMFLOAT4X4 xmf4x4World)
{
	m_xmf4x4World._11 = xmf4x4World._11;		m_xmf4x4World._12 = xmf4x4World._12;
	m_xmf4x4World._13 = xmf4x4World._13;		m_xmf4x4World._14 = xmf4x4World._14;
	m_xmf4x4World._21 = xmf4x4World._21;		m_xmf4x4World._22 = xmf4x4World._22;
	m_xmf4x4World._23 = xmf4x4World._23;		m_xmf4x4World._24 = xmf4x4World._24;
	m_xmf4x4World._31 = xmf4x4World._31;		m_xmf4x4World._32 = xmf4x4World._32;
	m_xmf4x4World._33 = xmf4x4World._33;		m_xmf4x4World._34 = xmf4x4World._34;
	m_xmf4x4World._41 = xmf4x4World._41;		m_xmf4x4World._42 = xmf4x4World._42;
	m_xmf4x4World._43 = xmf4x4World._43;		m_xmf4x4World._44 = xmf4x4World._44;
}

void CGameObject::SetToParentTransform(XMFLOAT4X4 xmf4x4Parent)
{
	m_xmf4x4ToParentTransform._11 = xmf4x4Parent._11;		m_xmf4x4ToParentTransform._12 = xmf4x4Parent._12;
	m_xmf4x4ToParentTransform._13 = xmf4x4Parent._13;		m_xmf4x4ToParentTransform._14 = xmf4x4Parent._14;
	m_xmf4x4ToParentTransform._21 = xmf4x4Parent._21;		m_xmf4x4ToParentTransform._22 = xmf4x4Parent._22;
	m_xmf4x4ToParentTransform._23 = xmf4x4Parent._23;		m_xmf4x4ToParentTransform._24 = xmf4x4Parent._24;
	m_xmf4x4ToParentTransform._31 = xmf4x4Parent._31;		m_xmf4x4ToParentTransform._32 = xmf4x4Parent._32;
	m_xmf4x4ToParentTransform._33 = xmf4x4Parent._33;		m_xmf4x4ToParentTransform._34 = xmf4x4Parent._34;
	m_xmf4x4ToParentTransform._41 = xmf4x4Parent._41;		m_xmf4x4ToParentTransform._42 = xmf4x4Parent._42;
	m_xmf4x4ToParentTransform._43 = xmf4x4Parent._43;		m_xmf4x4ToParentTransform._44 = xmf4x4Parent._44;
}

CGameObject *CGameObject::FindFrame(_TCHAR *pstrFrameName)
{
	CGameObject *pFrameObject = NULL;
	if (!_tcsncmp(m_pstrFrameName, pstrFrameName, _tcslen(pstrFrameName))) return(this);

	if (m_pSibling) if (pFrameObject = m_pSibling->FindFrame(pstrFrameName)) return(pFrameObject);
	if (m_pChild) if (pFrameObject = m_pChild->FindFrame(pstrFrameName)) return(pFrameObject);

	return(NULL);
}

void CGameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4ToParentTransform._41 = x;
	m_xmf4x4ToParentTransform._42 = y;
	m_xmf4x4ToParentTransform._43 = z;
}

void CGameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

void CGameObject::SetLocalPosition(XMFLOAT3 xmf3Position)
{
	XMMATRIX mtxTranslation = XMMatrixTranslation(xmf3Position.x, xmf3Position.y, xmf3Position.z);
	m_xmf4x4ToParentTransform = Matrix4x4::Multiply(m_xmf4x4ToParentTransform, mtxTranslation);
}

void CGameObject::SetScale(float x, float y, float z)
{
	XMMATRIX mtxScale = XMMatrixScaling(x, y, z);
	m_xmf4x4ToParentTransform = Matrix4x4::Multiply(mtxScale, m_xmf4x4ToParentTransform);
}

void CGameObject::SetLocalScale(float x, float y, float z)
{
}

XMFLOAT3 CGameObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}

XMFLOAT3 CGameObject::GetLook()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33)));
}

XMFLOAT3 CGameObject::GetUp()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23)));
}

XMFLOAT3 CGameObject::GetRight()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13)));
}

void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4ToParentTransform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParentTransform);
}

void CGameObject::Rotate(XMFLOAT3 *pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4ToParentTransform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParentTransform);
}

void CGameObject::Rotate(XMFLOAT4 *pxmf4Quaternion)
{
	XMMATRIX mtxRotate = XMMatrixRotationQuaternion(XMLoadFloat4(pxmf4Quaternion));
	m_xmf4x4ToParentTransform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParentTransform);
}

void CGameObject::LoadFrameHierarchyFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, 
	ID3D12RootSignature *pd3dGraphicsRootSignature, wifstream& InFile, UINT nFrame)
{
	XMFLOAT3 *pxmf3Positions = NULL, *pxmf3Normals = NULL;
	XMFLOAT2 *pxmf3TextureCoords0 = NULL, *pxmf3TextureCoords1 = NULL;
	UINT *pnIndices = NULL;

	TCHAR pstrMeshName[64] = { '\0' };
	TCHAR pstrAlbedoTextureName[64] = { '\0' };
	TCHAR pstrToken[64] = { '\0' };
	TCHAR pstrDebug[128] = { '\0' };

	XMFLOAT3 xmf3FrameLocalPosition, xmf3FrameLocalRotation, xmf3FrameLocalScale, xmf3FrameScale;
	XMFLOAT4 xmf4FrameLocalQuaternion, xmf4MaterialAlbedo;
	int nVertices = 0, nNormals = 0, nTextureCoords = 0, nIndices = 0;

	for ( ; ; )
	{
		InFile >> pstrToken;
		if (!InFile) break;

		if (!_tcscmp(pstrToken, _T("FrameName:")))
		{
			InFile >> m_pstrFrameName;

			nVertices = nNormals = nTextureCoords = nIndices = 0;
			xmf4MaterialAlbedo = XMFLOAT4(-1.0f, -1.0f, -1.0f, -1.0f);
			pxmf3Positions = pxmf3Normals = NULL;
			pxmf3TextureCoords0 = pxmf3TextureCoords1 = NULL;
			pstrAlbedoTextureName[0] = '\0';
			pnIndices = NULL;
		}
		else if (!_tcscmp(pstrToken, _T("Transform:")))
		{
			InFile >> xmf3FrameLocalPosition.x >> xmf3FrameLocalPosition.y >> xmf3FrameLocalPosition.z;
			InFile >> xmf3FrameLocalRotation.x >> xmf3FrameLocalRotation.y >> xmf3FrameLocalRotation.z;
			InFile >> xmf4FrameLocalQuaternion.x >> xmf4FrameLocalQuaternion.y >> xmf4FrameLocalQuaternion.z >> xmf4FrameLocalQuaternion.w;
			InFile >> xmf3FrameLocalScale.x >> xmf3FrameLocalScale.y >> xmf3FrameLocalScale.z;
			InFile >> xmf3FrameScale.x >> xmf3FrameScale.y >> xmf3FrameScale.z;
		}
		else if (!_tcscmp(pstrToken, _T("TransformMatrix:")))
		{
			InFile >> m_xmf4x4ToRootTransform._11 >> m_xmf4x4ToRootTransform._12 >> m_xmf4x4ToRootTransform._13 >> m_xmf4x4ToRootTransform._14;
			InFile >> m_xmf4x4ToRootTransform._21 >> m_xmf4x4ToRootTransform._22 >> m_xmf4x4ToRootTransform._23 >> m_xmf4x4ToRootTransform._24;
			InFile >> m_xmf4x4ToRootTransform._31 >> m_xmf4x4ToRootTransform._32 >> m_xmf4x4ToRootTransform._33 >> m_xmf4x4ToRootTransform._34;
			InFile >> m_xmf4x4ToRootTransform._41 >> m_xmf4x4ToRootTransform._42 >> m_xmf4x4ToRootTransform._43 >> m_xmf4x4ToRootTransform._44;

			InFile >> m_xmf4x4ToParentTransform._11 >> m_xmf4x4ToParentTransform._12 >> m_xmf4x4ToParentTransform._13 >> m_xmf4x4ToParentTransform._14;
			InFile >> m_xmf4x4ToParentTransform._21 >> m_xmf4x4ToParentTransform._22 >> m_xmf4x4ToParentTransform._23 >> m_xmf4x4ToParentTransform._24;
			InFile >> m_xmf4x4ToParentTransform._31 >> m_xmf4x4ToParentTransform._32 >> m_xmf4x4ToParentTransform._33 >> m_xmf4x4ToParentTransform._34;
			InFile >> m_xmf4x4ToParentTransform._41 >> m_xmf4x4ToParentTransform._42 >> m_xmf4x4ToParentTransform._43 >> m_xmf4x4ToParentTransform._44;
		}
		else if (!_tcscmp(pstrToken, _T("MeshName:")))
		{
			InFile >> pstrMeshName;
		}
		else if (!_tcscmp(pstrToken, _T("Vertices:")))
		{
			InFile >> nVertices;
			pxmf3Positions = new XMFLOAT3[nVertices];
			for (int i = 0; i < nVertices; i++)
			{
				InFile >> pxmf3Positions[i].x >> pxmf3Positions[i].y >> pxmf3Positions[i].z;
			}
		}
		else if (!_tcscmp(pstrToken, _T("Normals:")))
		{
			InFile >> nNormals;
			pxmf3Normals = new XMFLOAT3[nNormals];
			for (int i = 0; i < nNormals; i++)
			{
				InFile >> pxmf3Normals[i].x >> pxmf3Normals[i].y >> pxmf3Normals[i].z;
			}
		}
		else if (!_tcscmp(pstrToken, _T("TextureCoordinates0:")))
		{
			InFile >> nTextureCoords;
			pxmf3TextureCoords0 = new XMFLOAT2[nTextureCoords];
			for (int i = 0; i < nTextureCoords; i++)
			{
				InFile >> pxmf3TextureCoords0[i].x >> pxmf3TextureCoords0[i].y;
			}
		}
		else if (!_tcscmp(pstrToken, _T("TextureCoordinates1:")))
		{
			InFile >> nTextureCoords;
			pxmf3TextureCoords1 = new XMFLOAT2[nTextureCoords];
			for (int i = 0; i < nTextureCoords; i++)
			{
				InFile >> pxmf3TextureCoords1[i].x >> pxmf3TextureCoords1[i].y;
			}
		}
		else if (!_tcscmp(pstrToken, _T("Indices:")))
		{
			InFile >> nIndices;
			pnIndices = new UINT[nIndices];
			for (int i = 0; i < nIndices; i++)
			{
				InFile >> pnIndices[i];
			}
		}
		else if (!_tcscmp(pstrToken, _T("AlbedoColor:")))
		{
			InFile >> xmf4MaterialAlbedo.x >> xmf4MaterialAlbedo.y >> xmf4MaterialAlbedo.z >> xmf4MaterialAlbedo.w;
		}
		else if (!_tcscmp(pstrToken, _T("AlbedoTextureName:")))
		{
			InFile >> pstrAlbedoTextureName;
		}
		else if (!_tcscmp(pstrToken, _T("Children:")))
		{
			int nChilds = 0;
			InFile >> nChilds;
			if (nChilds > 0)
			{
				for (int i = 0; i < nChilds; i++)
				{
					CGameObject *pChild = new CGameObject(1);
					pChild->LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, InFile, nFrame+1);
					SetChild(pChild);
#ifdef _WITH_DEBUG_FRAME_HIERARCHY
					_stprintf_s(pstrDebug, 128, _T("(Frame: %p) (Parent: %p)\n"), pChild, this);
					OutputDebugString(pstrDebug);
#endif
				}
			}
		}
		else if (!_tcscmp(pstrToken, _T("EndOfFrame")))
		{
			CMesh *pMesh = NULL;
			CMaterial *pMaterial = NULL;
			if ((nNormals > 0) && (nTextureCoords > 0) && (pstrAlbedoTextureName[0] != '\0'))
			{
				if (nVertices > 0) pMesh = new CMeshIlluminatedTextured(pd3dDevice, pd3dCommandList, nVertices, pxmf3Positions, pxmf3Normals, pxmf3TextureCoords0, nIndices, pnIndices);

				TCHAR pstrPathName[128] = { '\0' };
				_tcscpy_s(pstrPathName, 128, _T("../Assets/Model/"));
				_tcscat_s(pstrPathName, 128, pstrAlbedoTextureName);
				_tcscat_s(pstrPathName, 128, _T(".dds"));

				CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
				pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pstrPathName, 0);

				pMaterial = new CMaterial();
				pMaterial->m_xmf4Albedo = xmf4MaterialAlbedo;

				pMaterial->SetTexture(pTexture);

				UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

				ID3D12Resource *pd3dcbResource = CreateShaderVariables(pd3dDevice, pd3dCommandList);

				CIlluminatedTexturedShader *pShader = new CIlluminatedTexturedShader();
				pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
				pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
				pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 1);
				pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, pd3dcbResource, ncbElementBytes);
				pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, true);

				SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());

				pMaterial->SetShader(pShader);
			}
			else if (nNormals > 0)
			{
				if (nVertices > 0) pMesh = new CMeshIlluminated(pd3dDevice, pd3dCommandList, nVertices, pxmf3Positions, pxmf3Normals, nIndices, pnIndices);

				pMaterial = new CMaterial();
				pMaterial->m_xmf4Albedo = xmf4MaterialAlbedo;

				UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

				ID3D12Resource *pd3dcbResource = CreateShaderVariables(pd3dDevice, pd3dCommandList);

				CIlluminatedShader *pShader = new CIlluminatedShader();
				pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
				pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
				pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 0);
				pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, pd3dcbResource, ncbElementBytes);

				SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());

				pMaterial->SetShader(pShader);
			}
			else if (nTextureCoords > 0)
			{
				if (nVertices > 0) pMesh = new CMeshTextured(pd3dDevice, pd3dCommandList, nVertices, pxmf3Positions, pxmf3TextureCoords0, nIndices, pnIndices);
			}
			else 
			{
				if (nVertices > 0) pMesh = new CMesh(pd3dDevice, pd3dCommandList, nVertices, pxmf3Positions, nIndices, pnIndices);
			}
					
			if (pMesh) 
				SetMesh(0, pMesh);
			else 
				ResizeMeshes(0);

			if (pMaterial) SetMaterial(pMaterial);

			if (pxmf3Positions) delete[] pxmf3Positions;
			if (pxmf3Normals) delete[] pxmf3Normals;
			if (pxmf3TextureCoords0) delete[] pxmf3TextureCoords0;
			if (pxmf3TextureCoords1) delete[] pxmf3TextureCoords1;
			if (pnIndices) delete[] pnIndices;

			break;
		}
	}
}


void CGameObject::CopyFrameHierarchyFromObj(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList,
	ID3D12RootSignature *pd3dGraphicsRootSignature, CGameObject *pObject)
{
	CMesh *pMesh = NULL;
	CMaterial *pMaterial = new CMaterial();

	memcpy(m_pstrFrameName, pObject->m_pstrFrameName, 256);

	m_xmf4x4ToRootTransform._11 = pObject->m_xmf4x4ToRootTransform._11;		m_xmf4x4ToRootTransform._12 = pObject->m_xmf4x4ToRootTransform._12;
	m_xmf4x4ToRootTransform._13 = pObject->m_xmf4x4ToRootTransform._13;		m_xmf4x4ToRootTransform._14 = pObject->m_xmf4x4ToRootTransform._14;
	m_xmf4x4ToRootTransform._21 = pObject->m_xmf4x4ToRootTransform._21;		m_xmf4x4ToRootTransform._22 = pObject->m_xmf4x4ToRootTransform._22;
	m_xmf4x4ToRootTransform._23 = pObject->m_xmf4x4ToRootTransform._23;		m_xmf4x4ToRootTransform._24 = pObject->m_xmf4x4ToRootTransform._24;
	m_xmf4x4ToRootTransform._31 = pObject->m_xmf4x4ToRootTransform._31;		m_xmf4x4ToRootTransform._32 = pObject->m_xmf4x4ToRootTransform._32;
	m_xmf4x4ToRootTransform._33 = pObject->m_xmf4x4ToRootTransform._33;		m_xmf4x4ToRootTransform._34 = pObject->m_xmf4x4ToRootTransform._34;
	m_xmf4x4ToRootTransform._41 = pObject->m_xmf4x4ToRootTransform._41;		m_xmf4x4ToRootTransform._42 = pObject->m_xmf4x4ToRootTransform._42;
	m_xmf4x4ToRootTransform._43 = pObject->m_xmf4x4ToRootTransform._43;		m_xmf4x4ToRootTransform._44 = pObject->m_xmf4x4ToRootTransform._44;

	m_xmf4x4ToParentTransform._11 = pObject->m_xmf4x4ToParentTransform._11;		m_xmf4x4ToParentTransform._12 = pObject->m_xmf4x4ToParentTransform._12;
	m_xmf4x4ToParentTransform._13 = pObject->m_xmf4x4ToParentTransform._13;		m_xmf4x4ToParentTransform._14 = pObject->m_xmf4x4ToParentTransform._14;
	m_xmf4x4ToParentTransform._21 = pObject->m_xmf4x4ToParentTransform._21;		m_xmf4x4ToParentTransform._22 = pObject->m_xmf4x4ToParentTransform._22;
	m_xmf4x4ToParentTransform._23 = pObject->m_xmf4x4ToParentTransform._23;		m_xmf4x4ToParentTransform._24 = pObject->m_xmf4x4ToParentTransform._24;
	m_xmf4x4ToParentTransform._31 = pObject->m_xmf4x4ToParentTransform._31;		m_xmf4x4ToParentTransform._32 = pObject->m_xmf4x4ToParentTransform._32;
	m_xmf4x4ToParentTransform._33 = pObject->m_xmf4x4ToParentTransform._33;		m_xmf4x4ToParentTransform._34 = pObject->m_xmf4x4ToParentTransform._34;
	m_xmf4x4ToParentTransform._41 = pObject->m_xmf4x4ToParentTransform._41;		m_xmf4x4ToParentTransform._42 = pObject->m_xmf4x4ToParentTransform._42;
	m_xmf4x4ToParentTransform._43 = pObject->m_xmf4x4ToParentTransform._43;		m_xmf4x4ToParentTransform._44 = pObject->m_xmf4x4ToParentTransform._44;
	
	if (pObject->m_pChild)
	{
		CGameObject *curChild = pObject->m_pChild;
		do
		{
			if (memcmp(curChild->m_pstrFrameName, _T("Hellfire_Missile"), 256) != 0) {
				CGameObject *pChild = new CGameObject(1);
				pChild->CopyFrameHierarchyFromObj(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, curChild);
				SetChild(pChild);
			}
			curChild = curChild->m_pSibling;
		} while (curChild);
		
	}

	pMesh = pObject->GetMesh(0);
	if (pMesh)
		SetMesh(0, pMesh);
	else
		ResizeMeshes(0);

	if (pObject->m_pMaterial)
	{
		CTexture * pTexture = pObject->m_pMaterial->m_pTexture;
		UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
		ID3D12Resource *pd3dcbResource = CreateShaderVariables(pd3dDevice, pd3dCommandList);
		if (pTexture)
		{
			pMaterial->SetTexture(pTexture);

			CIlluminatedTexturedShader *pShader = new CIlluminatedTexturedShader();
			pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
			pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
			pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 1);
			pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, pd3dcbResource, ncbElementBytes);
			pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, true);

			SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());

			pMaterial->SetShader(pShader);
		}
		else
		{
			CIlluminatedShader *pShader = new CIlluminatedShader();
			pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
			pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
			pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 0);
			pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, pd3dcbResource, ncbElementBytes);

			SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());

			pMaterial->SetShader(pShader);
		}
		pMaterial->SetAlbedo(pObject->m_pMaterial->m_xmf4Albedo);
		pMaterial->SetReflection(pObject->m_pMaterial->m_nReflection);

		SetMaterial(pMaterial);
	}
}

void CGameObject::CopyObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList,
	ID3D12RootSignature *pd3dGraphicsRootSignature, CGameObject *pObject)
{
	CMesh *pMesh = NULL;
	CMaterial *pMaterial = new CMaterial();

	memcpy(m_pstrFrameName, pObject->m_pstrFrameName, 256);

	m_xmf4x4ToRootTransform._11 = pObject->m_xmf4x4ToRootTransform._11;		m_xmf4x4ToRootTransform._12 = pObject->m_xmf4x4ToRootTransform._12;
	m_xmf4x4ToRootTransform._13 = pObject->m_xmf4x4ToRootTransform._13;		m_xmf4x4ToRootTransform._14 = pObject->m_xmf4x4ToRootTransform._14;
	m_xmf4x4ToRootTransform._21 = pObject->m_xmf4x4ToRootTransform._21;		m_xmf4x4ToRootTransform._22 = pObject->m_xmf4x4ToRootTransform._22;
	m_xmf4x4ToRootTransform._23 = pObject->m_xmf4x4ToRootTransform._23;		m_xmf4x4ToRootTransform._24 = pObject->m_xmf4x4ToRootTransform._24;
	m_xmf4x4ToRootTransform._31 = pObject->m_xmf4x4ToRootTransform._31;		m_xmf4x4ToRootTransform._32 = pObject->m_xmf4x4ToRootTransform._32;
	m_xmf4x4ToRootTransform._33 = pObject->m_xmf4x4ToRootTransform._33;		m_xmf4x4ToRootTransform._34 = pObject->m_xmf4x4ToRootTransform._34;
	m_xmf4x4ToRootTransform._41 = pObject->m_xmf4x4ToRootTransform._41;		m_xmf4x4ToRootTransform._42 = pObject->m_xmf4x4ToRootTransform._42;
	m_xmf4x4ToRootTransform._43 = pObject->m_xmf4x4ToRootTransform._43;		m_xmf4x4ToRootTransform._44 = pObject->m_xmf4x4ToRootTransform._44;

	m_xmf4x4ToParentTransform._11 = pObject->m_xmf4x4ToParentTransform._11;		m_xmf4x4ToParentTransform._12 = pObject->m_xmf4x4ToParentTransform._12;
	m_xmf4x4ToParentTransform._13 = pObject->m_xmf4x4ToParentTransform._13;		m_xmf4x4ToParentTransform._14 = pObject->m_xmf4x4ToParentTransform._14;
	m_xmf4x4ToParentTransform._21 = pObject->m_xmf4x4ToParentTransform._21;		m_xmf4x4ToParentTransform._22 = pObject->m_xmf4x4ToParentTransform._22;
	m_xmf4x4ToParentTransform._23 = pObject->m_xmf4x4ToParentTransform._23;		m_xmf4x4ToParentTransform._24 = pObject->m_xmf4x4ToParentTransform._24;
	m_xmf4x4ToParentTransform._31 = pObject->m_xmf4x4ToParentTransform._31;		m_xmf4x4ToParentTransform._32 = pObject->m_xmf4x4ToParentTransform._32;
	m_xmf4x4ToParentTransform._33 = pObject->m_xmf4x4ToParentTransform._33;		m_xmf4x4ToParentTransform._34 = pObject->m_xmf4x4ToParentTransform._34;
	m_xmf4x4ToParentTransform._41 = pObject->m_xmf4x4ToParentTransform._41;		m_xmf4x4ToParentTransform._42 = pObject->m_xmf4x4ToParentTransform._42;
	m_xmf4x4ToParentTransform._43 = pObject->m_xmf4x4ToParentTransform._43;		m_xmf4x4ToParentTransform._44 = pObject->m_xmf4x4ToParentTransform._44;

	pMesh = pObject->GetMesh(0);
	if (pMesh)
		SetMesh(0, pMesh);
	else
		ResizeMeshes(0);

	if (pObject->m_pMaterial)
	{
		CTexture * pTexture = pObject->m_pMaterial->m_pTexture;
		UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
		ID3D12Resource *pd3dcbResource = CreateShaderVariables(pd3dDevice, pd3dCommandList);
		if (pTexture)
		{
			pMaterial->SetTexture(pTexture);

			CIlluminatedTexturedShader *pShader = new CIlluminatedTexturedShader();
			pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
			pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
			pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 1);
			pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, pd3dcbResource, ncbElementBytes);
			pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, true);

			SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());

			pMaterial->SetShader(pShader);
		}
		else
		{
			CIlluminatedShader *pShader = new CIlluminatedShader();
			pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
			pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
			pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 0);
			pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, pd3dcbResource, ncbElementBytes);

			SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());

			pMaterial->SetShader(pShader);
		}
		pMaterial->SetAlbedo(pObject->m_pMaterial->m_xmf4Albedo);
		pMaterial->SetReflection(pObject->m_pMaterial->m_nReflection);

		SetMaterial(pMaterial);
	}
}

void CGameObject::PrintFrameInfo(CGameObject *pGameObject, CGameObject *pParent)
{
	TCHAR pstrDebug[128] = { 0 };

	_stprintf_s(pstrDebug, 128, _T("(Frame: %p) (Parent: %p)\n"), pGameObject, pParent);
	OutputDebugString(pstrDebug);

	if (pGameObject->m_pSibling) PrintFrameInfo(pGameObject->m_pSibling, pParent);
	if (pGameObject->m_pChild) PrintFrameInfo(pGameObject->m_pChild, pGameObject);
}

void CGameObject::LoadGeometryFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, TCHAR *pstrFileName)
{
	wifstream InFile(pstrFileName);
	LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, InFile, 0);

#ifdef _WITH_DEBUG_FRAME_HIERARCHY
	TCHAR pstrDebug[128] ={ 0 };
	_stprintf_s(pstrDebug, 128, _T("Frame Hierarchy\n"));
	OutputDebugString(pstrDebug);

	PrintFrameInfo(this, NULL);
#endif
}

CGameObject* CGameObject::GetEffect()
{
	m_effect->SetPosition(GetPosition());
	m_effect->UpdateTransform(nullptr);

	return m_effect;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
CRotatingObject::CRotatingObject(int nMeshes)
{
	m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed = 15.0f;
}

CRotatingObject::~CRotatingObject()
{
}

void CRotatingObject::Animate(float fTimeElapsed)
{
	CGameObject::Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);
}

void CRotatingObject::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CGameObject::Render(pd3dCommandList, pCamera);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
CRevolvingObject::CRevolvingObject(int nMeshes)
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

////////////////////////////////////////////////////////////////////////////////////////////////////
//
CApacheHellicopter::CApacheHellicopter(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, CGameObject* pObject)
{
	CopyFrameHierarchyFromObj(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pObject);
}

CApacheHellicopter::CApacheHellicopter(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, L"../Assets/Model/Apache.txt");
}

CApacheHellicopter::~CApacheHellicopter()
{
}

void CApacheHellicopter::Animate(float fTimeElapsed)
{
	if (m_pRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationY(XMConvertToRadians(360.0f * 3.0f) * fTimeElapsed);
		m_pRotorFrame->m_xmf4x4ToParentTransform = Matrix4x4::Multiply(xmmtxRotate, m_pRotorFrame->m_xmf4x4ToParentTransform);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
CGunshipHellicopter::CGunshipHellicopter(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, CGameObject* pObject)
{
	CopyFrameHierarchyFromObj(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pObject);

	InitBaseSetting();
}

CGunshipHellicopter::CGunshipHellicopter(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, L"../Assets/Model/Gunship.txt");

	InitBaseSetting();
}

CGunshipHellicopter::~CGunshipHellicopter()
{
}

void CGunshipHellicopter::InitBaseSetting() {
	m_iNextMissile = 0;
	m_fSpeed = 15.0f;
	m_fReloadTime = COOLTIME_ENEMY_RELOAD;

	m_pRotorFrame = FindFrame(_T("Rotor"));
	m_pBackRotorFrame = FindFrame(_T("Back_Rotor"));

	m_pHellfileMissileFrame = FindFrame(_T("Hellfire_Missile"));
	if (m_pHellfileMissileFrame) m_pHellfileMissileFrame->m_bActive = false;
}

void CGunshipHellicopter::Animate(float fTimeElapsed)
{
	m_fReloadTime += fTimeElapsed;

	// 다음 웨이포인트 업데이트
	TargetUpdate();

	// 웨이 포인트 따라 이동
	MoveForward(m_fSpeed * fTimeElapsed);

	if (m_pRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationY(XMConvertToRadians(360.0f * 3.0f) * fTimeElapsed);
		m_pRotorFrame->m_xmf4x4ToParentTransform = Matrix4x4::Multiply(xmmtxRotate, m_pRotorFrame->m_xmf4x4ToParentTransform);
	}

	if (m_pBackRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationX(XMConvertToRadians(360.0f * 3.0f) * fTimeElapsed);
		m_pBackRotorFrame->m_xmf4x4ToParentTransform = Matrix4x4::Multiply(xmmtxRotate, m_pBackRotorFrame->m_xmf4x4ToParentTransform);
	}
}

bool CGunshipHellicopter::CollisionTest(CGameObject* other)
{
	if (!m_bActive) return false;

	float fDistance = Vector3::Length(Vector3::Subtract(GetPosition(), other->GetPosition()));
	if (other->m_fSize + m_fSize > fDistance)
	{
		m_bActive = false;
		other->m_bActive = false;
		return true;
	}
	return false;
}

bool CGunshipHellicopter::LockOn(CGameObject* target)
{
	if (!m_bActive) return false;
	if (!target->m_bActive) return false;

	float fDistance = Vector3::Length(Vector3::Subtract(GetPosition(), target->GetPosition()));
	if (fDistance > ENEMY_ATTACK_RADIUS) return false;

	m_xmf3Target = target->GetPosition();

	return true;
}

bool CGunshipHellicopter::ReadyToFire(CGameObject* target)
{
	if (!LockOn(target)) return false;
	if (m_fReloadTime < COOLTIME_ENEMY_RELOAD) return false;
	return true;
}

void CGunshipHellicopter::SetShaderEffect(CShader* shader)
{
	m_effect = shader->GetEffect();
	for (int i = 0; i < MAX_ENEMY_MISSILE; ++i)
	{
		m_missiles[i].m_effect = shader->GetEffect();
	}
}

void CGunshipHellicopter::TargetUpdate()
{
	if (Vector3::Length(Vector3::Subtract(GetPosition(), g_wayPoint[m_nNextPos])) > WayPointRadius) return;
	if (m_nNextPos < 9) 
	{ 
		m_nNextPos++;
		LookAtNextPos();
	}
	// 마지막 지점에 도달한 경우 처음 위치로 돌리고 1번 위치를 바라보도록 목표를 업데이트 한다.
	else
	{
		m_bActive = true;
		m_nNextPos = 1;
		SetPosition(g_wayPoint[0]);
		LookAtNextPos();
		UpdateTransform(NULL);
	}
}

CGameObject* CGunshipHellicopter::GetMissile()
{
	int nextMissile = m_iNextMissile++;

	XMFLOAT3 xmf3Up(0.0f, 1.0f, 0.0f);
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = Vector3::Subtract(m_xmf3Target, xmf3Position, true);
	XMFLOAT3 xmf3Right = Vector3::CrossProduct(xmf3Up, xmf3Look);

	if (m_iNextMissile >= MAX_ENEMY_MISSILE) m_iNextMissile = 0;

	m_missiles[nextMissile].SetToParentTransform(
		XMFLOAT4X4(
			xmf3Right.x, xmf3Right.y, xmf3Right.z, 0.0f,
			xmf3Up.x, xmf3Up.y, xmf3Up.z, 0.0f,
			xmf3Look.x, xmf3Look.y, xmf3Look.z, 0.0f,
			xmf3Position.x, xmf3Position.y, xmf3Position.z, 1.0f
		)
	);
	m_missiles[nextMissile].UpdateTransform(nullptr);
	m_fReloadTime = 0;

	return &m_missiles[nextMissile];
}

void CGunshipHellicopter::SetMissile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList,
	ID3D12RootSignature *pd3dGraphicsRootSignature, CGameObject* pHellfileMissileFrame)
{
	for (int i = 0; i < MAX_ENEMY_MISSILE; ++i)
	{
		m_missiles[i].CopyObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pHellfileMissileFrame);
		m_missiles[i].m_bActive = false;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
CFlyerShip::CFlyerShip(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, L"../Assets/Model/Flyer.txt");
}

CFlyerShip::~CFlyerShip()
{
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CHeightMapTerrain::CHeightMapTerrain(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color) : CGameObject(0)
{
	m_nWidth = nWidth;
	m_nLength = nLength;

	int cxQuadsPerBlock = nBlockWidth - 1;
	int czQuadsPerBlock = nBlockLength - 1;

	m_xmf3Scale = xmf3Scale;

	m_pHeightMapImage = new CHeightMapImage(pFileName, nWidth, nLength, xmf3Scale);

	long cxBlocks = (m_nWidth - 1) / cxQuadsPerBlock;
	long czBlocks = (m_nLength - 1) / czQuadsPerBlock;

	m_nMeshes = cxBlocks * czBlocks;
	m_ppMeshes = new CMesh*[m_nMeshes];
	for (int i = 0; i < m_nMeshes; i++)	m_ppMeshes[i] = NULL;

	CHeightMapGridMesh *pHeightMapGridMesh = NULL;
	for (int z = 0, zStart = 0; z < czBlocks; z++)
	{
		for (int x = 0, xStart = 0; x < cxBlocks; x++)
		{
			xStart = x * (nBlockWidth - 1);
			zStart = z * (nBlockLength - 1);
			pHeightMapGridMesh = new CHeightMapGridMesh(pd3dDevice, pd3dCommandList, xStart, zStart, nBlockWidth, nBlockLength, xmf3Scale, xmf4Color, m_pHeightMapImage);
			SetMesh(x + (z*cxBlocks), pHeightMapGridMesh);
		}
	}

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CTexture *pTerrainTexture = new CTexture(3, RESOURCE_TEXTURE2D, 0);

	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Terrain/Base_Texture.dds", 0);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Terrain/Detail_Texture_7.dds", 1);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Terrain/Normal_Texture_7.dds", 2);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수

	CTerrainShader *pTerrainShader = new CTerrainShader();
	pTerrainShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pTerrainShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pTerrainShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 3);
	pTerrainShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, m_pd3dcbGameObject, ncbElementBytes);
	pTerrainShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTerrainTexture, 6, true);

	CMaterial *pTerrainMaterial = new CMaterial();
	pTerrainMaterial->SetTexture(pTerrainTexture);
	pTerrainMaterial->SetReflection(8);

	SetMaterial(pTerrainMaterial);

	SetCbvGPUDescriptorHandle(pTerrainShader->GetGPUCbvDescriptorStartHandle());

	SetShader(pTerrainShader);
}

CHeightMapTerrain::~CHeightMapTerrain(void)
{
	if (m_pHeightMapImage) delete m_pHeightMapImage;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
CSkyBox::CSkyBox(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature) : CGameObject(6)
{
	CTexturedRectMesh *pSkyBoxMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f, +10.0f);
	SetMesh(0, pSkyBoxMesh);
	pSkyBoxMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f, -10.0f);
	SetMesh(1, pSkyBoxMesh);
	pSkyBoxMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 0.0f, 20.0f, 20.0f, -10.0f, 0.0f, 0.0f);
	SetMesh(2, pSkyBoxMesh);
	pSkyBoxMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 0.0f, 20.0f, 20.0f, +10.0f, 0.0f, 0.0f);
	SetMesh(3, pSkyBoxMesh);
	pSkyBoxMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 20.0f, 0.0f, 20.0f, 0.0f, +10.0f, 0.0f);
	SetMesh(4, pSkyBoxMesh);
	pSkyBoxMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 20.0f, 0.0f, 20.0f, 0.0f, -10.0f, 0.0f);
	SetMesh(5, pSkyBoxMesh);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CTexture *pSkyBoxTexture = new CTexture(6, RESOURCE_TEXTURE2D, 0);

	pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/SkyBox/SkyBox_Front_0.dds", 0);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/SkyBox/SkyBox_Back_0.dds", 1);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/SkyBox/SkyBox_Left_0.dds", 2);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/SkyBox/SkyBox_Right_0.dds", 3);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/SkyBox/SkyBox_Top_0.dds", 4);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/SkyBox/SkyBox_Bottom_0.dds", 5);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수

	CSkyBoxShader *pSkyBoxShader = new CSkyBoxShader();
	pSkyBoxShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pSkyBoxShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pSkyBoxShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 6);
	pSkyBoxShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, m_pd3dcbGameObject, ncbElementBytes);
	pSkyBoxShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pSkyBoxTexture, 9, false);

	CMaterial *pSkyBoxMaterial = new CMaterial();
	pSkyBoxMaterial->SetTexture(pSkyBoxTexture);

	SetMaterial(pSkyBoxMaterial);

	SetCbvGPUDescriptorHandle(pSkyBoxShader->GetGPUCbvDescriptorStartHandle());

	SetShader(pSkyBoxShader);
}

CSkyBox::~CSkyBox()
{
}

void CSkyBox::SetPosition(float x, float y, float z)
{
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;
}

void CSkyBox::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	XMFLOAT3 xmf3CameraPos = pCamera->GetPosition();
	SetPosition(xmf3CameraPos.x, xmf3CameraPos.y, xmf3CameraPos.z);

	OnPrepareRender();

	if (m_pMaterial)
	{
		if (m_pMaterial->m_pShader)
		{
			m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
			m_pMaterial->m_pShader->UpdateShaderVariables(pd3dCommandList);

			UpdateShaderVariables(pd3dCommandList);
		}
	}

	if (m_d3dCbvGPUDescriptorHandle.ptr)
		pd3dCommandList->SetGraphicsRootDescriptorTable(2, m_d3dCbvGPUDescriptorHandle);

	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_pMaterial)
			{
				if (m_pMaterial->m_pTexture) m_pMaterial->m_pTexture->UpdateShaderVariable(pd3dCommandList, i);
			}
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CBulletObject::CBulletObject(CGameObject* pMissileFrame, Owner owner)
{
	m_pMissile = pMissileFrame;
	m_pMissile->m_bActive = true;

	m_owner = owner;

	m_fTimer = 0.0f;
	if (m_owner == Owner::Player)
	{
		m_fSpeed = 120.0f;
		m_fSize = 2.5f;
	}
	else
	{
		m_fSpeed = 80.0f;
		m_fSize = 1.2f;
	}
}

CBulletObject::~CBulletObject() {}

void CBulletObject::Animate(float fTimeElapsed, void *pContext)
{
	static CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;
	XMFLOAT3 pos = m_pMissile->GetPosition();

	m_fTimer += fTimeElapsed;
	m_pMissile->MoveForward(m_fSpeed * fTimeElapsed); // 미사일 위치 업데이트
	m_pMissile->UpdateTransform(nullptr); // 미사일 위치 적용
	float height = pTerrain->GetHeight(pos.x, pos.z); // 지형에 부딫혔는지 파악하기 위한 높이 값

	// 폭파 조건
	// 1. 지속 시간 이후 폭파
	// 2. 지형에 부딪힌 경우 폭파
	if (m_fTimer > BULLET_LAST_TIME
		|| height > pos.y)
	{
		m_bActive = m_pMissile->m_bActive = false;
	}
}

void CBulletObject::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	if (!m_bActive) return;

	m_pMissile->OnPrepareRender();

	if (m_pMissile->m_pMaterial)
	{
		if (m_pMissile->m_pMaterial->m_pShader)
		{
			m_pMissile->m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
			m_pMissile->m_pMaterial->m_pShader->UpdateShaderVariables(pd3dCommandList);

			m_pMissile->UpdateShaderVariables(pd3dCommandList);
		}
		if (m_pMissile->m_pMaterial->m_pTexture)
		{
			m_pMissile->m_pMaterial->m_pTexture->UpdateShaderVariables(pd3dCommandList);
		}
	}

	if (m_pMissile->m_ppMeshes && (m_pMissile->m_nMeshes > 0))
	{
		m_pMissile->SetRootParameter(pd3dCommandList);

		for (int i = 0; i < m_pMissile->m_nMeshes; i++)
		{
			if (m_pMissile->m_ppMeshes[i]) m_pMissile->m_ppMeshes[i]->Render(pd3dCommandList);
		}
	}
}

bool CBulletObject::CollisionTest(CGameObject* other)
{
	if (!m_bActive) return false;

	float fDistance = Vector3::Length(Vector3::Subtract(m_pMissile->GetPosition(), other->GetPosition()));
	if (other->m_fSize + m_fSize > fDistance)
	{
		m_bActive = m_pMissile->m_bActive = false;
		other->m_bActive = false;
		return true;
	}
	return false;
}

CGameObject* CBulletObject::GetEffect()
{
	m_pMissile->m_effect->SetPosition(m_pMissile->GetPosition());
	m_pMissile->m_effect->UpdateTransform(nullptr);

	return m_pMissile->m_effect;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
CEffectObject::CEffectObject(CGameObject* effect)
{
	m_pEffect = effect;
	m_bActive = m_pEffect->m_bActive = true;

	m_fTickTimer = m_fTimer = 0.0f;
}

CEffectObject::~CEffectObject(){}

void CEffectObject::Animate(float fTimeElapsed, CCamera* pCamera)
{
	m_fTickTimer += fTimeElapsed;	// 다음 프레임으로 넘겨주기 위한 변수
														// 2초간 재생하며 64장이므로 한 프레임을 0.03125초 동안 재생한다.
	m_fTimer += fTimeElapsed;	// 총 지속 시간 업데이트
	
	// 카메라를 바라보도록 설정
	SetLookAt(pCamera->GetPosition());

	// 재생 시간이 끝나면 이펙트 제거
	if (m_fTimer > BULLET_LAST_TIME)
	{
		m_bActive = m_pEffect->m_bActive = false;	// 이펙트 리스트에서 제거 하기 위해 플래그 설정
		m_pEffect->SetPosition(0.0f, -2000.0f, 0.0f); // 보이지 않는 위치로 이동
		m_pEffect->UpdateTransform(nullptr); // 이동 적용
	}
}

void CEffectObject::SetLookAt(XMFLOAT3& xmf3Target)
{
	static XMFLOAT3 xmf3Up(0.0f, 1.0f, 0.0f);
	XMFLOAT3 xmf3Position = m_pEffect->GetPosition();
	XMFLOAT3 xmf3Look = Vector3::Subtract(xmf3Position, xmf3Target, true);
	XMFLOAT3 xmf3Right = Vector3::CrossProduct(xmf3Up, xmf3Look, true);
	XMFLOAT3 xmf3newUp = Vector3::CrossProduct(xmf3Look, xmf3Right, true);

	m_pEffect->SetToParentTransform(
		XMFLOAT4X4(
			xmf3Right.x, xmf3Right.y, xmf3Right.z, 0.0f,
			xmf3newUp.x, xmf3newUp.y, xmf3newUp.z, 0.0f,
			xmf3Look.x, xmf3Look.y, xmf3Look.z, 0.0f,
			xmf3Position.x, xmf3Position.y, xmf3Position.z, 1.0f
		)
	);
	m_pEffect->UpdateTransform(nullptr);

	// 2초 동안 재생(64개 사진) 한 프레임은 1/32 초 동안 재생되어야 한다.
	// 0.03125 = 1/32
	if (m_fTickTimer < 0.03125) return;

	m_fTickTimer = 0.0f;

	m_pEffect->m_xmf2AddUV.x += 0.125;
	if (m_pEffect->m_xmf2AddUV.x >= 1)
	{
		m_pEffect->m_xmf2AddUV.x = 0;
		m_pEffect->m_xmf2AddUV.y += 0.125;
	}
}