#include "Trigger.h"
#include "GameInstance.h"
#include "Collider_Sphere.h"
#include "Collider_Cube.h"
#include "Door.h"

CTrigger::CTrigger(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CCollisionObject{ pGraphic_Device }
{
}

CTrigger::CTrigger(const CTrigger& Prototype)
	:CCollisionObject( Prototype )
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
		if(!pDesc->stTargetTag.empty())
		m_stTargetTag = pDesc->stTargetTag;
	}

	Find_Target();

	if (m_eTriggerType == TRIGGER_TYPE::INTERACTION)
	{
		m_pTransformCom->Set_Scale(40.f, 1.f, 40.f);
	}
	return S_OK;
}

void CTrigger::Update(_float fTimeDelta)
{
	Find_Target();
	if (m_pTransformCom && m_pColliderCom)
	{
		m_pColliderCom->Set_WorldMat(m_pTransformCom->Get_WorldMat());

		// 콜라이더 트랜스폼 업데이트
		m_pColliderCom->Update_Collider(TEXT("Com_Transform"), m_pTransformCom->Compute_Scaled());
	}

	if (m_eTriggerType == TRIGGER_TYPE::INTERACTION)
	{
		if (m_pTargetObject&& m_pTargetObject->IsMoving())
		{
			for (auto& monster : m_setMonsters)
			{
				monster->SetActive(true);
			}
			m_setMonsters.clear();
		}
	}

	if(!m_bInitMonsters)
	// 게임 인스턴스의 충돌 확인 시스템에 콜라이더 추가
	m_pGameInstance->Add_Collider(CG_TRIGGER, m_pColliderCom);
}

void CTrigger::Late_Update(_float fTimeDelta)
{
		m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CTrigger::Render()
{
	if (m_eTriggerType == TRIGGER_TYPE::BUTTON)
	{
		if (FAILED(m_pTextureCom->Bind_Resource(m_bWasTriggered)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTextureCom->Bind_Resource(0)))
			return E_FAIL;
	}
	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	SetUp_RenderState();

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;
	m_pColliderCom->Render();
	Release_RenderState();

	return S_OK;
}

HRESULT CTrigger::On_Collision(CCollisionObject* other)
{
	// 비활성 상태에서는 충돌 처리하지 않음
	if (!m_bIsActive)
		return S_OK;



	// 트리거 타입에 따른 처리
	switch (m_eTriggerType)
	{
	case TRIGGER_TYPE::BUTTON:
	{	// 플레이어인지 확인 (CG_PLAYER와 비교)
		if (other->Get_Type() != CG_PLAYER)
			return S_OK;
 		if (!m_bWasTriggered&&GetAsyncKeyState(VK_SPACE)&0x8000)
		{
			m_bWasTriggered = true;
			OnTrigger_Activated();
		}
		break;
	}
	case TRIGGER_TYPE::INTERACTION:
	{
		if (!m_bInitMonsters)
		{
			m_setMonsters.insert(other);

			if (m_iPrevSize == m_setMonsters.size())
			{
				m_bInitMonsters = true;
				for (auto& monster : m_setMonsters)
				{
					monster->SetActive(false);
				}
				break;
			}
			m_iPrevSize = m_setMonsters.size();

		}

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

void CTrigger::AddTargetObject(CDoor* pTarget)
{
	if (pTarget != nullptr)
		m_pTargetObject = pTarget;
	Safe_AddRef(m_pTargetObject);
}

void CTrigger::OnTrigger_Activated()
{
	if (m_pTargetObject)
	{
		m_pTargetObject->Open_Door();
	}
}

void CTrigger::OnTrigger_Deactivated()
{
}

void CTrigger::Find_Target()
{
	if (!m_pTargetObject&& !m_stTargetTag.empty())
	{
		CDoor* pDoor = dynamic_cast<CDoor*>(m_pGameInstance->Find_Object(m_tObjDesc.iLevel, m_stTargetTag));
		AddTargetObject(pDoor);
	}

	
}

HRESULT CTrigger::SetUp_RenderState()
{
	// 일단 추가해보기

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); // 알파 값이 기준보다 크면 픽셀 렌더링
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200); // 기준값 설정 (0~255)

	return S_OK;
}

HRESULT CTrigger::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
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
	m_eType = CG_TRIGGER;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	if (m_eTriggerType == TRIGGER_TYPE::BUTTON)
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Prototype_Component_VIBuffer_Rect",
			TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
			return E_FAIL;
	}
	else
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Prototype_Component_VIBuffer_TextureCube",
			TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
			return E_FAIL;
	}


	/* For.Com_Material */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Material"),
		TEXT("Com_Material"), reinterpret_cast<CComponent**>(&m_pMaterialCom))))
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
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pMaterialCom);
	Safe_Release(m_pTargetObject);
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

	if (m_pTargetObject)
	{
		j["target_tags"] = ISerializable::WideToUtf8(m_pTargetObject->Get_Tag());
	}
	else
	{
		j["target_tags"] = "";
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

	if (j.contains("target_tags"))
		m_stTargetTag = ISerializable::Utf8ToWide(j["target_tags"].get<string>());

}