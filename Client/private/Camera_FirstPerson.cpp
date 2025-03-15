#include "Camera_FirstPerson.h"
#include "GameInstance.h"
#include "PickingSys.h" // 테스트 용으로 헤더 추가

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

	Desc.vEye = _float3(0.f, 10.f, -10.f);
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


	m_pPlayer = m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
	if (nullptr == m_pPlayer)
		return E_FAIL;

	Safe_AddRef(m_pPlayer);
	
	return S_OK;
}

void CCamera_FirstPerson::Priority_Update(_float fTimeDelta)
{
	CTransform* fPlayerTrans = static_cast<CPlayer*>(m_pPlayer)->Get_TransForm();

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, fPlayerTrans->Get_State(CTransform::STATE_RIGHT));
	m_pTransformCom->Set_State(CTransform::STATE_UP, fPlayerTrans->Get_State(CTransform::STATE_UP));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, fPlayerTrans->Get_State(CTransform::STATE_LOOK));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, fPlayerTrans->Get_State(CTransform::STATE_POSITION));

	

	__super::Update_VP_Matrices();

}

void CCamera_FirstPerson::Update(_float fTimeDelta)
{
}

void CCamera_FirstPerson::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera_FirstPerson::Render()
{
    return S_OK;
}

HRESULT CCamera_FirstPerson::Ready_Components()
{
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 30.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(180.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

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
		MSG_BOX("Failed to Created : CCamera_Free");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_FirstPerson::Free()
{
	__super::Free();

	Safe_Release(m_pPlayer);
}
