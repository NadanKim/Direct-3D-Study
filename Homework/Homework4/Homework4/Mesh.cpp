#include "stdafx.h"
#include "Mesh.h"


CMesh::CMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) {

}

CMesh::~CMesh()
{
	if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();

	if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release();
	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();

	if (m_pVertices) delete[] m_pVertices;
	if (m_pnIndices) delete[] m_pnIndices;
}

void CMesh::ReleaseUploadBuffers()
{
	//���� ���۸� ���� ���ε� ���۸� �Ҹ��Ų��.
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	m_pd3dVertexUploadBuffer = NULL;

	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();
	m_pd3dIndexUploadBuffer = NULL;
};

void CMesh::Render(ID3D12GraphicsCommandList *pd3dCommandList, UINT nInstance)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);
	if (m_pd3dIndexBuffer)
	{
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, nInstance, 0, 0, 0);
		//�ε��� ���۰� ������ �ε��� ���۸� ����������(IA: �Է� ������)�� �����ϰ� �ε����� ����Ͽ� �������Ѵ�.
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, nInstance, m_nOffset, 0);
	}
}


int CMesh::CheckRayIntersection(XMFLOAT3& xmf3RayOrigin, XMFLOAT3& xmf3RayDirection,
	float *pfNearHitDistance)
{
	//�ϳ��� �޽����� ������ ���� ���� �ﰢ���� ������ �� �ִ�. �����ϴ� �ﰢ���� �� ���� ����� �ﰢ���� ã�´�.
	int nIntersections = 0;
	BYTE *pbPositions = (BYTE *)m_pVertices;

	int nOffset = (m_d3dPrimitiveTopology == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? 3 : 1;
	/*�޽��� ������Ƽ��(�ﰢ��)���� �����̴�. �ﰢ�� ����Ʈ�� ��� (������ ���� / 3) �Ǵ� (�ε����� ���� / 3), �ﰢ
	�� ��Ʈ���� ��� (������ ���� - 2) �Ǵ� (�ε����� ���� ? 2)�̴�.*/
	int nPrimitives = (m_d3dPrimitiveTopology == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ?
		(m_nVertices / 3) : (m_nVertices - 2);
	if (m_nIndices > 0) nPrimitives = (m_d3dPrimitiveTopology ==
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nIndices / 3) : (m_nIndices - 2);

	//������ �� ��ǥ��� ǥ���ȴ�.
	XMVECTOR xmRayOrigin = XMLoadFloat3(&xmf3RayOrigin);
	XMVECTOR xmRayDirection = XMLoadFloat3(&xmf3RayDirection);
	//�� ��ǥ���� ������ �޽��� �ٿ�� �ڽ�(�� ��ǥ��)���� ������ �˻��Ѵ�.
	bool bIntersected = m_xmBoundingBox.Intersects(xmRayOrigin, xmRayDirection,
		*pfNearHitDistance);
	//�� ��ǥ���� ������ �޽��� �ٿ�� �ڽ��� �����ϸ� �޽����� ������ �˻��Ѵ�.
	if (bIntersected)
	{
		float fNearHitDistance = FLT_MAX;
		/*�޽��� ��� ������Ƽ��(�ﰢ��)�鿡 ���Ͽ� ��ŷ �������� �浹�� �˻��Ѵ�. �浹�ϴ� ��� �ﰢ���� ã�� ������
		������(�����δ� ī�޶� ��ǥ���� ����)�� ���� ����� �ﰢ���� ã�´�.*/
		for (int i = 0; i < nPrimitives; i++)
		{
			XMVECTOR v0 = XMLoadFloat3((XMFLOAT3 *)(pbPositions + ((m_pnIndices) ?
				(m_pnIndices[(i*nOffset) + 0]) : ((i*nOffset) + 0)) * m_nStride));
			XMVECTOR v1 = XMLoadFloat3((XMFLOAT3 *)(pbPositions + ((m_pnIndices) ?
				(m_pnIndices[(i*nOffset) + 1]) : ((i*nOffset) + 1)) * m_nStride));
			XMVECTOR v2 = XMLoadFloat3((XMFLOAT3 *)(pbPositions + ((m_pnIndices) ?
				(m_pnIndices[(i*nOffset) + 2]) : ((i*nOffset) + 2)) * m_nStride));
			float fHitDistance;
			BOOL bIntersected = TriangleTests::Intersects(xmRayOrigin, xmRayDirection, v0,
				v1, v2, fHitDistance);
			if (bIntersected)
			{
				if (fHitDistance < fNearHitDistance)
				{
					*pfNearHitDistance = fNearHitDistance = fHitDistance;
				}
				nIntersections++;
			}
		}
	}
	return(nIntersections);
}

XMFLOAT3 CMesh::CheckRayIntersection(XMFLOAT3& xmf3RayOrigin, XMFLOAT3& xmf3RayDirection)
{
	//�ϳ��� �޽����� ������ ���� ���� �ﰢ���� ������ �� �ִ�. �����ϴ� �ﰢ���� �� ���� ����� �ﰢ���� ã�´�.
	int nIntersections = 0;
	BYTE *pbPositions = (BYTE *)m_pVertices;

	int nOffset = (m_d3dPrimitiveTopology == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? 3 : 1;
	/*�޽��� ������Ƽ��(�ﰢ��)���� �����̴�. �ﰢ�� ����Ʈ�� ��� (������ ���� / 3) �Ǵ� (�ε����� ���� / 3), �ﰢ
	�� ��Ʈ���� ��� (������ ���� - 2) �Ǵ� (�ε����� ���� ? 2)�̴�.*/
	int nPrimitives = (m_d3dPrimitiveTopology == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ?
		(m_nVertices / 3) : (m_nVertices - 2);
	if (m_nIndices > 0) nPrimitives = (m_d3dPrimitiveTopology ==
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nIndices / 3) : (m_nIndices - 2);

	//������ �� ��ǥ��� ǥ���ȴ�.
	XMVECTOR xmRayOrigin = XMLoadFloat3(&xmf3RayOrigin);
	XMVECTOR xmRayDirection = XMLoadFloat3(&xmf3RayDirection);
	float fNearHitDistance = FLT_MAX;

	/*�޽��� ��� ������Ƽ��(�ﰢ��)�鿡 ���Ͽ� ��ŷ �������� �浹�� �˻��Ѵ�. �浹�ϴ� ��� �ﰢ���� ã�� ������
	������(�����δ� ī�޶� ��ǥ���� ����)�� ���� ����� �ﰢ���� ã�´�.*/
	XMVECTOR normalVector;
	for (int i = 0; i < nPrimitives; i++)
	{
		XMVECTOR v0 = XMLoadFloat3((XMFLOAT3 *)(pbPositions + ((m_pnIndices) ?
			(m_pnIndices[(i*nOffset) + 0]) : ((i*nOffset) + 0)) * m_nStride));
		XMVECTOR v1 = XMLoadFloat3((XMFLOAT3 *)(pbPositions + ((m_pnIndices) ?
			(m_pnIndices[(i*nOffset) + 1]) : ((i*nOffset) + 1)) * m_nStride));
		XMVECTOR v2 = XMLoadFloat3((XMFLOAT3 *)(pbPositions + ((m_pnIndices) ?
			(m_pnIndices[(i*nOffset) + 2]) : ((i*nOffset) + 2)) * m_nStride));

		float fHitDistance;
		BOOL bIntersected = TriangleTests::Intersects(xmRayOrigin, xmRayDirection, v0,
			v1, v2, fHitDistance);
		if (bIntersected)
		{
			if (fHitDistance < fNearHitDistance)
			{
				XMVECTOR vector1 = v0 - v1;
				XMVECTOR vector2 = v2 - v1;
				normalVector = XMVector3Cross(vector1, vector2);
				fNearHitDistance = fHitDistance;
			}
			nIntersections++;
		}
	}

	XMFLOAT3 returnVector;
	XMStoreFloat3(&returnVector, -normalVector);

	return(Vector3::Normalize(returnVector));
}

////////////////// TriangleMesh ////////////////////////////

CTriangleMesh::CTriangleMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList) : CMesh(pd3dDevice, pd3dCommandList)
{
	//�ﰢ�� �޽��� �����Ѵ�.
	m_nVertices = 3;
	m_nStride = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	CDiffusedVertex pVertices[3];
	pVertices[0] = CDiffusedVertex(XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f,
		1.0f));
	pVertices[1] = CDiffusedVertex(XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f,
		1.0f));
	pVertices[2] = CDiffusedVertex(XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT4(Colors::Blue));
	//�ﰢ�� �޽��� ���ҽ�(���� ����)�� �����Ѵ�.
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	//���� ���� �並 �����Ѵ�.
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

////////////////////////CubeMeshDiffused////////////////////////////////////

CCubeMeshDiffused::CCubeMeshDiffused(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList, float fWidth, float fHeight, float fDepth) : CMesh(pd3dDevice,
		pd3dCommandList)
{
	//������ü�� ������(����)�� 8���̴�.
	m_nVertices = 8;
	m_nStride = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	//���� ���۴� ������ü�� ������ 8���� ���� ���� �����͸� ������.
	m_pVertices = new CDiffusedVertex[m_nVertices];
	m_pVertices[0] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR);
	m_pVertices[1] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	m_pVertices[2] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
	m_pVertices[3] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	m_pVertices[4] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	m_pVertices[5] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);
	m_pVertices[6] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	m_pVertices[7] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pVertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	/*�ε��� ���۴� ������ü�� 6���� ��(�簢��)�� ���� ���� ������ ���´�. �ﰢ�� ����Ʈ�� ������ü�� ǥ���� ����
	�Ƿ� �� ���� 2���� �ﰢ���� ������ �� �ﰢ���� 3���� ������ �ʿ��ϴ�. ��, �ε��� ���۴� ��ü 36(=6*2*3)���� ��
	������ ������ �Ѵ�.*/
	m_nIndices = 36;

	m_pnIndices = new UINT[m_nIndices];
	//�� �ո�(Front) �簢���� ���� �ﰢ��
	m_pnIndices[0] = 3; m_pnIndices[1] = 1; m_pnIndices[2] = 0;
	//�� �ո�(Front) �簢���� �Ʒ��� �ﰢ��
	m_pnIndices[3] = 2; m_pnIndices[4] = 1; m_pnIndices[5] = 3;
	//�� ����(Top) �簢���� ���� �ﰢ��
	m_pnIndices[6] = 0; m_pnIndices[7] = 5; m_pnIndices[8] = 4;
	//�� ����(Top) �簢���� �Ʒ��� �ﰢ��
	m_pnIndices[9] = 1; m_pnIndices[10] = 5; m_pnIndices[11] = 0;
	//�� �޸�(Back) �簢���� ���� �ﰢ��
	m_pnIndices[12] = 3; m_pnIndices[13] = 4; m_pnIndices[14] = 7;
	//�� �޸�(Back) �簢���� �Ʒ��� �ﰢ��
	m_pnIndices[15] = 0; m_pnIndices[16] = 4; m_pnIndices[17] = 3;
	//�� �Ʒ���(Bottom) �簢���� ���� �ﰢ��
	m_pnIndices[18] = 1; m_pnIndices[19] = 6; m_pnIndices[20] = 5;
	//�� �Ʒ���(Bottom) �簢���� �Ʒ��� �ﰢ��
	m_pnIndices[21] = 2; m_pnIndices[22] = 6; m_pnIndices[23] = 1;
	//�� ����(Left) �簢���� ���� �ﰢ��
	m_pnIndices[24] = 2; m_pnIndices[25] = 7; m_pnIndices[26] = 6;
	//�� ����(Left) �簢���� �Ʒ��� �ﰢ��
	m_pnIndices[27] = 3; m_pnIndices[28] = 7; m_pnIndices[29] = 2;
	//�� ����(Right) �簢���� ���� �ﰢ��
	m_pnIndices[30] = 6; m_pnIndices[31] = 4; m_pnIndices[32] = 5;
	//�� ����(Right) �簢���� �Ʒ��� �ﰢ��
	m_pnIndices[33] = 7; m_pnIndices[34] = 4; m_pnIndices[35] = 6;

	//�ε��� ���۸� �����Ѵ�.
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pnIndices,
		sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER,
		&m_pd3dIndexUploadBuffer);

	//�ε��� ���� �並 �����Ѵ�.
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;

	//�޽��� �ٿ�� �ڽ�(�� ��ǥ��)�� �����Ѵ�.
	m_xmBoundingBox = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fx, fy,
		fz), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}


