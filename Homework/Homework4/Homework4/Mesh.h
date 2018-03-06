#pragma once

class CVertex
{
protected:
	// ������ ��ġ���� / ��� ����  �ּ��� ��ġ���͸� ������ ��
	XMFLOAT3 m_xmf3Position;
public:
	CVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); }
	CVertex(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	~CVertex() {}
};

class CDiffusedVertex : public CVertex
{
protected:
	//������ �����̴�.
	XMFLOAT4 m_xmf4Diffuse;

public:
	CDiffusedVertex() {
		m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); m_xmf4Diffuse =
			XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	CDiffusedVertex(float x, float y, float z, XMFLOAT4 xmf4Diffuse) {
		m_xmf3Position =
			XMFLOAT3(x, y, z); m_xmf4Diffuse = xmf4Diffuse;
	}
	CDiffusedVertex(XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Diffuse) {
		m_xmf3Position =
			xmf3Position; m_xmf4Diffuse = xmf4Diffuse;
	}
	~CDiffusedVertex() { }
};

class CIlluminatedVertex : public CVertex
{
protected:
	XMFLOAT3						m_xmf3Normal;

public:
	CIlluminatedVertex() {
		m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
	}
	CIlluminatedVertex(float x, float y, float z, XMFLOAT3 xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f))
	{
		m_xmf3Position = XMFLOAT3(x, y, z); m_xmf3Normal = xmf3Normal;
	}
	CIlluminatedVertex(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f))
	{
		m_xmf3Position = xmf3Position; m_xmf3Normal = xmf3Normal;
	}
	~CIlluminatedVertex() { }
};

//////////////////////////////////////////////////////////////////////////////////////////////////////

class CMesh
{
public:
	CMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual ~CMesh();
private:
	int m_nReferences = 0;
public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
	void ReleaseUploadBuffers();
protected:
	ID3D12Resource *m_pd3dVertexBuffer = NULL;
	ID3D12Resource *m_pd3dVertexUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW m_d3dVertexBufferView;
	D3D12_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT m_nSlot = 0;
	UINT m_nVertices = 0;
	UINT m_nStride = 0;
	UINT m_nOffset = 0;
	//�� ��ǥ���� OOBB �ٿ�� �ڽ��̴�.
	BoundingOrientedBox m_xmBoundingBox;

public:
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, UINT nInstances = 1);
	BoundingOrientedBox GetBoundingBox() { return(m_xmBoundingBox); }

	//������ �޽��� ������ �˻��ϰ� �����ϴ� Ƚ���� �Ÿ��� ��ȯ�ϴ� �Լ��̴�.
	int CheckRayIntersection(XMFLOAT3& xmRayPosition, XMFLOAT3& xmRayDirection, float
		*pfNearHitDistance);
	virtual XMFLOAT3 CheckRayIntersection(XMFLOAT3& xmf3RayOrigin, XMFLOAT3& xmf3RayDirection);

protected:
	ID3D12Resource *m_pd3dIndexBuffer = NULL;
	ID3D12Resource *m_pd3dIndexUploadBuffer = NULL;
	/*�ε��� ����(�ε����� �迭)�� �ε��� ���۸� ���� ���ε� ���ۿ� ���� �������̽� �������̴�. �ε��� ���۴� ����
	����(�迭)�� ���� �ε����� ������.*/
	D3D12_INDEX_BUFFER_VIEW m_d3dIndexBufferView;

	UINT m_nIndices = 0;
	//�ε��� ���ۿ� ���ԵǴ� �ε����� �����̴�.
	UINT m_nStartIndex = 0;
	//�ε��� ���ۿ��� �޽��� �׸��� ���� ���Ǵ� ���� �ε����̴�.
	int m_nBaseVertex = 0;
	//�ε��� ������ �ε����� ������ �ε����̴�.

	//������ ��ŷ�� ���Ͽ� �����Ѵ�(���� ���۸� Map()�Ͽ� ���� �ʾƵ� �ǵ���).
	CDiffusedVertex *m_pVertices = NULL;
	//�޽��� �ε����� �����Ѵ�(�ε��� ���۸� Map()�Ͽ� ���� �ʾƵ� �ǵ���).
	UINT *m_pnIndices = NULL;
};

class CTriangleMesh : public CMesh
{
public:
	CTriangleMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual ~CTriangleMesh() { }
};


class CCubeMeshDiffused : public CMesh
{
public:
	//������ü�� ����, ����, ������ ���̸� �����Ͽ� ������ü �޽��� �����Ѵ�.
	CCubeMeshDiffused(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeMeshDiffused();
};

///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// CAirplaneMeshDiffused /////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

class CAirplaneMeshDiffused : public CMesh
{
public:
	CAirplaneMeshDiffused(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList, float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 4.0f,
		XMFLOAT4 xmf4Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f));
	virtual ~CAirplaneMeshDiffused();
};


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// Height Map Image //////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

