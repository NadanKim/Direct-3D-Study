//-----------------------------------------------------------------------------
// File: CGameFramework.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "GameFramework.h"

CGameFramework::CGameFramework()
{
	m_hInstance = NULL;
	m_hWnd = NULL;     

    m_hDCFrameBuffer = NULL;   
    m_hBitmapFrameBuffer = NULL;   

	m_bActive = true;    

	m_nObjects = 0;
	m_nWalls = 0;
	m_fMakeDistance = 0.0f;

// 커서 위치를 화면 중앙으로 변경
	m_ptOldCursorPos.x = CLIENT_WIDTH / 2;
	m_ptOldCursorPos.y = CLIENT_HEIGHT / 2;

}

CGameFramework::~CGameFramework()
{
}

bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
    srand(timeGetTime());

	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	m_pPlayer = new CPlayer();
	m_pPlayer->m_pCamera->SetViewport(rcClient.left, rcClient.top, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);

	BuildFrameBuffer(); 

	BuildObjects(); 

	SetupGameState();

	return(true);
}

void CGameFramework::BuildFrameBuffer()
{
    HDC hDC = ::GetDC(m_hWnd);

	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

    m_hDCFrameBuffer = ::CreateCompatibleDC(hDC);
	m_hBitmapFrameBuffer = ::CreateCompatibleBitmap(hDC, (rcClient.right - rcClient.left) + 1, (rcClient.bottom - rcClient.top) + 1);
    ::SelectObject(m_hDCFrameBuffer, m_hBitmapFrameBuffer);

	::ReleaseDC(m_hWnd, hDC);
    ::SetBkMode(m_hDCFrameBuffer, TRANSPARENT);
}

void CGameFramework::ClearFrameBuffer(DWORD dwColor)
{
    HBRUSH hBrush = ::CreateSolidBrush(dwColor);
    HBRUSH hOldBrush = (HBRUSH)::SelectObject(m_hDCFrameBuffer, hBrush);
	::Rectangle(m_hDCFrameBuffer, m_pPlayer->m_pCamera->m_Viewport.m_xStart, m_pPlayer->m_pCamera->m_Viewport.m_yStart, m_pPlayer->m_pCamera->m_Viewport.m_nWidth, m_pPlayer->m_pCamera->m_Viewport.m_nHeight);
    ::SelectObject(m_hDCFrameBuffer, hOldBrush);
    ::DeleteObject(hBrush);
}

void CGameFramework::PresentFrameBuffer()
{    
    HDC hDC = ::GetDC(m_hWnd);
    ::BitBlt(hDC, m_pPlayer->m_pCamera->m_Viewport.m_xStart, m_pPlayer->m_pCamera->m_Viewport.m_yStart, m_pPlayer->m_pCamera->m_Viewport.m_nWidth, m_pPlayer->m_pCamera->m_Viewport.m_nHeight, m_hDCFrameBuffer, m_pPlayer->m_pCamera->m_Viewport.m_xStart, m_pPlayer->m_pCamera->m_Viewport.m_yStart, SRCCOPY);
    ::ReleaseDC(m_hWnd, hDC);
}

void CGameFramework::SetupGameState()
{
}

void CGameFramework::SetupRenderStates()
{
}