CCubeMeshDiffused::~CCubeMeshDiffused()
{
}


///////////////////////AirplaneMeshDiffused//////////////////////////

CAirplaneMeshDiffused::CAirplaneMeshDiffused(ID3D12Device *pd3dDevice,
	ID3D12GraphicsCommandList *pd3dCommandList, float fWidth, float fHeight, float fDepth,
	XMFLOAT4 xmf4Color) : CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 24 * 3;
	m_nStride = sizeof(CDiffusedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	//���� �׸��� ���� ����� �޽��� ǥ���ϱ� ���� ���� �������̴�.
	m_pVertices = new CDiffusedVertex[m_nVertices];
	float x1 = fx * 0.2f, y1 = fy * 0.2f, x2 = fx * 0.1f, y3 = fy * 0.3f, y2 = ((y1 - (fy -
		y3)) / x1) * x2 + (fy - y3);
	int i = 0;

	//����� �޽��� ���� ��
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz),
		Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));

	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz),
		Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));

	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));

	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));

	//����� �޽��� �Ʒ��� ��
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz),
		Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));

	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz),
		Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));

	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));

	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));

	//����� �޽��� ������ ��
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz),
		Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz),
		Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));

	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz),
		Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));

	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));

	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));

	//����� �޽��� ����/������ ��
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));

	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));

	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));

	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));

	//����� �޽��� ���� ��
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz),
		Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz),
		Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));

	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz),
		Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));

	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));

	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));

	//����� �޽��� ����/���� ��
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));

	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));

	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));

	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pVertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	//�޽��� �ٿ�� �ڽ�(�� ��ǥ��)�� �����Ѵ�.
	m_xmBoundingBox = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fx, fy,
		fz), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}


CAirplaneMeshDiffused::~CAirplaneMeshDiffused()
{
}


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// Height Map Image //////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

CHeightMapImage::CHeightMapImage(LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3
	xmf3Scale)
{
	m_nWidth = nWidth;
	m_nLength = nLength;
	m_xmf3Scale = xmf3Scale;
	BYTE *pHeightMapPixels = new BYTE[m_nWidth * m_nLength];
	//������ ���� �д´�. ���� �� �̹����� ���� ����� ���� RAW �̹����̴�.
	HANDLE hFile = ::CreateFile(pFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY, NULL);
	DWORD dwBytesRead;
	::ReadFile(hFile, pHeightMapPixels, (m_nWidth * m_nLength), &dwBytesRead, NULL);
	::CloseHandle(hFile);

	/*�̹����� y-��� ������ z-���� ������ �ݴ��̹Ƿ� �̹����� ���ϴ�Ī ���� �����Ѵ�. �׷��� ���� �׸��� ���� ��
	������ ��ǥ��� ������ ��ǥ���� ������ ��ġ�ϰ� �ȴ�.*/

	m_pHeightMapPixels = new BYTE[m_nWidth * m_nLength];
	for (int y = 0; y < m_nLength; y++)
	{
		for (int x = 0; x < m_nWidth; x++)
		{
			m_pHeightMapPixels[x + ((m_nLength - 1 - y)*m_nWidth)] = pHeightMapPixels[x +
				(y*m_nWidth)];
		}
	}

	if (pHeightMapPixels) delete[] pHeightMapPixels;
}


CHeightMapImage::~CHeightMapImage()
{
	if (m_pHeightMapPixels) delete[] m_pHeightMapPixels;
	m_pHeightMapPixels = NULL;
}


XMFLOAT3 CHeightMapImage::GetHeightMapNormal(int x, int z)
{
	//x-��ǥ�� z-��ǥ�� ���� ���� ������ ����� ������ ���� ���ʹ� y-�� ���� �����̴�.
	if ((x < 0.0f) || (z < 0.0f) || (x >= m_nWidth) || (z >= m_nLength))
		return(XMFLOAT3(0.0f, 1.0f, 0.0f));

	/*���� �ʿ��� (x, z) ��ǥ�� �ȼ� ���� ������ �� ���� �� (x+1, z), (z, z+1)�� ���� �ȼ� ���� ����Ͽ� ���� ���͸�
	����Ѵ�.*/
	int nHeightMapIndex = x + (z * m_nWidth);
	int xHeightMapAdd = (x < (m_nWidth - 1)) ? 1 : -1;
	int zHeightMapAdd = (z < (m_nLength - 1)) ? m_nWidth : -m_nWidth;
	//(x, z), (x+1, z), (z, z+1)�� �ȼ����� ������ ���̸� ���Ѵ�.
	float y1 = (float)m_pHeightMapPixels[nHeightMapIndex] * m_xmf3Scale.y;
	float y2 = (float)m_pHeightMapPixels[nHeightMapIndex + xHeightMapAdd] * m_xmf3Scale.y;
	float y3 = (float)m_pHeightMapPixels[nHeightMapIndex + zHeightMapAdd] * m_xmf3Scale.y;
	//xmf3Edge1�� (0, y3, m_xmf3Scale.z) - (0, y1, 0) �����̴�.
	XMFLOAT3 xmf3Edge1 = XMFLOAT3(0.0f, y3 - y1, m_xmf3Scale.z);
	//xmf3Edge2�� (m_xmf3Scale.x, y2, 0) - (0, y1, 0) �����̴�.
	XMFLOAT3 xmf3Edge2 = XMFLOAT3(m_xmf3Scale.x, y2 - y1, 0.0f);
	//���� ���ʹ� xmf3Edge1�� xmf3Edge2�� ������ ����ȭ�ϸ� �ȴ�.
	XMFLOAT3 xmf3Normal = Vector3::CrossProduct(xmf3Edge1, xmf3Edge2, true);

	return(xmf3Normal);
}


