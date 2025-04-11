#include "Monster_Base.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Collider_Cube.h"
#include "Transform.h"
#include "Player.h"
#include "GameInstance.h"
#include "Stains_Effect.h"
#include "Gib_Effect.h"

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

	m_eType = CG_MONSTER;

	return S_OK;
}

HRESULT CMonster_Base::Ready_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_tObjDesc.stBufferTag,
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

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_BaseShader"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Material */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Material"),
		TEXT("Com_Material"), reinterpret_cast<CComponent**>(&m_pMaterialCom))))
		return E_FAIL;						

	return S_OK;
}

void CMonster_Base::Priority_Update(_float fTimeDelta)
{
	if (nullptr == m_pTarget)
	{
		CGameObject* pTarget = m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Player"));
		if (nullptr == pTarget)
			return;

		SetTarget(pTarget);
		Safe_AddRef(pTarget);

	}

	if (!m_bCheck)
	{
		if (m_pTrigger == static_cast<CCollisionObject*>(m_pTarget)->Get_Trigger() && nullptr != static_cast<CCollisionObject*>(m_pTarget)->Get_Trigger())
			m_bCheck = true;
	}

	if (m_iHp <= 0)
		m_eCurState = MS_DEATH;

	m_vCurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_vNextPos = m_vCurPos;
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

void CMonster_Base::Chasing(_float fTimeDelta, _float fDist)
{

	m_pTransformCom->Chase(static_cast<CPlayer*>(m_pTarget)->Get_TransForm()->Get_State(CTransform::STATE_POSITION), fTimeDelta * m_fSpeed, fDist);

	m_vNextPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vCurPos);
	
}

void CMonster_Base::Set_Trigger()
{

}

HRESULT CMonster_Base::Create_Stains(_uint effectNum)
{
	float offsetRangeX = 1.f, offsetRangeZ = 1.f;
	
	CStains_Effect::HIT_DESC hitDesc;
	hitDesc.vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	hitDesc.vUp = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	hitDesc.vLook = m_pTransformCom->Get_State(CTransform::STATE_UP);
	hitDesc.vScale = { 1.5f, 1.f, 1.5f };
	hitDesc.type = rand() % 2;

	for (_uint i = 0; i < effectNum; ++i)
	{
		_float offsetX = (((rand() % 100) / 100.f) - 0.5f) * (offsetRangeX * 2.0f);
		_float offsetZ = (((rand() % 100) / 100.f) - 0.5f) * (offsetRangeZ * 2.0f);
		_float3 vOffset = _float3(offsetX, 0.f, offsetZ);
		hitDesc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + vOffset;
		m_pGameInstance->Add_GameObject(
			LEVEL_STATIC,
			TEXT("Prototype_GameObject_Stains_Effect"),
			LEVEL_STATIC,
			TEXT("Layer_Stain_Effect"),
			&hitDesc);
	}
	return S_OK; 
}

HRESULT CMonster_Base::Create_Gibs(_uint eType)
{
	float offsetRangeX = 1.f, offsetRangeY = 1.f;

	CGib_Effect::HIT_DESC hitDesc;
	hitDesc.vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	hitDesc.vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	hitDesc.vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	hitDesc.vScale = { 1.5f, 1.f, 1.5f };
	hitDesc.type = eType;

	hitDesc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_pGameInstance->Add_GameObject(
		LEVEL_STATIC,
		TEXT("Prototype_GameObject_Gib_Effect"),
		LEVEL_STATIC,
		TEXT("Layer_Gib_Effect"),	
		&hitDesc);
	return S_OK;
}

json CMonster_Base::Serialize()
{

		json j = __super::Serialize();

		auto pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		auto scale = m_pTransformCom->Compute_Scaled();
		auto angle = m_pTransformCom->Get_EulerAngles();
		j["position"] = {
		RoundToDecimalPlaces(pos.x, 2),
		RoundToDecimalPlaces(pos.y, 2),
		RoundToDecimalPlaces(pos.z, 2)
		};
		j["rotation"] = {
			RoundToDecimalPlaces(angle.x, 2),
			RoundToDecimalPlaces(angle.y, 2),
			RoundToDecimalPlaces(angle.z, 2)
		};
		j["scale"] = {
			RoundToDecimalPlaces(scale.x, 2),
			RoundToDecimalPlaces(scale.y, 2),
			RoundToDecimalPlaces(scale.z, 2)
		};

		return j;
}

void CMonster_Base::Free()
{
	__super::Free();
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pMaterialCom);
	Safe_Release(m_pShaderCom);
}
