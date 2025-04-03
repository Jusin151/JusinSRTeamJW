#include "Monster_Base.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Collider_Cube.h"
#include "Transform.h"
#include "Player.h"
#include "GameInstance.h"

CMonster_Base::CMonster_Base(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CCollisionObject{ pGraphic_Device }
{
}

CMonster_Base::CMonster_Base(const CMonster_Base& Prototype)
	: CCollisionObject{ Prototype }
{
}

HRESULT CMonster_Base::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster_Base::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_pTransformCom->Set_Scale(0.5f, 0.5f, 0.5f);

	m_eType = CG_MONSTER;

	return S_OK;
}

void CMonster_Base::Priority_Update(_float fTimeDelta)
{
}

void CMonster_Base::Update(_float fTimeDelta)
{
	m_vDir = m_vNextPos - m_vCurPos;
	m_fLength = m_vDir.Length();
	m_vDir.Normalize();

	Look_Player();
	//m_pColliderCom->Set_WorldMat(m_pTransformCom->Get_WorldMat());
}

void CMonster_Base::Late_Update(_float fTimeDelta)
{
	
}

HRESULT CMonster_Base::Render()
{
	return S_OK;
}


void CMonster_Base::Look_Player()
{
	if (TEXT("Layer_Player") != m_pTarget->Get_Tag())
		return;

	m_pTransformCom->LookAt(static_cast<CPlayer*>(m_pTarget)->Get_TransForm()->Get_State(CTransform::STATE_POSITION));
}

void CMonster_Base::Chasing(_float fTimeDelta)
{

	m_pTransformCom->Chase(static_cast<CPlayer*>(m_pTarget)->Get_TransForm()->Get_State(CTransform::STATE_POSITION), fTimeDelta * m_fSpeed);

	m_vNextPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vCurPos);

	
}

HRESULT CMonster_Base::Ready_Components()
{

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC,m_tObjDesc.stBufferTag,
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 10.f, D3DXToRadian(90.f) };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;


	/* For.Com_Collider */
	CCollider_Cube::COL_CUBE_DESC	ColliderDesc = {};
	ColliderDesc.pOwner = this;
	// 이걸로 콜라이더 크기 설정
	ColliderDesc.fScale = { 0.5f,0.5f,0.5f };
	// 오브젝트와 상대적인 거리 설정
	ColliderDesc.fLocalPos = { 0.f, 0.f, 0.f };


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

void CMonster_Base::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pColliderCom);
}