#define _WITH_APPROXIMATE_OPPOSITE_CORNER

float CHeightMapImage::GetHeight(float fx, float fz)
{
	/*������ ��ǥ (fx, fz)�� �̹��� ��ǥ���̴�. ���� ���� x-��ǥ�� z-��ǥ�� ���� ���� ������ ����� ������ ���̴�
	0�̴�.*/
	if ((fx < 0.0f) || (fz < 0.0f) || (fx >= m_nWidth) || (fz >= m_nLength)) return(0.0f);
	//���� ���� ��ǥ�� ���� �κа� �Ҽ� �κ��� ����Ѵ�.
	int x = (int)fx;
	int z = (int)fz;
	float fxPercent = fx - x;
	float fzPercent = fz - z;

	float fBottomLeft = (float)m_pHeightMapPixels[x + (z*m_nWidth)];
	float fBottomRight = (float)m_pHeightMapPixels[(x + 1) + (z*m_nWidth)];
	float fTopLeft = (float)m_pHeightMapPixels[x + ((z + 1)*m_nWidth)];
	float fTopRight = (float)m_pHeightMapPixels[(x + 1) + ((z + 1)*m_nWidth)];
#ifdef _WITH_APPROXIMATE_OPPOSITE_CORNER
	//z-��ǥ�� 1, 3, 5, ...�� ��� �ε����� �����ʿ��� �������� �����ȴ�.
	bool bRightToLeft = ((z % 2) != 0);

	if (bRightToLeft)
	{
		/*������ �ﰢ������ �����ʿ��� ���� �������� �����Ǵ� ����̴�. ���� �׸��� �������� (fzPercent < fxPercent)
		�� ����̴�. �� ��� TopLeft�� �ȼ� ���� (fTopLeft = fTopRight + (fBottomLeft - fBottomRight))�� �ٻ��Ѵ�.
		���� �׸��� ������ (fzPercent �� fxPercent)�� ����̴�. �� ��� BottomRight�� �ȼ� ���� (fBottomRight =
		fBottomLeft + (fTopRight - fTopLeft))�� �ٻ��Ѵ�.*/
		if (fzPercent >= fxPercent)
			fBottomRight = fBottomLeft + (fTopRight - fTopLeft);
		else
			fTopLeft = fTopRight + (fBottomLeft - fBottomRight);
	}
	else
	{
		/*������ �ﰢ������ ���ʿ��� ������ �������� �����Ǵ� ����̴�. ���� �׸��� ������ (fzPercent < (1.0f -
		fxPercent))�� ����̴�. �� ��� TopRight�� �ȼ� ���� (fTopRight = fTopLeft + (fBottomRight - fBottomLeft))��
		�ٻ��Ѵ�. ���� �׸��� �������� (fzPercent �� (1.0f - fxPercent))�� ����̴�. �� ��� BottomLeft�� �ȼ� ����
		(fBottomLeft = fTopLeft + (fBottomRight - fTopRight))�� �ٻ��Ѵ�.*/
		if (fzPercent < (1.0f - fxPercent))
			fTopRight = fTopLeft + (fBottomRight - fBottomLeft);
		else
			fBottomLeft = fTopLeft + (fBottomRight - fTopRight);
	}
#endif
	//�簢���� �� ���� �����Ͽ� ����(�ȼ� ��)�� ����Ѵ�.
	float fTopHeight = fTopLeft * (1 - fxPercent) + fTopRight * fxPercent;
	float fBottomHeight = fBottomLeft * (1 - fxPercent) + fBottomRight * fxPercent;
	float fHeight = fBottomHeight * (1 - fzPercent) + fTopHeight * fzPercent;
	return(fHeight);
}


