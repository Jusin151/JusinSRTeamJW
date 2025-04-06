#include "Camera_FirstPerson.h"
#include "GameInstance.h"
#include "PickingSys.h" // 테스트 용으로 헤더 추가

static POINT ptLast = { 0, 0 };

CCamera_FirstPerson::CCamera_FirstPerson(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CCamera{ pGraphic_Device }
{
}

CCamera_FirstPerson::CCamera_FirstPerson(const CCamera_FirstPerson& Prototype)
    : CCamera{ Prototype }
{
}

HRESULT CCamera_FirstPerson::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera_FirstPerson::Initialize(void* pArg)
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
	
	return S_OK;
}

void CCamera_FirstPerson::Priority_Update(_float fTimeDelta)
{
	CTransform* fPlayerTrans = static_cast<CPlayer*>(m_pPlayer)->Get_TransForm();

	_float3 fScale = fPlayerTrans->Compute_Scaled();
	
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, fPlayerTrans->Get_State(CTransform::STATE_RIGHT).GetNormalized() * m_vScale.x);
	m_pTransformCom->Set_State(CTransform::STATE_UP, fPlayerTrans->Get_State(CTransform::STATE_UP).GetNormalized() * m_vScale.y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, fPlayerTrans->Get_State(CTransform::STATE_LOOK).GetNormalized() * m_vScale.z);
	auto vPos = fPlayerTrans->Get_State(CTransform::STATE_POSITION);
	vPos.y += 0.2f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,vPos);
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, fPlayerTrans->Get_State(CTransform::STATE_POSITION));
	UpdateRecoil(fTimeDelta);
	
	Shaking(fTimeDelta);

	
	
		HandleMouseInput(fTimeDelta);

	__super::Update_VP_Matrices();
	fPlayerTrans->Set_State(CTransform::STATE_RIGHT, m_pTransformCom->Get_State(CTransform::STATE_RIGHT).GetNormalized() * fScale.x);
	fPlayerTrans->Set_State(CTransform::STATE_UP, m_pTransformCom->Get_State(CTransform::STATE_UP).GetNormalized() * fScale.y);
	fPlayerTrans->Set_State(CTransform::STATE_LOOK, m_pTransformCom->Get_State(CTransform::STATE_LOOK).GetNormalized() * fScale.z);
	//fPlayerTrans->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
}

void CCamera_FirstPerson::Update(_float fTimeDelta)
{
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
	{
		m_tmpState = !m_tmpState;
	}
}
void CCamera_FirstPerson::HandleMouseInput(_float fTimeDelta)
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
void CCamera_FirstPerson::Late_Update(_float fTimeDelta)
{
	
}

HRESULT CCamera_FirstPerson::Render()
{
    return S_OK;
}

void CCamera_FirstPerson::UpdateRecoil(_float fTimeDelta)
{
	if (m_fRecoil > 0.f)
	{
		m_fYaw += m_fRecoil;
		m_fPitch -= m_fRecoil;

		m_fRecoil -= fTimeDelta * m_fRecoilDecay;
		if (m_fRecoil < 0.f)
			m_fRecoil = 0.f;
	}
}

void CCamera_FirstPerson::TriggerShake(_float shakeAmount, _float duration)
{
	m_fShakeAmount = shakeAmount;
	m_fShakeDuration = duration;
	m_fShakeTime = 0.f; // 초기화하여 새 효과 적용
	m_bTriggerShake = true; // 트리거 활성화
	m_vOriginalCameraPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION); // 원래 카메라 위치 저장
	m_vOriginalCameraPosition.y += 0.5f; // 카메라 위치 조정
}


void CCamera_FirstPerson::Shaking(_float fTimeDelta)
{
	if (m_bTriggerShake && m_fShakeDuration > 0.f)
	{
		m_fShakeTime += fTimeDelta;
		// 쉐이크 시간이 지속 시간을 초과하면 효과 종료
		if (m_fShakeTime >= m_fShakeDuration)
		{
			m_fShakeDuration = 0.f;
			m_fShakeAmount = 0.f;
			m_fShakeTime = 0.f;
			m_bTriggerShake = false; // 트리거 비활성화
			// 효과 종료 시 원래 위치로 복귀
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vOriginalCameraPosition);
			return;
		}

		// 매 프레임마다 랜덤 오프셋 생성 (범위: [-1,1] * 쉐이크 강도)
		float randomX = ((float)rand() / (float)RAND_MAX) * 1.5f - 1.f;
		float randomY = ((float)rand() / (float)RAND_MAX) * 1.5f - 1.f;
		_float3 randomOffset = { randomX * m_fShakeAmount, randomY * m_fShakeAmount, 0.f };

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vOriginalCameraPosition + randomOffset);
	}
	else
	{

	m_fShakeTime += fTimeDelta * 10.f;

	float shakeAmount = 0.42f; // 흔들림 강도
	_float3 shake = {
		cos(m_fShakeTime) * shakeAmount,
		sin(m_fShakeTime) * shakeAmount,
		0.f
	};

	// 보간 계수 (0.0 ~ 1.0 사이 값). 작을수록 부드럽지만 느리게 목표 도달.
	// 예시: 고정값 사용 (값을 조절하며 테스트 필요)
	float smoothFactor = 0.15f;
	// 또는 시간 기반 보간 (프레임 영향 받을 수 있으므로 clamp나 다른 기법 고려)
	// float smoothFactor = min(1.f, fTimeDelta * 10.f); // 예시

	bool bIsMoving = false; // 이동 키가 눌렸는지 확인

	if (GetAsyncKeyState('W') & 0x8000 ||
		GetAsyncKeyState('S') & 0x8000 ||
		GetAsyncKeyState('A') & 0x8000 ||
		GetAsyncKeyState('D') & 0x8000 )
	{
		bIsMoving = true;
		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + shake);
	}
	if (bIsMoving)
	{
		// 1. 현재 카메라 위치 가져오기
		_float3 currentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		// 2. 목표 위치 계산 (현재 위치 + 흔들림 오프셋)
		_float3 targetPos = currentPos + shake;

		// 3. 현재 위치에서 목표 위치로 부드럽게 보간하여 새 위치 계산
		_float3 newPos = VectorLerp(currentPos, targetPos, smoothFactor);
		// 만약 VectorLerp 헬퍼 함수를 사용하지 않는다면:
		/*
		_float3 newPos = {
			Lerp(currentPos.x, targetPos.x, smoothFactor),
			Lerp(currentPos.y, targetPos.y, smoothFactor),
			Lerp(currentPos.z, targetPos.z, smoothFactor) // Z축도 일관성을 위해 보간
		};
		*/

		// 4. 계산된 새 위치로 카메라 상태 업데이트
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, newPos);
	}
	// else
	// {
	//     // 만약 키를 뗐을 때 원래 위치로 부드럽게 돌아가게 하려면
	//     // 여기에 원래 위치(흔들림 없는)로 돌아가는 Lerp 로직 추가 필요
	// }

	}
}

HRESULT CCamera_FirstPerson::Ready_Components()
{
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 30.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(180.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;
	m_vScale = { 1.f, 1.f, 1.f };
	return S_OK;
}

CCamera_FirstPerson* CCamera_FirstPerson::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCamera_FirstPerson* pInstance = new CCamera_FirstPerson(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_Free");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_FirstPerson::Clone(void* pArg)
{
	CCamera_FirstPerson* pInstance = new CCamera_FirstPerson(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CCamera_FirstPerson");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_FirstPerson::Free()
{
	__super::Free();

	Safe_Release(m_pPlayer);
}
