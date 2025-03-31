#include "Camera_Free.h"
#include "PickingSys.h" // 테스트 용으로 헤더 추가
#include "MyImGui.h"

CCamera_Free::CCamera_Free(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CCamera { pGraphic_Device }
{
}

CCamera_Free::CCamera_Free(const CCamera_Free& Prototype)
	: CCamera{ Prototype }
{
}

HRESULT CCamera_Free::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Free::Initialize(void* pArg)
{
	/* TransformCom을 생성해놓는다. */
	if (FAILED(Ready_Components()))
		return E_FAIL;

	CAMERA_FREE_DESC	Desc{};

	Desc.vEye = _float3(0.f, 10.f, -10.f);
	Desc.vAt = _float3(0.f, 0.f, 0.f);
	Desc.fFov = D3DXToRadian(90.f);
	Desc.fNear = 0.1f;
	Desc.fFar = 300.f;
	Desc.fMouseSensor = 1.f;

	m_fMouseSensor = 0.1f;

	m_bIsRotating = false;    // 카메라 회전 활성화 상태
	m_fDamping = 0.9f;        // 감속 계수 (1.0에 가까울수록 움직임이 부드러움)
	m_vCurrentVelocity = _float3(0.f, 0.f, 0.f); // 현재 속도 벡터
	m_iWinCenterX = g_iWinSizeX / 2;
	m_iWinCenterY = g_iWinSizeY / 2;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	//SetCursorPos(g_iWinSizeX / 2, g_iWinSizeY / 2);
	GetCursorPos(&m_ptOldMousePos);
	ScreenToClient(g_hWnd, &m_ptOldMousePos);

	CPickingSys::Get_Instance()->Set_CameraTransform(m_pTransformCom);
	return S_OK;
}

void CCamera_Free::Priority_Update(_float fTimeDelta)
{
	POINT ptMouse{};
	_float fSpeed = 1.f;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	// 마우스가 클라이언트 영역 내에 있는지 확인
	RECT rcClient;
	GetClientRect(g_hWnd, &rcClient);
	bool bIsInClientArea = (ptMouse.x >= rcClient.left && ptMouse.x <= rcClient.right &&
		ptMouse.y >= rcClient.top && ptMouse.y <= rcClient.bottom);

	// 마우스 우클릭 상태 확인
	bool bRightButtonDown = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;

	// 우클릭 상태 변경 감지
	if (bRightButtonDown && !m_bIsRotating && bIsInClientArea)
	{
		m_bIsRotating = true;
		// 카메라 회전 모드 시작 시 마우스 위치 저장
		m_ptOldMousePos = ptMouse;
		// 우클릭 회전 모드에서는 마우스 커서 숨김 (옵션)
		ShowCursor(FALSE);
	}
	else if (!bRightButtonDown && m_bIsRotating)
	{
		m_bIsRotating = false;
		// 회전 모드 종료 시 커서 복원
		ShowCursor(TRUE);
	}

	// 이동 속도 조절 (SHIFT 키로 가속)
	if (GetKeyState(VK_SHIFT) & 0x8000)
		fSpeed = 3.f;
	else
		fSpeed = 1.f;

	// 목표 속도 계산
	_float3 vTargetVelocity = _float3(0.f, 0.f, 0.f);

	/* 카메라의 움직임 제어 */


	if (GetKeyState('W') & 0x8000)
	{
		vTargetVelocity.z += 1.f * fSpeed;
	}
	if (GetKeyState('S') & 0x8000)
	{
		vTargetVelocity.z -= 1.f * fSpeed;
	}
	if (GetKeyState('A') & 0x8000)
	{
		vTargetVelocity.x -= 1.f * fSpeed;
	}
	if (GetKeyState('D') & 0x8000)
	{
		vTargetVelocity.x += 1.f * fSpeed;
	}
	if (GetKeyState('Q') & 0x8000) // 수직 이동 추가
	{
		vTargetVelocity.y -= 1.f * fSpeed;
	}
	if (GetKeyState('E') & 0x8000) // 수직 이동 추가
	{
		vTargetVelocity.y += 1.f * fSpeed;
	}
 

	// 속도 벡터 업데이트 (부드러운 감속 적용)
	m_vCurrentVelocity.x = Lerp(m_vCurrentVelocity.x, vTargetVelocity.x, 1.0f - powf(m_fDamping, fTimeDelta * 60.f));
	m_vCurrentVelocity.y = Lerp(m_vCurrentVelocity.y, vTargetVelocity.y, 1.0f - powf(m_fDamping, fTimeDelta * 60.f));
	m_vCurrentVelocity.z = Lerp(m_vCurrentVelocity.z, vTargetVelocity.z, 1.0f - powf(m_fDamping, fTimeDelta * 60.f));

	// 이동 적용
	if (abs(m_vCurrentVelocity.z) > 0.001f)
		m_pTransformCom->Go_Straight(fTimeDelta * m_vCurrentVelocity.z);
	if (abs(m_vCurrentVelocity.x) > 0.001f)
		m_pTransformCom->Go_Right(fTimeDelta * m_vCurrentVelocity.x);
	if (abs(m_vCurrentVelocity.y) > 0.001f)
		m_pTransformCom->Go_Up(fTimeDelta * m_vCurrentVelocity.y);

	// 마우스 회전 (우클릭 상태이고 클라이언트 영역 내에 있을 때만)
	if (m_bIsRotating && bIsInClientArea)
	{
		_int iMouseMoveX = ptMouse.x - m_ptOldMousePos.x;
		_int iMouseMoveY = ptMouse.y - m_ptOldMousePos.y;

		if (iMouseMoveX != 0)
		{
			// X축 회전 (좌우 회전)
			m_pTransformCom->Turn(_float3(0.f, 1.f, 0.f), fTimeDelta * iMouseMoveX * m_fMouseSensor);
		}

		if (iMouseMoveY != 0)
		{
			// Y축 회전 (상하 회전)
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fTimeDelta * iMouseMoveY * m_fMouseSensor);
		}

		// 마우스가 화면 가장자리에 닿으면 중앙으로 재설정
		if (ptMouse.x < rcClient.left + 50 || ptMouse.x > rcClient.right - 50 ||
			ptMouse.y < rcClient.top + 50 || ptMouse.y > rcClient.bottom - 50)
		{
			SetCursorPos(m_iWinCenterX, m_iWinCenterY);
			GetCursorPos(&ptMouse);
			ScreenToClient(g_hWnd, &ptMouse);
		}
	}

	m_ptOldMousePos = ptMouse;

	/* DX는 한번 셋한 값은 바꾸기 전까지 유지된다. */
	/* 갱신된 카메라월드를 통해서 뷰행렬을 만들어 장치에 셋한다.  */
	/* 갱신된 FOV, NEAR, FAR으로 투영행렬을 만들어 장치에 셋한다. */
	__super::Update_VP_Matrices();
}

void CCamera_Free::Update(_float fTimeDelta)
{

}

void CCamera_Free::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera_Free::Render()
{
	return S_OK;
}

HRESULT CCamera_Free::Ready_Components()
{
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 30.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(180.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

CCamera_Free* CCamera_Free::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCamera_Free* pInstance = new CCamera_Free(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_Free");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Free::Clone(void* pArg)
{
	CCamera_Free* pInstance = new CCamera_Free(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CCamera_Free");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Free::Free()
{
	__super::Free();


}