CHeightMapGridMesh::CHeightMapGridMesh(ID3D12Device *pd3dDevice,
	ID3D12GraphicsCommandList *pd3dCommandList, int xStart, int zStart, int nWidth, int
	nLength, XMFLOAT3 xmf3Scale, void *pContext) : CMesh(pd3dDevice,
		pd3dCommandList)
{
	//������ ����(����)�� ������ (nWidth * nLength)�̴�.
	m_nVertices = nWidth * nLength;
	m_nStride = sizeof(CIlluminatedVertex);

	//���ڴ� �ﰢ�� ��Ʈ������ �����Ѵ�.
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	m_nWidth = nWidth;
	m_nLength = nLength;
	m_xmf3Scale = xmf3Scale;

	CIlluminatedVertex *pVertices = new CIlluminatedVertex[m_nVertices];

	CHeightMapImage *pHeightMapImage = (CHeightMapImage *)pContext;

	/*xStart�� zStart�� ������ ���� ��ġ(x-��ǥ�� z-��ǥ)�� ��Ÿ����. Ŀ�ٶ� ������ ���ڵ��� ������ �迭�� ���� ��
	�䰡 �ֱ� ������ ��ü �������� �� ������ ���� ��ġ�� ��Ÿ���� ������ �ʿ��ϴ�.*/
	float fHeight = 0.0f, fMinHeight = +FLT_MAX, fMaxHeight = -FLT_MAX;
	for (int i = 0, z = zStart; z < (zStart + nLength); z++)
	{
		for (int x = xStart; x < (xStart + nWidth); x++, i++)
		{
			//������ ���̿� ������ ���� �����κ��� ���Ѵ�.
			XMFLOAT3 xmf3Position = XMFLOAT3((x*m_xmf3Scale.x), OnGetHeight(x, z, pContext),
				(z*m_xmf3Scale.z));
			pVertices[i] = CIlluminatedVertex(xmf3Position, pHeightMapImage->GetHeightMapNormal(x, z));
			if (fHeight < fMinHeight) fMinHeight = fHeight;
			if (fHeight > fMaxHeight) fMaxHeight = fHeight;
		}
	}
	//���� �׸��� ������ ����(����)�� �����ϴ� ������ �����ش�.
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	delete[] pVertices;
	/*���ڴ� �簢������ �����̰� �簢���� �� ���� �ﰢ������ �����ǹǷ� ���ڴ� ���� �׸��� ���� �ﰢ������ ������
	��� �� �� �ִ�. ���ڸ� ǥ���ϱ� ���Ͽ� ������ �ﰢ������ ���� ������ �ε����� ǥ���ؾ� �Ѵ�. �ﰢ�� ��Ʈ����
	����Ͽ� �ﰢ������ ǥ���ϱ� ���Ͽ� �ﰢ������ �簢���� �� ������ �Ʒ����� ���� ��������(z-��) �����Ѵ�. ù ��
	° �簢�� ���� �ﰢ������ ���ʿ��� ����������(x-��) �����Ѵ�. �� ��° ���� �ﰢ������ �����ʿ��� ���� ��������
	�����Ѵ�. ��, �簢���� ���� �ٲ� ������ ���� ������ �ٲ�� �Ѵ�. ���� �׸��� ���ڿ� ���Ͽ� �ﰢ�� ��Ʈ���� ��
	���Ͽ� �ﰢ������ ǥ���ϱ� ���� �ε����� ������ ������ ���� ������ m��° �ٰ� (m+1)��° ���� ���� ��ȣ�� ��
	������ ���� ���⿡ ���� ������ �Ʒ�, ��, �Ʒ�, ��, ... ������ �����ϸ� �ȴ�.
	0, 6, 1, 7, 2, 8, 3, 9, 4, 10, 5, 11, 11, 17, 10, 16, 9, 15, 8, 14, 7, 13, 6, 12
	�̷��� �ε����� �����ϸ� �ﰢ�� ��Ʈ���� ����� ���̹Ƿ� ���� �׷����� �ﰢ������ �ε����� ������ ����.
	(0, 6, 1), (1, 6, 7), (1, 7, 2), (2, 7, 8), (2, 8, 3), (3, 8, 9), ...
	�׷��� �̷��� �ε����� ����ϸ� ù ��° ���� �����ϰ� �ﰢ������ ����� �׷����� �ʴ´�. �ֳ��ϸ� �ﰢ�� ��Ʈ
	�������� ������ 2���� ������ ���ο� �ϳ��� ������ ����Ͽ� ���ο� �ﰢ���� �׸���. �׸��� Ȧ�� ��° �ﰢ����
	���� ���� ����(���ε� ����)�� �ð�����̰� ¦�� ��° �ﰢ���� ���ε� ������ �ݽð�����̾�� �Ѵ�. ������ �簢
	���� �� �ٿ��� �� ���� �ִ��� ������� �� ���� ������ �ﰢ���� ¦�� ��° �ﰢ���̰� ���ε� ������ �ݽð� ����
	�̴�. �ֳ��ϸ� �簢���� �� ���� �ﰢ������ ���������� �����̴�. ù ��° �ٿ��� �� ��° ���� �ε��� ������ ����
	�׷����� �ﰢ������ �ε����� ���캸��.
	..., 4, 10, 5, 11, 11, 17, 10, 16, 9, 15, 8, 14, 7, 13, 6, 12, ...
	..., (4, 10, 5), (5, 10, 11), (5, 11, 11), (11, 11, 17), (11, 17, 10), ...
	�ﰢ�� (5, 10, 11)�� ù ��° ���� ������ �ﰢ���̰� ¦�� ��°�̴�. �ﰢ�� (11, 17, 10)�� �� ��° ���� ù ��°
	�ﰢ���̰� Ȧ�� ��°�̴�. Ȧ�� ��°�̹Ƿ� ���ε� ������ �ð�����̾�� �ϴµ� ���� ���ε� ������ �ݽð������
	�Ƿ� �׷����� ���� ���̴�. �翬�� ���� �ﰢ���� ���ε� ������ ���� �����Ƿ� �׷����� ���� ���̴�. �ﰢ�� (11,
	17, 10)�� ���ε� ������ �ݽð�����̹Ƿ� �׷������� �Ϸ��� �� �ﰢ���� ¦�� ��° �ﰢ���� �ǵ��� �ؾ� �Ѵ�. ��
	�� ���ؼ� ���� �ٲ� ������ ������ ������ �ε����� �߰��ϵ��� ����. �׷��� ���� �ٲ� ù ��° �ﰢ���� ¦�� ��°
	�ﰢ���� �ȴ�. ������ �������� 11�� �߰��� ������ ������ �ε����̴�. �̷��� �ϸ� �ﰢ���� ������ �� ��� ��
	������ �ʴ� �ﰢ���� �� �ٸ��� 3���� ����� �ȴ�.
	..., 4, 10, 5, 11, 11, 11, 17, 10, 16, 9, 15, 8, 14, 7, 13, 6, 12, ...
	..., (5, 10, 11), (5, 11, 11), (11, 11, 11), (11, 11, 17), (11, 17, 10), ...
	�� ���� �ﰢ�� (5, 11, 11), (11, 11, 11), (11, 11, 17)�� �ﰢ���� ������ �� �����Ƿ� ������ �׷����� �ʴ´�.*/

	/*�̷��� �ε����� �����ϸ� �ε��� ���۴� ((nWidth*2)*(nLength-1))+((nLength-1)-1)���� �ε����� ���´�. �簢
	�� ���� ������ (nLength-1)�̰� �� �ٿ��� (nWidth*2)���� �ε����� ���´�. �׸��� ���� �ٲ� ������ �ε����� ��
	�� �߰��ϹǷ� (nLength-1)-1���� �ε����� �߰��� �ʿ��ϴ�.*/
	m_nIndices = ((nWidth * 2)*(nLength - 1)) + ((nLength - 1) - 1);
	UINT *pnIndices = new UINT[m_nIndices];
	for (int j = 0, z = 0; z < nLength - 1; z++)
	{
		if ((z % 2) == 0)
		{
			//Ȧ�� ��° ���̹Ƿ�(z = 0, 2, 4, ...) �ε����� ���� ������ ���ʿ��� ������ �����̴�.
			for (int x = 0; x < nWidth; x++)
			{
				//ù ��° ���� �����ϰ� ���� �ٲ� ������(x == 0) ù ��° �ε����� �߰��Ѵ�.
				if ((x == 0) && (z > 0)) pnIndices[j++] = (UINT)(x + (z * nWidth));
				//�Ʒ�(x, z), ��(x, z+1)�� ������ �ε����� �߰��Ѵ�.
				pnIndices[j++] = (UINT)(x + (z * nWidth));
				pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
		else
		{
			//¦�� ��° ���̹Ƿ�(z = 1, 3, 5, ...) �ε����� ���� ������ �����ʿ��� ���� �����̴�.
			for (int x = nWidth - 1; x >= 0; x--)
			{
				//���� �ٲ� ������(x == (nWidth-1)) ù ��° �ε����� �߰��Ѵ�.
				if (x == (nWidth - 1)) pnIndices[j++] = (UINT)(x + (z * nWidth));
				//�Ʒ�(x, z), ��(x, z+1)�� ������ �ε����� �߰��Ѵ�.
				pnIndices[j++] = (UINT)(x + (z * nWidth));
				pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
	}

	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices,
		sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER,
		&m_pd3dIndexUploadBuffer);

	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;

	delete[] pnIndices;
}


CHeightMapGridMesh::~CHeightMapGridMesh()
{
}


//���� �� �̹����� �ȼ� ���� ������ ���̷� ��ȯ�Ѵ�.
float CHeightMapGridMesh::OnGetHeight(int x, int z, void *pContext)
{
	CHeightMapImage *pHeightMapImage = (CHeightMapImage *)pContext;
	BYTE *pHeightMapPixels = pHeightMapImage->GetHeightMapPixels();
	XMFLOAT3 xmf3Scale = pHeightMapImage->GetScale();
	int nWidth = pHeightMapImage->GetHeightMapWidth();
	float fHeight = pHeightMapPixels[x + (z*nWidth)] * xmf3Scale.y;
	return(fHeight);
}


XMFLOAT4 CHeightMapGridMesh::OnGetColor(int x, int z, void *pContext)
{
	//������ ���� ����(�������� ��������� ����)�̴�.
	XMFLOAT3 xmf3LightDirection = XMFLOAT3(-1.0f, 1.0f, 1.0f);
	xmf3LightDirection = Vector3::Normalize(xmf3LightDirection);
	CHeightMapImage *pHeightMapImage = (CHeightMapImage *)pContext;
	XMFLOAT3 xmf3Scale = pHeightMapImage->GetScale();
	//������ ����(����, ���)�̴�.
	XMFLOAT4 xmf4IncidentLightColor(0.9f, 0.8f, 0.4f, 1.0f);
	/*���� (x, z)���� ������ �ݻ�Ǵ� ��(����)�� ���� (x, z)�� ���� ���Ϳ� ������ ���� ������ ����(cos)�� ������ 3��
	�� ���� (x+1, z), (x, z+1), (x+1, z+1)�� ���� ���Ϳ� ������ ���� ������ ������ ����Ͽ� ���Ѵ�. ���� (x, z)�� ��
	���� ���� ����(����)�� �ݻ�Ǵ� ��(����)�� ���� ���̴�.*/
	float fScale = Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x, z),
		xmf3LightDirection);
	fScale += Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x + 1, z),
		xmf3LightDirection);
	fScale += Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x + 1, z + 1),
		xmf3LightDirection);
	fScale += Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x, z + 1),
		xmf3LightDirection);
	fScale = (fScale / 4.0f) + 0.05f;
	if (fScale > 1.0f) fScale = 1.0f;
	if (fScale < 0.25f) fScale = 0.25f;
	//fScale�� ���� ����(���)�� �ݻ�Ǵ� �����̴�.
	XMFLOAT4 xmf4Color = Vector4::Multiply(fScale, xmf4IncidentLightColor);
	return(xmf4Color);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// Sphere Mesh Diffused //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

