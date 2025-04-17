#include "AttackMelee.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "Collider_Cube.h"

CAttackMelee::CAttackMelee(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CProjectile_Base(pGraphic_Device)
{
}

CAttackMelee::CAttackMelee(const CAttackMelee& Prototype)
	:CProjectile_Base(Prototype)
{
}

HRESULT CAttackMelee::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAttackMelee::Initialize(void* pArg)
{
	
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		MELEE_DESC* pDesc = static_cast<MELEE_DESC*>(pArg);


		m_vPos = pDesc->vPos;
		m_vDir = pDesc->vDir;
		m_iAp = pDesc->iDamage;
		m_fLifeTime = pDesc->fLifeTIme;

		m_pTransformCom->Set_Scale(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPos);


	}


	m_eType = CG_MONSTER_MELEE;



	return S_OK;
}

void CAttackMelee::Priority_Update(_float fTimeDelta)
{
	m_fLifeTime -= fTimeDelta;

	if (m_fLifeTime <= 0)
	{
		m_bIsActive = false;
	}
}

void CAttackMelee::Update(_float fTimeDelta)
{
	m_pColliderCom->Update_Collider(TEXT("Com_Transform"), m_pTransformCom->Compute_Scaled());

	m_pGameInstance->Add_Collider(CG_MONSTER_MELEE, m_pColliderCom);
}

void CAttackMelee::Late_Update(_float fTimeDelta)
{
}

HRESULT CAttackMelee::Render()
{
	return E_NOTIMPL;
}

HRESULT CAttackMelee::On_Collision(CCollisionObject* other)
{
	if (nullptr == m_pColliderCom)
		return E_FAIL;

	if (nullptr == other)
		return S_OK;


	// 안바뀌면 충돌 안일어남
	if (other->Get_Type() == CG_END)
		return S_OK;



	switch (other->Get_Type())
	{
	case CG_PLAYER:

		Take_Damage(other);

		m_bIsActive = false;

	break;

	}
	return S_OK;
}

HRESULT CAttackMelee::Ready_Components()
{
	/* For.Com_Collider */
	CCollider_Cube::COL_CUBE_DESC	ColliderDesc = {};
	ColliderDesc.pOwner = this;
	// 이걸로 콜라이더 크기 설정
	ColliderDesc.fScale = { 1.f, 1.f, 1.f };
	// 오브젝트와 상대적인 거리 설정
	ColliderDesc.fLocalPos = { 0.f, 0.f, 0.f };

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 10.f, D3DXToRadian(90.f) };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

CAttackMelee* CAttackMelee::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CAttackMelee* pInstance = new CAttackMelee(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CAttackMelee");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAttackMelee::Clone(void* pArg)
{
	CAttackMelee* pInstance = new CAttackMelee(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CAttackMelee");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAttackMelee::Free()
{
	__super::Free();
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
}
