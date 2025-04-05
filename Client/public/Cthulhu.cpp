#include "Cthulhu.h"
#include "Player.h"
#include "Transform.h"
CCthulhu::CCthulhu(LPDIRECT3DDEVICE9 pGraphic_Device):
    CMonster_Base(pGraphic_Device)
{
}

CCthulhu::CCthulhu(const CCthulhu& Prototype)
    : CMonster_Base(Prototype)
{
}

HRESULT CCthulhu::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCthulhu::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	// 행동트리 인스턴스 생성
	if (!m_pBehaviorTree)
	{
		m_pBehaviorTree = new CBehaviorTree();
	}
	
    return S_OK;
}

void CCthulhu::Priority_Update(_float fTimeDelta)
{
}

void CCthulhu::Update(_float fTimeDelta)
{
	Billboarding(fTimeDelta);
	if (m_pBehaviorTree)
		m_pBehaviorTree->Run();
}

void CCthulhu::Late_Update(_float fTimeDelta)
{
}

HRESULT CCthulhu::Render()
{
    return S_OK;
}

void CCthulhu::Billboarding(_float fTimeDelta)
{
	if (!m_pTarget) return;


	CTransform* pPlayerTransform = static_cast<CPlayer*>(m_pTarget)->Get_TransForm();

	_float3 vPlayerLook = pPlayerTransform->Get_State(CTransform::STATE_LOOK);

	vPlayerLook.y = 0.f;
	vPlayerLook.Normalize();

	_float3 vShopLook = -vPlayerLook;


	_float3 vUp = _float3(0.0f, 1.0f, 0.0f);
	_float3 vRight = vUp.Cross(vShopLook);
	vRight.Normalize();

	_float3 vNewUp = vShopLook.Cross(vRight);
	vNewUp.Normalize();

	_float3 vScale = m_pTransformCom->Compute_Scaled();

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight * vScale.x);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vNewUp * vScale.y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vShopLook * vScale.z);
}

CGameObject* CCthulhu::Clone(void* pArg)
{
    return nullptr;
}

void CCthulhu::Free()
{
	__super::Free();
}