CSphereMeshDiffused::CSphereMeshDiffused(ID3D12Device *pd3dDevice,
	ID3D12GraphicsCommandList *pd3dCommandList, float fRadius, int nSlices, int nStacks) :
	CMesh(pd3dDevice, pd3dCommandList)
{
	/*nSlices�� ���� xz-��鿡 �����ϰ� �� ����� ���� ���� ��Ÿ����. nStacks�� ������� �� �������� �ڸ� ����
	���� ��Ÿ����.*/
	m_nStride = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	/*������� ǥ�鿡 �ִ� �簢��(��)�� ������ {nSlices * (nStacks-2)}�̴�. �簢������ ������� ǥ���� ���� ���ӵ�
	�� ó���� ������ �簢���� ����Ǿ� �����Ƿ� ù ��° ������� �����ϰ� �簢�� �ϳ��� ǥ���ϱ� ���Ͽ� �ϳ��� ��
	���� �ʿ��ϴ�. ù ��° ������� ���Ʒ��� �� ���� ������ �ʿ��ϹǷ� ������� ǥ���� �簢������ ǥ���ϱ� ���Ͽ�
	�ʿ��� ������ ������ {(nSlices * (nStacks?1))}�̴�. �׷��� ���� ���� �Ʒ�(���� ������� ������ �� ���ذ� �ϱ�)
	�� �ڸ��� ������� �ƴ϶� ������ �ǹǷ� �� ������ ǥ���ϱ� ���Ͽ� 2���� ������ �� �ʿ��ϴ�. �׷��Ƿ� ������ ��
	ü ������ {(nSlices * (nStacks?1)) + 2}�̴�.*/
	m_nVertices = 2 + (nSlices * (nStacks - 1));
	m_pVertices = new CDiffusedVertex[m_nVertices];

	//180���� nStacks ��ŭ �����Ѵ�.
	float fDeltaPhi = float(XM_PI / nStacks);

	//360���� nSlices ��ŭ �����Ѵ�.
	float fDeltaTheta = float((2.0f * XM_PI) / nSlices);

	int k = 0;
	//���� ��(�ϱ�)�� ��Ÿ���� �����̴�.
	m_pVertices[k++] = CDiffusedVertex(0.0f, +fRadius, 0.0f, RANDOM_COLOR);

	float theta_i, phi_j;
	//����� ǥ���� �����̴�.
	for (int j = 1; j < nStacks; j++)
	{
		phi_j = fDeltaPhi * j;
		for (int i = 0; i < nSlices; i++)
		{
			theta_i = fDeltaTheta * i;
			m_pVertices[k++] = CDiffusedVertex(fRadius*sinf(phi_j)*cosf(theta_i),
				fRadius*cosf(phi_j), fRadius*sinf(phi_j)*sinf(theta_i), RANDOM_COLOR);
		}
	}
	//���� �Ʒ�(����)�� ��Ÿ���� �����̴�.
	m_pVertices[k] = CDiffusedVertex(0.0f, -fRadius, 0.0f, RANDOM_COLOR);

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pVertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	/*������� ǥ�鿡 �����ϴ� �簢���� ������ {nSlices * (nStacks-2)}�̰� �簢���� 2���� �ﰢ������ �����ǹǷ�
	�ﰢ�� ����Ʈ�� �� �ʿ��� �ε����� ������ {nSlices * (nStacks-2) * 2 * 3}�̴�. �׸��� ���� ���Ʒ� ������ ǥ��
	�� �����ϴ� �ﰢ���� ������ nSlices���̹Ƿ� ���� ���Ʒ� ������ ǥ���ϱ� ���� �ε����� ������ {(nSlices * 3) *
	2}�̴�. �׷��Ƿ� ���� ǥ���� �ﰢ�� ����Ʈ�� ǥ���ϱ� ���Ͽ� �ʿ��� �ε����� ������ {(nSlices * 3) * 2 +
	(nSlices * (nStacks - 2) * 3 * 2)}�̴�*/
	m_nIndices = (nSlices * 3) * 2 + (nSlices * (nStacks - 2) * 3 * 2);
	m_pnIndices = new UINT[m_nIndices];

	k = 0;
	//���� ���� ������ ǥ���� ǥ���ϴ� �ﰢ������ �ε����̴�.
	for (int i = 0; i < nSlices; i++)
	{
		m_pnIndices[k++] = 0;
		m_pnIndices[k++] = 1 + ((i + 1) % nSlices);
		m_pnIndices[k++] = 1 + i;
	}
	//���� ������� ǥ���� ǥ���ϴ� �ﰢ������ �ε����̴�.
	for (int j = 0; j < nStacks - 2; j++)
	{
		for (int i = 0; i < nSlices; i++)
		{
			//�簢���� ù ��° �ﰢ���� �ε����̴�.
			m_pnIndices[k++] = 1 + (i + (j * nSlices));
			m_pnIndices[k++] = 1 + (((i + 1) % nSlices) + (j * nSlices));
			m_pnIndices[k++] = 1 + (i + ((j + 1) * nSlices));
			//�簢���� �� ��° �ﰢ���� �ε����̴�.
			m_pnIndices[k++] = 1 + (i + ((j + 1) * nSlices));
			m_pnIndices[k++] = 1 + (((i + 1) % nSlices) + (j * nSlices));
			m_pnIndices[k++] = 1 + (((i + 1) % nSlices) + ((j + 1) * nSlices));
		}
	}

	//���� �Ʒ��� ������ ǥ���� ǥ���ϴ� �ﰢ������ �ε����̴�.
	for (int i = 0; i < nSlices; i++)
	{
		m_pnIndices[k++] = (m_nVertices - 1);
		m_pnIndices[k++] = ((m_nVertices - 1) - nSlices) + i;
		m_pnIndices[k++] = ((m_nVertices - 1) - nSlices) + ((i + 1) % nSlices);
	}

	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pnIndices,
		sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER,
		&m_pd3dIndexUploadBuffer);

	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;

	m_xmBoundingBox = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fRadius,
		fRadius, fRadius), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}


CSphereMeshDiffused::~CSphereMeshDiffused()
{
}




CRoofMeshDiffussed::CRoofMeshDiffussed(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList,
	float fWidth, float fHeight, float fDepth, XMFLOAT4 xmf4Color) : CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 8;
	m_nStride = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	m_pVertices = new CDiffusedVertex[m_nVertices];
	m_pVertices[0] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), xmf4Color);
	m_pVertices[1] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), xmf4Color);
	m_pVertices[2] = CDiffusedVertex(XMFLOAT3(fx + 60, 0, -fz - 60), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[3] = CDiffusedVertex(XMFLOAT3(-fx - 60, 0, -fz - 60), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[4] = CDiffusedVertex(XMFLOAT3(-fx - 60, 0, +fz + 60), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[5] = CDiffusedVertex(XMFLOAT3(fx + 60, 0, +fz + 60), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[6] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), xmf4Color);
	m_pVertices[7] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), xmf4Color);

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pVertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	/*�ε��� ���۴� ������ü�� 6���� ��(�簢��)�� ���� ���� ������ ���´�. �ﰢ�� ����Ʈ�� ������ü�� ǥ���� ����
	�Ƿ� �� ���� 2���� �ﰢ���� ������ �� �ﰢ���� 3���� ������ �ʿ��ϴ�. ��, �ε��� ���۴� ��ü 36(=6*2*3)���� ��
	������ ������ �Ѵ�.*/
	m_nIndices = 36;

	m_pnIndices = new UINT[m_nIndices];
	//�� �ո�(Front) �簢���� ���� �ﰢ��
	m_pnIndices[0] = 0; m_pnIndices[1] = 1; m_pnIndices[2] = 3;
	//�� �ո�(Front) �簢���� �Ʒ��� �ﰢ��
	m_pnIndices[3] = 1; m_pnIndices[4] = 2; m_pnIndices[5] = 3;
	//�� ����(Top) �簢���� ���� �ﰢ��
	m_pnIndices[6] = 0; m_pnIndices[7] = 7; m_pnIndices[8] = 6;
	//�� ����(Top) �簢���� �Ʒ��� �ﰢ��
	m_pnIndices[9] = 6; m_pnIndices[10] = 1; m_pnIndices[11] = 0;
	//�� �޸�(Back) �簢���� ���� �ﰢ��
	m_pnIndices[12] = 6; m_pnIndices[13] = 7; m_pnIndices[14] = 5;
	//�� �޸�(Back) �簢���� �Ʒ��� �ﰢ��
	m_pnIndices[15] = 7; m_pnIndices[16] = 4; m_pnIndices[17] = 5;
	//�� �Ʒ���(Bottom) �簢���� ���� �ﰢ��
	m_pnIndices[18] = 5; m_pnIndices[19] = 4; m_pnIndices[20] = 3;
	//�� �Ʒ���(Bottom) �簢���� �Ʒ��� �ﰢ��
	m_pnIndices[21] = 3; m_pnIndices[22] = 2; m_pnIndices[23] = 5;
	//�� ����(Left) �簢���� ���� �ﰢ��
	m_pnIndices[24] = 4; m_pnIndices[25] = 7; m_pnIndices[26] = 0;
	//�� ����(Left) �簢���� �Ʒ��� �ﰢ��
	m_pnIndices[27] = 0; m_pnIndices[28] = 3; m_pnIndices[29] = 4;
	//�� ����(Right) �簢���� ���� �ﰢ��
	m_pnIndices[30] = 1; m_pnIndices[31] = 6; m_pnIndices[32] = 2;
	//�� ����(Right) �簢���� �Ʒ��� �ﰢ��
	m_pnIndices[33] = 2; m_pnIndices[34] = 6; m_pnIndices[35] = 5;

	//�ε��� ���۸� �����Ѵ�.
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pnIndices,
		sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER,
		&m_pd3dIndexUploadBuffer);

	//�ε��� ���� �並 �����Ѵ�.
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;

	//�޽��� �ٿ�� �ڽ�(�� ��ǥ��)�� �����Ѵ�.
	m_xmBoundingBox = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fx+60, fy,
		fz+60), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}