class CHeightMapImage
{
private:
	//���� �� �̹��� �ȼ�(8-��Ʈ)���� ������ �迭�̴�. �� �ȼ��� 0~255�� ���� ���´�.
	BYTE *m_pHeightMapPixels;
	//���� �� �̹����� ���ο� ���� ũ���̴�.
	int m_nWidth;
	int m_nLength;
	//���� �� �̹����� ������ �� �� Ȯ���Ͽ� ����� ���ΰ��� ��Ÿ���� ������ �����̴�.
	XMFLOAT3 m_xmf3Scale;

public:
	CHeightMapImage(LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale);
	~CHeightMapImage(void);

	//���� �� �̹������� (x, z) ��ġ�� �ȼ� ���� ����� ������ ���̸� ��ȯ�Ѵ�.
	float GetHeight(float x, float z);
	//���� �� �̹������� (x, z) ��ġ�� ���� ���͸� ��ȯ�Ѵ�.
	XMFLOAT3 GetHeightMapNormal(int x, int z);

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }

	BYTE *GetHeightMapPixels() { return(m_pHeightMapPixels); }
	int GetHeightMapWidth() { return(m_nWidth); }
	int GetHeightMapLength() { return(m_nLength); }
};


//////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// Height Map Grid Mesh ////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

class CHeightMapGridMesh : public CMesh
{
protected:
	//������ ũ��(����: x-����, ����: z-����)�̴�.
	int m_nWidth;
	int m_nLength;
	/*������ ������(����: x-����, ����: z-����, ����: y-����) �����̴�. ���� ���� �޽��� �� ������ x-��ǥ, y-��ǥ,
	z-��ǥ�� ������ ������ x-��ǥ, y-��ǥ, z-��ǥ�� ���� ���� ���´�. ��, ���� ������ x-�� ������ ������ 1�� �ƴ�
	�� ������ ������ x-��ǥ�� �ȴ�. �̷��� �ϸ� ���� ����(���� ����)�� ����ϴ��� ū ũ���� ����(����)�� ������
	�� �ִ�.*/
	XMFLOAT3 m_xmf3Scale;

public:
	CHeightMapGridMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList, int xStart, int zStart, int nWidth, int nLength, XMFLOAT3 xmf3Scale =
		XMFLOAT3(1.0f, 1.0f, 1.0f), void	*pContext = NULL);
	virtual ~CHeightMapGridMesh();

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	int GetWidth() { return(m_nWidth); }
	int GetLength() { return(m_nLength); }

	//������ ��ǥ�� (x, z)�� �� ����(����)�� ���̸� ��ȯ�ϴ� �Լ��̴�.
	virtual float OnGetHeight(int x, int z, void *pContext);
	//������ ��ǥ�� (x, z)�� �� ����(����)�� ������ ��ȯ�ϴ� �Լ��̴�.
	virtual XMFLOAT4 OnGetColor(int x, int z, void *pContext);
};


//////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// Sphere Mesh Diffused //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

class CSphereMeshDiffused : public CMesh
{
public:
	CSphereMeshDiffused(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList, float fRadius = 2.0f, int nSlices = 20, int nStacks = 20);
	virtual ~CSphereMeshDiffused();
};


//////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// Roof  Mesh Diffused //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

class CRoofMeshDiffussed : public CMesh
{
public:
	CRoofMeshDiffussed(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList,
		float fWidth = 660.0f, float fHeight = 350.0f, float fDepth = 660.0f, XMFLOAT4 xmf4Color = XMFLOAT4(0.3f, 0.24f, 0.24f, 0.0f));
	virtual ~CRoofMeshDiffussed();
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CMeshIlluminated : public CMesh
{
public:
	CMeshIlluminated(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual ~CMeshIlluminated();

public:
	void CalculateTriangleListVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, int nVertices);
	void CalculateTriangleListVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, UINT nVertices, UINT *pnIndices, UINT nIndices);
	void CalculateTriangleStripVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, UINT nVertices, UINT *pnIndices, UINT nIndices);
	void CalculateVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, int nVertices, UINT *pnIndices, int nIndices);

	virtual XMFLOAT3 CheckRayIntersection(XMFLOAT3& xmf3RayOrigin, XMFLOAT3& xmf3RayDirection);

protected:
	CIlluminatedVertex *m_pVertices = NULL;
};

class CCubeMeshIlluminated : public CMeshIlluminated
{
public:
	CCubeMeshIlluminated(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeMeshIlluminated();
};

class CSphereMeshIlluminated : public CMeshIlluminated
{
public:
	CSphereMeshIlluminated(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fRadius = 2.0f, UINT nSlices = 20, UINT nStacks = 20);
	virtual ~CSphereMeshIlluminated();
};


class CRoofMeshIlluminated : public CMeshIlluminated
{
public:
	CRoofMeshIlluminated(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList,
		float fWidth = 660.0f, float fHeight = 350.0f, float fDepth = 660.0f);
	virtual ~CRoofMeshIlluminated();
};