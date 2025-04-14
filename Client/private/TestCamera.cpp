#include "TestCamera.h"
#include "GameInstance.h"
#include "PickingSys.h" // 테스트 용으로 헤더 추가
#include "Sound_Listener.h"


static POINT ptLast = { 0, 0 };

CTestCamera::CTestCamera(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CCamera{ pGraphic_Device }
{
}

CTestCamera::CTestCamera(const CTestCamera& Prototype)
    : CCamera{ Prototype }
{
}

HRESULT CTestCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTestCamera::Initialize(void* pArg)
{
	/* TransformCom을 생성해놓는다. */
	if (FAILED(Ready_Components()))
		return E_FAIL;

	CAMERA_FREE_DESC	Desc{};

	Desc.vEye = _float3(0.f, 100.f, -10.f);
	Desc.vAt = _float3(0.f, 0.f, 0.f);
	Desc.vAt = _float3(0.f, 10.f, 0.f);
	Desc.fFov = D3DXToRadian(45.f);
	Desc.fNear = 0.1f;
	Desc.fFar = 300.f;
	Desc.fFar = 200.f;
	Desc.fMouseSensor = 1.f;

	m_fMouseSensor = 0.1f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	//SetCursorPos(g_iWinSizeX / 2, g_iWinSizeY / 2);
	GetCursorPos(&m_ptOldMousePos);
	ScreenToClient(g_hWnd, &m_ptOldMousePos);

	CPickingSys::Get_Instance()->Set_CameraTransform(m_pTransformCom);


	m_pPlayer = m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Player"));
	if (nullptr == m_pPlayer)
	{
		m_pPlayer = m_pGameInstance->Find_Object(LEVEL_EDITOR, TEXT("Layer_Player"));
		if (nullptr == m_pPlayer)
		{
			m_pPlayer = m_pGameInstance->Find_Object(LEVEL_HUB, TEXT("Layer_Player"));
			if (nullptr == m_pPlayer)
			{
				m_pPlayer = m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
				if (nullptr == m_pPlayer)
				{
					return E_FAIL;
				}
			}
		}
	}


	Safe_AddRef(m_pPlayer);
	Camera_Lock(false);
	return S_OK;
}

void CTestCamera::Priority_Update(_float fTimeDelta)
{
	CTransform* fPlayerTrans = static_cast<CPlayer*>(m_pPlayer)->Get_TransForm();

	_float3 fScale = fPlayerTrans->Compute_Scaled();

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, fPlayerTrans->Get_State(CTransform::STATE_RIGHT).GetNormalized() * m_vScale.x);
	m_pTransformCom->Set_State(CTransform::STATE_UP, fPlayerTrans->Get_State(CTransform::STATE_UP).GetNormalized() * m_vScale.y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, fPlayerTrans->Get_State(CTransform::STATE_LOOK).GetNormalized() * m_vScale.z);

	


	m_pListenerCom->Set_Listner(*fPlayerTrans);
	if (!m_bCameraLocked)
	{
		Shaking(fTimeDelta);

		HandleMouseInput(fTimeDelta);
	}
	__super::Update_VP_Matrices();
	fPlayerTrans->Set_State(CTransform::STATE_RIGHT, m_pTransformCom->Get_State(CTransform::STATE_RIGHT).GetNormalized() * fScale.x);
	fPlayerTrans->Set_State(CTransform::STATE_UP, m_pTransformCom->Get_State(CTransform::STATE_UP).GetNormalized() * fScale.y);
	fPlayerTrans->Set_State(CTransform::STATE_LOOK, m_pTransformCom->Get_State(CTransform::STATE_LOOK).GetNormalized() * fScale.z);
}

void CTestCamera::Update(_float fTimeDelta)
{
	if (GetAsyncKeyState(VK_TAB) & 0xf8000)
	{
		Camera_Lock(!m_bCameraLocked);
	}
}

void CTestCamera::HandleMouseInput(_float fTimeDelta)
{
	POINT pt;
	GetCursorPos(&pt);

	static POINT ptLast = pt;

	// 클라이언트 영역 중앙 좌표 계산
	RECT rect;
	GetClientRect(g_hWnd, &rect);
	int centerX = rect.right / 2;
	int centerY = rect.bottom / 2;

	// 클라이언트 중앙 좌표를 화면 좌표로 변환
	POINT screenCenter = { centerX, centerY };
	ClientToScreen(g_hWnd, &screenCenter);

	// 마우스 이동량 계산 (화면 좌표 기반)
	_float fDeltaX = (pt.x - ptLast.x) * m_fSensitivity;
	_float fDeltaY = (pt.y - ptLast.y) * m_fSensitivity;

	m_fYaw += fDeltaX;
	m_fPitch += fDeltaY;

	if (m_fPitch > D3DX_PI / 2.0f)
		m_fPitch = D3DX_PI / 2.0f;
	if (m_fPitch < -D3DX_PI / 2.0f)
		m_fPitch = -D3DX_PI / 2.0f;

	D3DXMATRIX matRotation;
	D3DXMatrixRotationYawPitchRoll(&matRotation, m_fYaw, m_fPitch, 0.0f);

	_float3 vLook = { 0.0f, 0.0f, 1.0f };
	D3DXVec3TransformNormal(&vLook, &vLook, &matRotation);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);

	_float3 vRight = { 1.0f, 0.0f, 0.0f };
	D3DXVec3TransformNormal(&vRight, &vRight, &matRotation);
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);

	_float3 vUp = { 0.0f, 1.0f, 0.0f };
	D3DXVec3TransformNormal(&vUp, &vUp, &matRotation);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
	// 마우스 커서를 화면 중앙으로 이동
	SetCursorPos(screenCenter.x, screenCenter.y);

	// ptLast 변수를 화면 중앙 좌표로 업데이트
	ptLast.x = screenCenter.x;
	ptLast.y = screenCenter.y;
}

void CTestCamera::Late_Update(_float fTimeDelta)
{
}

HRESULT CTestCamera::Render()
{
    return E_NOTIMPL;
}

void CTestCamera::Shaking(_float fTimeDelta)
{
}

HRESULT CTestCamera::Ready_Components()
{
    return E_NOTIMPL;
}

CTestCamera* CTestCamera::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    return nullptr;
}

CGameObject* CTestCamera::Clone(void* pArg)
{
    return nullptr;
}

void CTestCamera::Free()
{
}