CRoofMeshDiffussed::~CRoofMeshDiffussed() 
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CMeshIlluminated::CMeshIlluminated(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) : CMesh(pd3dDevice, pd3dCommandList)
{
}

CMeshIlluminated::~CMeshIlluminated()
{
}

void CMeshIlluminated::CalculateTriangleListVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, int nVertices)
{
	int nPrimitives = nVertices / 3;
	UINT nIndex0, nIndex1, nIndex2;
	for (int i = 0; i < nPrimitives; i++)
	{
		nIndex0 = i * 3 + 0;
		nIndex1 = i * 3 + 1;
		nIndex2 = i * 3 + 2;
		XMFLOAT3 xmf3Edge01 = Vector3::Subtract(pxmf3Positions[nIndex1], pxmf3Positions[nIndex0]);
		XMFLOAT3 xmf3Edge02 = Vector3::Subtract(pxmf3Positions[nIndex2], pxmf3Positions[nIndex0]);
		pxmf3Normals[nIndex0] = pxmf3Normals[nIndex1] = pxmf3Normals[nIndex2] = Vector3::CrossProduct(xmf3Edge01, xmf3Edge02, true);
	}
}

void CMeshIlluminated::CalculateTriangleListVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, UINT nVertices, UINT *pnIndices, UINT nIndices)
{
	UINT nPrimitives = (pnIndices) ? (nIndices / 3) : (nVertices / 3);
	XMFLOAT3 xmf3SumOfNormal, xmf3Edge01, xmf3Edge02, xmf3Normal;
	UINT nIndex0, nIndex1, nIndex2;
	for (UINT j = 0; j < nVertices; j++)
	{
		xmf3SumOfNormal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		for (UINT i = 0; i < nPrimitives; i++)
		{
			nIndex0 = pnIndices[i * 3 + 0];
			nIndex1 = pnIndices[i * 3 + 1];
			nIndex2 = pnIndices[i * 3 + 2];
			if (pnIndices && ((nIndex0 == j) || (nIndex1 == j) || (nIndex2 == j)))
			{
				xmf3Edge01 = Vector3::Subtract(pxmf3Positions[nIndex1], pxmf3Positions[nIndex0]);
				xmf3Edge02 = Vector3::Subtract(pxmf3Positions[nIndex2], pxmf3Positions[nIndex0]);
				xmf3Normal = Vector3::CrossProduct(xmf3Edge01, xmf3Edge02, false);
				xmf3SumOfNormal = Vector3::Add(xmf3SumOfNormal, xmf3Normal);
			}
		}
		pxmf3Normals[j] = Vector3::Normalize(xmf3SumOfNormal);
	}
}

void CMeshIlluminated::CalculateTriangleStripVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, UINT nVertices, UINT *pnIndices, UINT nIndices)
{
	UINT nPrimitives = (pnIndices) ? (nIndices - 2) : (nVertices - 2);
	XMFLOAT3 xmf3SumOfNormal(0.0f, 0.0f, 0.0f);
	UINT nIndex0, nIndex1, nIndex2;
	for (UINT j = 0; j < nVertices; j++)
	{
		xmf3SumOfNormal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		for (UINT i = 0; i < nPrimitives; i++)
		{
			nIndex0 = ((i % 2) == 0) ? (i + 0) : (i + 1);
			if (pnIndices) nIndex0 = pnIndices[nIndex0];
			nIndex1 = ((i % 2) == 0) ? (i + 1) : (i + 0);
			if (pnIndices) nIndex1 = pnIndices[nIndex1];
			nIndex2 = (pnIndices) ? pnIndices[i + 2] : (i + 2);
			if ((nIndex0 == j) || (nIndex1 == j) || (nIndex2 == j))
			{
				XMFLOAT3 xmf3Edge01 = Vector3::Subtract(pxmf3Positions[nIndex1], pxmf3Positions[nIndex0]);
				XMFLOAT3 xmf3Edge02 = Vector3::Subtract(pxmf3Positions[nIndex2], pxmf3Positions[nIndex0]);
				XMFLOAT3 xmf3Normal = Vector3::CrossProduct(xmf3Edge01, xmf3Edge02, true);
				xmf3SumOfNormal = Vector3::Add(xmf3SumOfNormal, xmf3Normal);
			}
		}
		pxmf3Normals[j] = Vector3::Normalize(xmf3SumOfNormal);
	}
}

void CMeshIlluminated::CalculateVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, int nVertices, UINT *pnIndices, int nIndices)
{
	switch (m_d3dPrimitiveTopology)
	{
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		if (pnIndices)
			CalculateTriangleListVertexNormals(pxmf3Normals, pxmf3Positions, nVertices, pnIndices, nIndices);
		else
			CalculateTriangleListVertexNormals(pxmf3Normals, pxmf3Positions, nVertices);
		break;
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		CalculateTriangleStripVertexNormals(pxmf3Normals, pxmf3Positions, nVertices, pnIndices, nIndices);
		break;
	default:
		break;
	}
}