void CGameFramework::BuildObjects()
{
	CAirplaneMesh *pAirplaneMesh = new CAirplaneMesh(6.0f, 6.0f, 1.0f);
	m_pPlayer = new CPlayer();
	m_pPlayer->SetPosition(0.0f, 0.0f, 0.0f);
	m_pPlayer->SetMesh(pAirplaneMesh);
	m_pPlayer->SetColor(RGB(0, 0, 255));
	m_pPlayer->SetCameraOffset(XMFLOAT3(0.0f, 5.0f, -15.0f));
	m_pPlayer->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.3f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	float fWallLength = 8.0f;
	CWallMesh *pWallCubeMesh = new CWallMesh(40.0f, 40.0f, fWallLength);

	m_nWalls = 50;
	m_pWalls = new CGameObject[m_nWalls];

	m_fMakeDistance = ((m_nWalls * fWallLength) / 2 + (m_nWalls % 2) * fWallLength / 2);
	for (int i = 0; i < m_nWalls; ++i) {
		m_pWalls[i].SetPosition(0.0f, 0.0f, -m_fMakeDistance + fWallLength * i);
		m_pWalls[i].SetMesh(pWallCubeMesh);
		m_pWalls[i].SetColor(RGB(0, 0, 0));
		m_pWalls[i].SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(20.0f, 20.0f, 20.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	}
	m_pxmf4WallPlanes[0] = XMFLOAT4(+1.0f, 0.0f, 0.0f, 20.0f);
	m_pxmf4WallPlanes[1] = XMFLOAT4(-1.0f, 0.0f, 0.0f, 20.0f);
	m_pxmf4WallPlanes[2] = XMFLOAT4(0.0f, +1.0f, 0.0f, 20.0f);
	m_pxmf4WallPlanes[3] = XMFLOAT4(0.0f, -1.0f, 0.0f, 20.0f);

	CCubeMesh *pObjectCubeMesh = new CCubeMesh(4.0f, 4.0f, 4.0f);

	m_nObjects = 20;
	m_pObjects = new CGameObject[m_nObjects];
	
	uniform_real_distribution<float>posDst(-17.0, 17.0);
	uniform_real_distribution<float>zDst(-m_fMakeDistance, m_fMakeDistance);
	default_random_engine dre((unsigned)time(NULL));
	//최초 오브젝트 설정
	for (int i = 0; i < m_nObjects; ++i) {
		m_pObjects[i].SetMesh(pObjectCubeMesh);
		m_pObjects[i].ResetObjects(0);
		m_pObjects[i].SetPosition(posDst(dre), posDst(dre), zDst(dre));
		m_pObjects[i].SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(2.0f, 2.0f, 2.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	}
}

void CGameFramework::ReleaseObjects()
{
	if (m_pObjects) delete [] m_pObjects;
	m_pObjects = NULL;
	if (m_pWalls) delete[] m_pWalls;
	m_pWalls = NULL;
}

void CGameFramework::OnDestroy()
{
	ReleaseObjects();

	if (m_hBitmapFrameBuffer) ::DeleteObject(m_hBitmapFrameBuffer);
    if (m_hDCFrameBuffer) ::DeleteDC(m_hDCFrameBuffer);

    if (m_hWnd) DestroyWindow(m_hWnd);
}

void CGameFramework::ProcessInput()
{
	//플레이어가 살아있는 동안에만 진행
	if (m_pPlayer->m_bRender) {
		static UCHAR pKeyBuffer[256];

		DWORD dwDirection = 0;
		if (GetKeyboardState(pKeyBuffer))
		{
			if (pKeyBuffer[VK_UP] & 0xF0) dwDirection |= DIR_FORWARD;
			if (pKeyBuffer[VK_DOWN] & 0xF0) dwDirection |= DIR_BACKWARD;
			if (pKeyBuffer[VK_LEFT] & 0xF0) dwDirection |= DIR_LEFT;
			if (pKeyBuffer[VK_RIGHT] & 0xF0) dwDirection |= DIR_RIGHT;
			if (pKeyBuffer[VK_PRIOR] & 0xF0) dwDirection |= DIR_UP;
			if (pKeyBuffer[VK_NEXT] & 0xF0) dwDirection |= DIR_DOWN;

			if (GetKeyState('W') & 0xF0) dwDirection |= DIR_FORWARD;
			if (GetKeyState('S') & 0xF0) dwDirection |= DIR_BACKWARD;
			if (GetKeyState('A') & 0xF0) dwDirection |= DIR_LEFT;
			if (GetKeyState('D') & 0xF0) dwDirection |= DIR_RIGHT;
			if (GetKeyState('Q') & 0xF0) dwDirection |= DIR_DOWN;
			if (GetKeyState('E') & 0xF0) dwDirection |= DIR_UP;

		}
		float cxDelta = 0.0f, cyDelta = 0.0f;
		POINT ptCursorPos;
		// 마우스 이동시 화면이 돌아가도록 설정
		SetCursor(NULL);
		GetCursorPos(&ptCursorPos);
		cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
		cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
		SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
		if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
		{
			if (cxDelta || cyDelta)
			{
				m_pPlayer->Rotate(cyDelta, 0.0f, -cxDelta);
				m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
			}

			float playerSpeed = 0.125;
			if (dwDirection) {
				//부스터
				if (GetKeyState(VK_LSHIFT) & 0xF0)  m_pPlayer->Move(dwDirection, playerSpeed * 2);
				else  m_pPlayer->Move(dwDirection, playerSpeed);
			}
			m_pPlayer->Animate();

			//플레이어와 벽 충돌검사
			ContainmentType containType = m_pWalls[0].m_xmOOBB.Contains(m_pPlayer->m_xmOOBB);
			switch (containType)
			{
			case DISJOINT:
			{
				int nPlaneIndex = -1;
				for (int j = 0; j < 4; j++)
				{
					PlaneIntersectionType intersectType = m_pPlayer->m_xmOOBB.Intersects(XMLoadFloat4(&m_pxmf4WallPlanes[j]));
					if (intersectType == BACK)
					{
						nPlaneIndex = j;
						break;
					}
				}
				if (nPlaneIndex != -1)
				{
					float length;
					XMFLOAT3 xmfNormal;
					XMVECTOR xmvNormal = XMVectorSet(m_pxmf4WallPlanes[nPlaneIndex].x, m_pxmf4WallPlanes[nPlaneIndex].y, m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
					XMStoreFloat3(&xmfNormal, xmvNormal);
					//벽의 노멀에 속도를 투영하여 반사시킬 속도를 얻어옴
					XMVECTOR xmvProj = XMVector3Dot(XMLoadFloat3(&m_pPlayer->m_xmf3Velocity), xmvNormal)*xmvNormal;
					XMStoreFloat(&length, XMVector3Length(xmvProj));
					m_pPlayer->Move(xmfNormal, length);
				}
				break;
			}
			case INTERSECTS:
			{
				int nPlaneIndex = -1;
				for (int j = 0; j < 4; j++)
				{
					PlaneIntersectionType intersectType = m_pPlayer->m_xmOOBB.Intersects(XMLoadFloat4(&m_pxmf4WallPlanes[j]));
					if (intersectType == INTERSECTING)
					{
						nPlaneIndex = j;
						break;
					}
				}
				if (nPlaneIndex != -1)
				{
					float length;
					XMFLOAT3 xmfNormal;
					XMVECTOR xmvNormal = XMVectorSet(m_pxmf4WallPlanes[nPlaneIndex].x, m_pxmf4WallPlanes[nPlaneIndex].y, m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
					XMStoreFloat3(&xmfNormal, xmvNormal);
					//벽의 노멀에 속도를 투영하여 반사시킬 속도를 얻어옴
					XMVECTOR xmvProj = XMVector3Dot(XMLoadFloat3(&m_pPlayer->m_xmf3Velocity), xmvNormal)*xmvNormal;
					XMStoreFloat(&length, XMVector3Length(xmvProj));
					m_pPlayer->Move(xmfNormal, length);
				}
				break;
			}
			case CONTAINS:
				break;
			}

			XMFLOAT3 playerPos = m_pPlayer->GetPosition();

			//벽 무한히 그리기
			for (int i = 0; i < m_nWalls; ++i) {
				if (m_pWalls[i].GetPosition().z < playerPos.z - m_fMakeDistance) {
					m_pWalls[i].SetPosition(0, 0, playerPos.z + m_fMakeDistance);
				}
				else if (m_pWalls[i].GetPosition().z > playerPos.z + m_fMakeDistance) {
					m_pWalls[i].SetPosition(0, 0, playerPos.z - m_fMakeDistance);
				}
			}

			//장애물 무한히 그리기
			for (int i = 0; i < m_nObjects; i++) {
				if (m_pObjects[i].GetPosition().z < playerPos.z - m_fMakeDistance) {
					m_pObjects[i].ResetObjects(playerPos.z + m_fMakeDistance);
				}
				else if (m_pObjects[i].GetPosition().z > playerPos.z + m_fMakeDistance) {
					m_pObjects[i].ResetObjects(playerPos.z - m_fMakeDistance);
				}
			}
		}

		//총알 발사
		if (GetAsyncKeyState(VK_LBUTTON) & 0xFF00) {
			if (m_pPlayer->m_fShootRate < EPSILON) {
				m_pPlayer->m_fShootRate = RELOAD;
				CGameObject bullet;
				bullet.SetColor(RGB(215, 170, 20));
				XMFLOAT3 playerPos;
				XMStoreFloat3(&playerPos, XMVectorAdd(XMLoadFloat3(&m_pPlayer->GetPosition()),
										XMLoadFloat3(&m_pPlayer->m_xmf3Look)));
				bullet.SetPosition(playerPos.x, playerPos.y, playerPos.z);
				bullet.SetMovingDirection(m_pPlayer->m_xmf3Look);
				bullet.SetRotationAxis(m_pPlayer->m_xmf3Look);
				bullet.SetRotationSpeed(3.2f);
				bullet.SetMovingSpeed(0.55f);
				bullet.SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.5f, 0.5f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
				bullet.SetMesh(new CCubeMesh(1.0f, 1.0f, 2.0f));
				m_lBullets.push_back(move(bullet));
			}
		}
		m_pPlayer->Update(0.00516f);
	}
	else {
		if (GetKeyState('R') & 0xF0) {
			m_pPlayer->m_bRender = TRUE;
		}
	}
}

void CGameFramework::AnimateObjects()
{
	for (int i = 0; i < m_nObjects; i++) m_pObjects[i].Animate();
	for (auto& bullet : m_lBullets) {
		bullet.Animate();
	}

	for (auto& particle : m_lParticles) {
		if (particle.m_bRender)
			particle.Update();
	}

	//플레이어와 물체 충돌검사
	if (m_pPlayer->m_bRender) {
		for (int i = 0; i < m_nObjects; i++)
		{
			if (m_pObjects[i].m_bRender == FALSE)
				continue;
			if (m_pObjects[i].m_xmOOBB.Intersects(m_pPlayer->m_xmOOBB))
			{
				m_lParticles.push_back(CParticle(&m_pObjects[i], 35.0f));
				m_lParticles.push_back(CParticle(m_pPlayer, 35.0f));

				m_pObjects[i].m_bRender = FALSE;
				m_pPlayer->m_bRender = FALSE;
			}
		}
	}

	//벽과 물체 충돌 검사
	for (int i = 0; i < m_nObjects; i++)
	{
		ContainmentType containType = m_pWalls[0].m_xmOOBB.Contains(m_pObjects[i].m_xmOOBB);
		switch (containType)
		{
		case DISJOINT:
		{
			int nPlaneIndex = -1;
			for (int j = 0; j < 4; j++)
			{
				PlaneIntersectionType intersectType = m_pObjects[i].m_xmOOBB.Intersects(XMLoadFloat4(&m_pxmf4WallPlanes[j]));
				if (intersectType == BACK)
				{
					nPlaneIndex = j;
					break;
				}
			}
			if (nPlaneIndex != -1)
			{
				XMVECTOR xmvNormal = XMVectorSet(m_pxmf4WallPlanes[nPlaneIndex].x, m_pxmf4WallPlanes[nPlaneIndex].y, m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
				XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(&m_pObjects[i].m_xmf3MovingDirection), xmvNormal);
				XMStoreFloat3(&m_pObjects[i].m_xmf3MovingDirection, xmvReflect);
			}
			break;
		}
		case INTERSECTS:
		{
			int nPlaneIndex = -1;
			for (int j = 0; j < 4; j++)
			{
				PlaneIntersectionType intersectType = m_pObjects[i].m_xmOOBB.Intersects(XMLoadFloat4(&m_pxmf4WallPlanes[j]));
				if (intersectType == INTERSECTING)
				{
					nPlaneIndex = j;
					break;
				}
			}
			if (nPlaneIndex != -1)
			{
				XMVECTOR xmvNormal = XMVectorSet(m_pxmf4WallPlanes[nPlaneIndex].x, m_pxmf4WallPlanes[nPlaneIndex].y, m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
				XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(&m_pObjects[i].m_xmf3MovingDirection), xmvNormal);
				XMStoreFloat3(&m_pObjects[i].m_xmf3MovingDirection, xmvReflect);
			}
			break;
		}
		case CONTAINS:
			break;
		}
	}
	//물체와 물체 충돌검사
	for (int i = 0; i < m_nObjects; i++) m_pObjects[i].m_pCollider = NULL;
	for (int i = 0; i < m_nObjects; i++)
	{
		if (m_pObjects[i].m_bRender == NULL)
			continue;
		for (int j = (i + 1); j < m_nObjects; j++)
		{
			if (m_pObjects[j].m_bRender == NULL)
				continue;
			if (m_pObjects[i].m_xmOOBB.Intersects(m_pObjects[j].m_xmOOBB))
			{
				m_pObjects[i].m_pCollider = &m_pObjects[j];
				m_pObjects[j].m_pCollider = &m_pObjects[i];
			}
		}
	}
	for (int i = 0; i < m_nObjects; i++)
	{
		if (m_pObjects[i].m_pCollider && m_pObjects[i].m_pCollider->m_pCollider)
		{
			XMFLOAT3 xmf3MovingDirection = m_pObjects[i].m_xmf3MovingDirection;
			float fMovingSpeed = m_pObjects[i].m_fMovingSpeed;
			int intKind = m_pObjects[i].m_dkind;
			DWORD dwColor = m_pObjects[i].m_dwColor;

			m_pObjects[i].m_dkind = m_pObjects[i].m_pCollider->m_dkind;
			m_pObjects[i].SetColor(m_pObjects[i].m_pCollider->m_dwColor);
			m_pObjects[i].m_xmf3MovingDirection = m_pObjects[i].m_pCollider->m_xmf3MovingDirection;
			m_pObjects[i].m_fMovingSpeed = m_pObjects[i].m_pCollider->m_fMovingSpeed;

			m_pObjects[i].m_pCollider->m_dkind = intKind;
			m_pObjects[i].m_pCollider->SetColor(dwColor);
			m_pObjects[i].m_pCollider->m_xmf3MovingDirection = xmf3MovingDirection;
			m_pObjects[i].m_pCollider->m_fMovingSpeed = fMovingSpeed;

			m_pObjects[i].m_pCollider->m_pCollider = NULL;
			m_pObjects[i].m_pCollider = NULL;
		}
	}
	//벽과 총알 충돌검사
	for (auto& bullet : m_lBullets)
	{
		if (bullet.m_bRender == FALSE)
			continue;

		ContainmentType containType = m_pWalls[0].m_xmOOBB.Contains(bullet.m_xmOOBB);
		switch (containType)
		{
		case DISJOINT:
		{
			int nPlaneIndex = -1;
			for (int j = 0; j < 4; j++)
			{
				PlaneIntersectionType intersectType = bullet.m_xmOOBB.Intersects(XMLoadFloat4(&m_pxmf4WallPlanes[j]));
				if (intersectType == BACK)
				{
					nPlaneIndex = j;
					break;
				}
			}
			if (nPlaneIndex != -1)
			{
				bullet.m_bRender = FALSE;
			}
			break;
		}
		case INTERSECTS:
		{
			int nPlaneIndex = -1;
			for (int j = 0; j < 4; j++)
			{
				PlaneIntersectionType intersectType = bullet.m_xmOOBB.Intersects(XMLoadFloat4(&m_pxmf4WallPlanes[j]));
				if (intersectType == INTERSECTING)
				{
					nPlaneIndex = j;
					break;
				}
			}
			if (nPlaneIndex != -1)
			{
				bullet.m_bRender = FALSE;
			}
			break;
		}
		case CONTAINS:
			break;
		}
	}

	//물체와 총알 충돌검사
	for (auto& bullet : m_lBullets)
	{
		if (bullet.m_bRender == FALSE)
			continue;
		for (int i = 0; i < m_nObjects; i++)
		{
			if (m_pObjects[i].m_bRender == FALSE)
				continue;
			if (m_pObjects[i].m_xmOOBB.Intersects(bullet.m_xmOOBB))
			{
				m_lParticles.push_back(CParticle(&m_pObjects[i], 15.0f));

				m_pObjects[i].m_bRender = FALSE;
				bullet.m_bRender = FALSE;
			}
			else if (m_pObjects[i].m_xmOOBB.Contains(bullet.m_xmOOBB) == (CONTAINS))
			{
				m_lParticles.push_back(CParticle(&m_pObjects[i], 15.0f));

				m_pObjects[i].m_bRender = FALSE;
				bullet.m_bRender = FALSE;
			}
		}
	}

	//총알과 플레이어의 거리 검사후 일정 거리 이상시 제거
	XMVECTOR xmvPlayerPos = XMLoadFloat3(&m_pPlayer->GetPosition());
	for (auto& bullet : m_lBullets) {
		if (bullet.m_bRender == FALSE)
			continue;
		XMFLOAT3 xmf3Length;
		XMStoreFloat3(&xmf3Length, XMVector3Length(XMLoadFloat3(&bullet.GetPosition()) - xmvPlayerPos));
		if (xmf3Length.x > m_fMakeDistance) {
			bullet.m_bRender = FALSE;
		}
	}

	//총알 제거
	m_lBullets.remove_if([](const CGameObject& a) {return a.m_bRender == FALSE; });

	//파티클 제거
	m_lParticles.remove_if([](const CParticle& a) {return a.m_bRender == FALSE; });
}

void CGameFramework::FrameAdvance()
{    
    if (!m_bActive) return;

	ProcessInput();

	AnimateObjects();

    ClearFrameBuffer(RGB(255, 255, 255));

	for(int i=0; i<m_nWalls; ++i)
		m_pWalls[i].Render(m_hDCFrameBuffer, m_pPlayer->m_pCamera);

	for (int i = 0; i < m_nObjects; i++) 
		if( m_pObjects[i].m_bRender)
			m_pObjects[i].Render(m_hDCFrameBuffer, m_pPlayer->m_pCamera);

	for (auto& bullet : m_lBullets) {
		if (bullet.m_bRender)
			bullet.Render(m_hDCFrameBuffer, m_pPlayer->m_pCamera);
	}

	for (auto& particle : m_lParticles) {
		if(particle.m_bRender)
			particle.Render(m_hDCFrameBuffer, m_pPlayer->m_pCamera);
	}

	if (m_pPlayer->m_bRender)
		m_pPlayer->Render(m_hDCFrameBuffer, m_pPlayer->m_pCamera);

	if(m_pPlayer->m_bRender == FALSE)
		TextOut(m_hDCFrameBuffer, CLIENT_WIDTH / 2 - 40, CLIENT_HEIGHT / 2 - 40, L"'R'로 재시작", 8);

	PresentFrameBuffer();
}


