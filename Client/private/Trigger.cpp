#include "Trigger.h"
#include "GameInstance.h"
#include "Collider_Sphere.h"
#include "Collider_Cube.h"

CTrigger::CTrigger(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CCollisionObject{ pGraphic_Device }
{
}

CTrigger::CTrigger(const CTrigger& Prototype)
	:CCollisionObject{ Prototype }
{
}

HRESULT CTrigger::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTrigger::Initialize(void* pArg)
{
	INIT_PARENT(pArg)

		if (FAILED(Ready_Components()))
			return E_FAIL;

	if (pArg != nullptr)
	{
		TRIGGER_DESC* pDesc = (TRIGGER_DESC*)pArg;
		m_eTriggerType = pDesc->eType;
		m_bIsActive = pDesc->bStartsActive;
	}

	return S_OK;
}

void CTrigger::Update(_float fTimeDelta)
{
	if (m_pTransformCom && m_pColliderCom)
	{
		m_pColliderCom->Set_WorldMat(m_pTransformCom->Get_WorldMat());

		// 콜라이더 트랜스폼 업데이트
		m_pColliderCom->Update_Collider(TEXT("Com_Transform"), m_pTransformCom->Compute_Scaled());
	}

	// 게임 인스턴스의 충돌 확인 시스템에 콜라이더 추가
	m_pGameInstance->Add_Collider(CG_TRIGGER, m_pColliderCom);
}

void CTrigger::Late_Update(_float fTimeDelta)
{
	// 디버그 모드에서만 렌더링 그룹에 추가
	if (g_bDebugCollider)
		m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CTrigger::Render()
{
	// 디버그 모드에서만 트리거 콜라이더 렌더링
	if (g_bDebugCollider && m_pColliderCom)
	{
		m_pColliderCom->Render();
	}

	return S_OK;
}

HRESULT CTrigger::On_Collision(CCollisionObject* other)
{
	// 비활성 상태에서는 충돌 처리하지 않음
	if (!m_bIsActive)
		return S_OK;

	// 플레이어인지 확인 (CG_PLAYER와 비교)
	if (other->Get_Type() != CG_PLAYER)
		return S_OK;

	// 트리거 타입에 따른 처리
	switch (m_eTriggerType)
	{
	case TRIGGER_TYPE::BUTTON:
	{
		// 버튼은 충돌 시 바로 활성화
		if (!m_bWasTriggered)
		{
			m_bWasTriggered = true;
			OnTrigger_Activated();
		}
		break;
	}
	case TRIGGER_TYPE::INTERACTION:
	{

		// 키 입력 로직은 게임의 입력 시스템과 연동 필요
		break;
	}
	}

	return S_OK;
}

void CTrigger::Activate()
{
	if (!m_bIsActive)
	{
		m_bIsActive = true;
		OnTrigger_Activated();
	}
}

void CTrigger::Deactivate()
{
	if (m_bIsActive)
	{
		m_bIsActive = false;
		m_bWasTriggered = false;
		OnTrigger_Deactivated();
	}
}

void CTrigger::AddTargetObject(CGameObject* pTarget)
{
	if (pTarget != nullptr)
		m_TargetObject = pTarget;
	Safe_AddRef(m_TargetObject);
}

void CTrigger::OnTrigger_Activated()
{

}

void CTrigger::OnTrigger_Deactivated()
{

}

HRESULT CTrigger::Ready_Components()
{
	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC TransformDesc{ 1.0f, D3DXToRadian(90.0f) };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;


	CCollider_Cube::COL_CUBE_DESC ColliderDesc = {};
	ColliderDesc.pOwner = this;
	ColliderDesc.fScale = { 1.0f, 1.0f, 1.0f };
	ColliderDesc.fLocalPos = { 0.0f, 0.0f, 0.0f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;


	if (!m_tObjDesc.stProtTextureTag.empty())
	{
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(m_tObjDesc.iProtoLevel, m_tObjDesc.stProtTextureTag,
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}

	return S_OK;
}

CTrigger* CTrigger::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTrigger* pInstance = new CTrigger(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTrigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTrigger::Clone(void* pArg)
{
	CTrigger* pInstance = new CTrigger(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTrigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrigger::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_TargetObject);
}

json CTrigger::Serialize()
{
	json j = __super::Serialize();
	// 트랜스폼 데이터 직렬화
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

	j["trigger_type"] = static_cast<int>(m_eTriggerType);

	if (m_TargetObject)
	{
		j["target_tags"] = ISerializable::WideToUtf8(m_TargetObject->Get_Tag());
	}

	return j;
}

void CTrigger::Deserialize(const json& j)
{
	SET_TRANSFORM(j, m_pTransformCom);

	if (j.contains("trigger_type"))
		m_eTriggerType = static_cast<TRIGGER_TYPE>(j["trigger_type"].get<int>());

	if (j.contains("is_active"))
		m_bIsActive = j["is_active"].get<bool>();

}