XMFLOAT3 CMeshIlluminated::CheckRayIntersection(XMFLOAT3& xmf3RayOrigin, XMFLOAT3& xmf3RayDirection)
{
	//�ϳ��� �޽����� ������ ���� ���� �ﰢ���� ������ �� �ִ�. �����ϴ� �ﰢ���� �� ���� ����� �ﰢ���� ã�´�.
	int nIntersections = 0;
	BYTE *pbPositions = (BYTE *)m_pVertices;

	int nOffset = (m_d3dPrimitiveTopology == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? 3 : 1;
	/*�޽��� ������Ƽ��(�ﰢ��)���� �����̴�. �ﰢ�� ����Ʈ�� ��� (������ ���� / 3) �Ǵ� (�ε����� ���� / 3), �ﰢ
	�� ��Ʈ���� ��� (������ ���� - 2) �Ǵ� (�ε����� ���� ? 2)�̴�.*/
	int nPrimitives = (m_d3dPrimitiveTopology == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ?
		(m_nVertices / 3) : (m_nVertices - 2);
	if (m_nIndices > 0) nPrimitives = (m_d3dPrimitiveTopology ==
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nIndices / 3) : (m_nIndices - 2);

	//������ �� ��ǥ��� ǥ���ȴ�.
	XMVECTOR xmRayOrigin = XMLoadFloat3(&xmf3RayOrigin);
	XMVECTOR xmRayDirection = XMLoadFloat3(&xmf3RayDirection);
	float fNearHitDistance = FLT_MAX;

	/*�޽��� ��� ������Ƽ��(�ﰢ��)�鿡 ���Ͽ� ��ŷ �������� �浹�� �˻��Ѵ�. �浹�ϴ� ��� �ﰢ���� ã�� ������
	������(�����δ� ī�޶� ��ǥ���� ����)�� ���� ����� �ﰢ���� ã�´�.*/
	XMVECTOR normalVector;
	for (int i = 0; i < nPrimitives; i++)
	{
		XMVECTOR v0 = XMLoadFloat3((XMFLOAT3 *)(pbPositions + ((m_pnIndices) ?
			(m_pnIndices[(i*nOffset) + 0]) : ((i*nOffset) + 0)) * m_nStride));
		XMVECTOR v1 = XMLoadFloat3((XMFLOAT3 *)(pbPositions + ((m_pnIndices) ?
			(m_pnIndices[(i*nOffset) + 1]) : ((i*nOffset) + 1)) * m_nStride));
		XMVECTOR v2 = XMLoadFloat3((XMFLOAT3 *)(pbPositions + ((m_pnIndices) ?
			(m_pnIndices[(i*nOffset) + 2]) : ((i*nOffset) + 2)) * m_nStride));

		float fHitDistance;
		BOOL bIntersected = TriangleTests::Intersects(xmRayOrigin, xmRayDirection, v0,
			v1, v2, fHitDistance);
		if (bIntersected)
		{
			if (fHitDistance < fNearHitDistance)
			{
				XMVECTOR vector1 = v0 - v1;
				XMVECTOR vector2 = v2 - v1;
				normalVector = XMVector3Cross(vector1, vector2);
				fNearHitDistance = fHitDistance;
			}
			nIntersections++;
		}
	}

	XMFLOAT3 returnVector;
	XMStoreFloat3(&returnVector, -normalVector);

	return(Vector3::Normalize(returnVector));
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
CCubeMeshIlluminated::CCubeMeshIlluminated(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth, float fHeight, float fDepth) : CMeshIlluminated(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 8;
	m_nStride = sizeof(CIlluminatedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	m_nIndices = 36;

	m_pnIndices = new UINT[m_nIndices];
	m_pnIndices[0] = 3; m_pnIndices[1] = 1; m_pnIndices[2] = 0;
	m_pnIndices[3] = 2; m_pnIndices[4] = 1; m_pnIndices[5] = 3;
	m_pnIndices[6] = 0; m_pnIndices[7] = 5; m_pnIndices[8] = 4;
	m_pnIndices[9] = 1; m_pnIndices[10] = 5; m_pnIndices[11] = 0;
	m_pnIndices[12] = 3; m_pnIndices[13] = 4; m_pnIndices[14] = 7;
	m_pnIndices[15] = 0; m_pnIndices[16] = 4; m_pnIndices[17] = 3;
	m_pnIndices[18] = 1; m_pnIndices[19] = 6; m_pnIndices[20] = 5;
	m_pnIndices[21] = 2; m_pnIndices[22] = 6; m_pnIndices[23] = 1;
	m_pnIndices[24] = 2; m_pnIndices[25] = 7; m_pnIndices[26] = 6;
	m_pnIndices[27] = 3; m_pnIndices[28] = 7; m_pnIndices[29] = 2;
	m_pnIndices[30] = 6; m_pnIndices[31] = 4; m_pnIndices[32] = 5;
	m_pnIndices[33] = 7; m_pnIndices[34] = 4; m_pnIndices[35] = 6;

	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pnIndices, 
		sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER,
		&m_pd3dIndexUploadBuffer);

	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;

	XMFLOAT3 pxmf3Positions[8];

	pxmf3Positions[0] = XMFLOAT3(-fx, +fy, -fz);
	pxmf3Positions[1] = XMFLOAT3(+fx, +fy, -fz);
	pxmf3Positions[2] = XMFLOAT3(+fx, +fy, +fz);
	pxmf3Positions[3] = XMFLOAT3(-fx, +fy, +fz);
	pxmf3Positions[4] = XMFLOAT3(-fx, -fy, -fz);
	pxmf3Positions[5] = XMFLOAT3(+fx, -fy, -fz);
	pxmf3Positions[6] = XMFLOAT3(+fx, -fy, +fz);
	pxmf3Positions[7] = XMFLOAT3(-fx, -fy, +fz);

	XMFLOAT3 pxmf3Normals[8];
	for (int i = 0; i < 8; i++) pxmf3Normals[i] = XMFLOAT3(0.0f, 0.0f, 0.0f);

	CalculateVertexNormals(pxmf3Normals, pxmf3Positions, m_nVertices, m_pnIndices, m_nIndices);

	m_pVertices = new CIlluminatedVertex[m_nVertices];
	for (int i = 0; i < 8; i++) m_pVertices[i] = CIlluminatedVertex(pxmf3Positions[i], pxmf3Normals[i]);

	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_pVertices,
		m_nStride * m_nVertices, 
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		&m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	//�޽��� �ٿ�� �ڽ�(�� ��ǥ��)�� �����Ѵ�.
	m_xmBoundingBox = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fx, fy,
		fz), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

CCubeMeshIlluminated::~CCubeMeshIlluminated()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
#define _WITH_INDEX_BUFFER

CSphereMeshIlluminated::CSphereMeshIlluminated(ID3D12Device *pd3dDevice,
	ID3D12GraphicsCommandList *pd3dCommandList, float fRadius, UINT nSlices, UINT nStacks)
	: CMeshIlluminated(pd3dDevice, pd3dCommandList)
{
	m_nStride = sizeof(CIlluminatedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fDeltaPhi = float(XM_PI / nStacks);
	float fDeltaTheta = float((2.0f * XM_PI) / nSlices);
	int k = 0;
#ifdef _WITH_INDEX_BUFFER
	m_nVertices = 2 + (nSlices * (nStacks - 1));
	XMFLOAT3 *pxmf3Positions = new XMFLOAT3[m_nVertices];
	XMFLOAT3 *pxmf3Normals = new XMFLOAT3[m_nVertices];

	pxmf3Positions[k] = XMFLOAT3(0.0f, +fRadius, 0.0f);
	pxmf3Normals[k] = Vector3::Normalize(pxmf3Positions[k]); k++;
	float theta_i, phi_j;
	for (UINT j = 1; j < nStacks; j++)
	{
		phi_j = fDeltaPhi * j;
		for (UINT i = 0; i < nSlices; i++)
		{
			theta_i = fDeltaTheta * i;
			pxmf3Positions[k] = XMFLOAT3(fRadius*sinf(phi_j)*cosf(theta_i), fRadius*cosf(phi_j),
				fRadius*sinf(phi_j)*sinf(theta_i));
			pxmf3Normals[k] = Vector3::Normalize(pxmf3Positions[k]); k++;
		}
	}
	pxmf3Positions[k] = XMFLOAT3(0.0f, -fRadius, 0.0f);
	pxmf3Normals[k] = Vector3::Normalize(pxmf3Positions[k]); k++;

	m_pVertices = new CIlluminatedVertex[m_nVertices];
	for (UINT i = 0; i < m_nVertices; i++) m_pVertices[i] = CIlluminatedVertex(pxmf3Positions[i], pxmf3Normals[i]);
	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_pVertices,
		m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		&m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	if (pxmf3Positions) delete[] pxmf3Positions;
	if (pxmf3Normals) delete[] pxmf3Normals;

	k = 0;
	m_nIndices = (nSlices * 3) * 2 + (nSlices * (nStacks - 2) * 3 * 2);
	m_pnIndices = new UINT[m_nIndices];
	for (UINT i = 0; i < nSlices; i++)
	{
		m_pnIndices[k++] = 0;
		m_pnIndices[k++] = 1 + ((i + 1) % nSlices);
		m_pnIndices[k++] = 1 + i;
	}
	for (UINT j = 0; j < nStacks - 2; j++)
	{
		for (UINT i = 0; i < nSlices; i++)
		{
			m_pnIndices[k++] = 1 + (i + (j * nSlices));
			m_pnIndices[k++] = 1 + (((i + 1) % nSlices) + (j * nSlices));
			m_pnIndices[k++] = 1 + (i + ((j + 1) * nSlices));
			m_pnIndices[k++] = 1 + (i + ((j + 1) * nSlices));
			m_pnIndices[k++] = 1 + (((i + 1) % nSlices) + (j * nSlices));
			m_pnIndices[k++] = 1 + (((i + 1) % nSlices) + ((j + 1) * nSlices));
		}
	}
	for (UINT i = 0; i < nSlices; i++)
	{
		m_pnIndices[k++] = (m_nVertices - 1);
		m_pnIndices[k++] = ((m_nVertices - 1) - nSlices) + i;
		m_pnIndices[k++] = ((m_nVertices - 1) - nSlices) + ((i + 1) % nSlices);
	}

	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pnIndices,
		sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER,
		&m_pd3dIndexUploadBuffer);

	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;

#else
	m_nVertices = (nSlices * 3) * 2 + (nSlices * (nStacks - 2) * 3 * 2);
	pxmf3Positions = new XMFLOAT3[m_nVertices];
	XMFLOAT3 *pxmf3Normals = new XMFLOAT3[m_nVertices];

	float theta_i, theta_ii, phi_j = 0.0f, phi_jj = fDeltaPhi;
	for (int i = 0; i < nSlices; i++)
	{
		theta_i = fDeltaTheta * i;
		theta_ii = fDeltaTheta * (i + 1);
		pxmf3Positions[k] = XMFLOAT3(0.0f, +fRadius, 0.0f);
		D3DXVec3Normalize(&pxmf3Normals[k], &pxmf3Positions[k]); k++;
		pxmf3Positions[k] = XMFLOAT3(fRadius*cosf(theta_i)*sinf(phi_jj), fRadius*cosf(phi_jj), fRadius*sinf(theta_i)*sinf(phi_jj));
		D3DXVec3Normalize(&pxmf3Normals[k], &pxmf3Positions[k]); k++;
		pxmf3Positions[k] = XMFLOAT3(fRadius*cosf(theta_ii)*sinf(phi_jj), fRadius*cosf(phi_jj), fRadius*sinf(theta_ii)*sinf(phi_jj));
		D3DXVec3Normalize(&pxmf3Normals[k], &pxmf3Positions[k]); k++;
	}

	for (int j = 1; j < nStacks - 1; j++)
	{
		phi_j = fDeltaPhi * j;
		phi_jj = fDeltaPhi * (j + 1);
		for (int i = 0; i < nSlices; i++)
		{
			theta_i = fDeltaTheta * i;
			theta_ii = fDeltaTheta * (i + 1);
			pxmf3Positions[k] = XMFLOAT3(fRadius*cosf(theta_i)*sinf(phi_j), fRadius*cosf(phi_j), fRadius*sinf(theta_i)*sinf(phi_j));
			D3DXVec3Normalize(&pxmf3Normals[k], &pxmf3Positions[k]); k++;
			pxmf3Positions[k] = XMFLOAT3(fRadius*cosf(theta_i)*sinf(phi_jj), fRadius*cosf(phi_jj), fRadius*sinf(theta_i)*sinf(phi_jj));
			D3DXVec3Normalize(&pxmf3Normals[k], &pxmf3Positions[k]); k++;
			pxmf3Positions[k] = XMFLOAT3(fRadius*cosf(theta_ii)*sinf(phi_j), fRadius*cosf(phi_j), fRadius*sinf(theta_ii)*sinf(phi_j));
			D3DXVec3Normalize(&pxmf3Normals[k], &pxmf3Positions[k]); k++;
			pxmf3Positions[k] = XMFLOAT3(fRadius*cosf(theta_i)*sinf(phi_jj), fRadius*cosf(phi_jj), fRadius*sinf(theta_i)*sinf(phi_jj));
			D3DXVec3Normalize(&pxmf3Normals[k], &pxmf3Positions[k]); k++;
			pxmf3Positions[k] = XMFLOAT3(fRadius*cosf(theta_ii)*sinf(phi_jj), fRadius*cosf(phi_jj), fRadius*sinf(theta_ii)*sinf(phi_jj));
			D3DXVec3Normalize(&pxmf3Normals[k], &pxmf3Positions[k]); k++;
			pxmf3Positions[k] = XMFLOAT3(fRadius*cosf(theta_ii)*sinf(phi_j), fRadius*cosf(phi_j), fRadius*sinf(theta_ii)*sinf(phi_j));
			D3DXVec3Normalize(&pxmf3Normals[k], &pxmf3Positions[k]); k++;
		}
	}
	phi_j = fDeltaPhi * (nStacks - 1);
	for (int i = 0; i < nSlices; i++)
	{
		theta_i = fDeltaTheta * i;
		theta_ii = fDeltaTheta * (i + 1);
		pxmf3Positions[k] = XMFLOAT3(0.0f, -fRadius, 0.0f);
		D3DXVec3Normalize(&pxmf3Normals[k], &pxmf3Positions[k]); k++;
		pxmf3Positions[k] = XMFLOAT3(fRadius*cosf(theta_ii)*sinf(phi_j), fRadius*cosf(phi_j), fRadius*sinf(theta_ii)*sinf(phi_j));
		D3DXVec3Normalize(&pxmf3Normals[k], &pxmf3Positions[k]); k++;
		pxmf3Positions[k] = XMFLOAT3(fRadius*cosf(theta_i)*sinf(phi_j), fRadius*cosf(phi_j), fRadius*sinf(theta_i)*sinf(phi_j));
		D3DXVec3Normalize(&pxmf3Normals[k], &pxmf3Positions[k]); k++;
	}

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(XMFLOAT3) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pxmf3Positions;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);

	d3dBufferData.pSysMem = pxmf3Normals;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dNormalBuffer);

	if (pxmf3Normals) delete[] pxmf3Normals;

	ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dNormalBuffer };
	UINT pnBufferStrides[2] = { sizeof(XMFLOAT3), sizeof(XMFLOAT3) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer(2, pd3dBuffers, pnBufferStrides, pnBufferOffsets);
#endif

	m_xmBoundingBox = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fRadius,
		fRadius, fRadius), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

CSphereMeshIlluminated::~CSphereMeshIlluminated()
{
}


CRoofMeshIlluminated::CRoofMeshIlluminated(ID3D12Device *pd3dDevice,
	ID3D12GraphicsCommandList *pd3dCommandList,
	float fWidth, float fHeight, float fDepth)
	: CMeshIlluminated(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 8;
	m_nStride = sizeof(CIlluminatedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_nIndices = 36;
	m_pnIndices = new UINT[m_nIndices];
	//�� �ո�(Front) �簢���� ���� �ﰢ��
	m_pnIndices[0] = 0; m_pnIndices[1] = 1; m_pnIndices[2] = 3;
	//�� �ո�(Front) �簢���� �Ʒ��� �ﰢ��
	m_pnIndices[3] = 1; m_pnIndices[4] = 2; m_pnIndices[5] = 3;
	//�� ����(Top) �簢���� ���� �ﰢ��
	m_pnIndices[6] = 0; m_pnIndices[7] = 7; m_pnIndices[8] = 6;
	//�� ����(Top) �簢���� �Ʒ��� �ﰢ��
	m_pnIndices[9] = 6; m_pnIndices[10] = 1; m_pnIndices[11] = 0;
	//�� �޸�(Back) �簢���� ���� �ﰢ��
	m_pnIndices[12] = 6; m_pnIndices[13] = 7; m_pnIndices[14] = 5;
	//�� �޸�(Back) �簢���� �Ʒ��� �ﰢ��
	m_pnIndices[15] = 7; m_pnIndices[16] = 4; m_pnIndices[17] = 5;
	//�� �Ʒ���(Bottom) �簢���� ���� �ﰢ��
	m_pnIndices[18] = 5; m_pnIndices[19] = 4; m_pnIndices[20] = 3;
	//�� �Ʒ���(Bottom) �簢���� �Ʒ��� �ﰢ��
	m_pnIndices[21] = 3; m_pnIndices[22] = 2; m_pnIndices[23] = 5;
	//�� ����(Left) �簢���� ���� �ﰢ��
	m_pnIndices[24] = 4; m_pnIndices[25] = 7; m_pnIndices[26] = 0;
	//�� ����(Left) �簢���� �Ʒ��� �ﰢ��
	m_pnIndices[27] = 0; m_pnIndices[28] = 3; m_pnIndices[29] = 4;
	//�� ����(Right) �簢���� ���� �ﰢ��
	m_pnIndices[30] = 1; m_pnIndices[31] = 6; m_pnIndices[32] = 2;
	//�� ����(Right) �簢���� �Ʒ��� �ﰢ��
	m_pnIndices[33] = 2; m_pnIndices[34] = 6; m_pnIndices[35] = 5;

	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pnIndices,
		sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER,
		&m_pd3dIndexUploadBuffer);

	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;

	XMFLOAT3 pxmf3Positions[8];

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	pxmf3Positions[0] = XMFLOAT3(-fx, +fy, -fz);
	pxmf3Positions[1] = XMFLOAT3(+fx, +fy, -fz);
	pxmf3Positions[2] = XMFLOAT3(fx + 60, 0, -fz - 60);
	pxmf3Positions[3] = XMFLOAT3(-fx - 60, 0, -fz - 60);
	pxmf3Positions[4] = XMFLOAT3(-fx - 60, 0, +fz + 60);
	pxmf3Positions[5] = XMFLOAT3(fx + 60, 0, +fz + 60);
	pxmf3Positions[6] = XMFLOAT3(+fx, fy, +fz);
	pxmf3Positions[7] = XMFLOAT3(-fx, fy, +fz);

	XMFLOAT3 pxmf3Normals[8];
	for (int i = 0; i < 8; i++) pxmf3Normals[i] = XMFLOAT3(0.0f, 0.0f, 0.0f);

	CalculateVertexNormals(pxmf3Normals, pxmf3Positions, m_nVertices, m_pnIndices, m_nIndices);

	m_pVertices = new CIlluminatedVertex[m_nVertices];
	for (int i = 0; i < 8; i++) m_pVertices[i] = CIlluminatedVertex(pxmf3Positions[i], pxmf3Normals[i]);

	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList,
		m_pVertices, m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, 
		&m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	//�޽��� �ٿ�� �ڽ�(�� ��ǥ��)�� �����Ѵ�.
	m_xmBoundingBox = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fx + 60, fy,
		fz + 60), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

CRoofMeshIlluminated::~CRoofMeshIlluminated()
{
}