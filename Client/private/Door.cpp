#include "Door.h"
#include "GameInstance.h"
#include "Collider_Sphere.h"
#include "Collider_Cube.h"
#include "Player.h"  


HRESULT CDoor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDoor::Initialize(void* pArg)
{
	INIT_PARENT(pArg)

		if (pArg != nullptr)
		{
			DOOR_DESC* pDesc = (DOOR_DESC*)pArg;
			m_eDoorType = pDesc->eType;
			m_fOpenAngle = pDesc->fOpenAngle;
			m_stKeyItemTag = pDesc->stKeyItemTag;
			m_bIsActive = pDesc->bStartsActive;
		}

	if (FAILED(Ready_Components()))
		return E_FAIL;

	// 초기 회전값 저장
	m_vOriginalRotation = m_pTransformCom->Get_EulerAngles();

	// 열렸을 때의 회전값 계산
	m_vOpenRotation = m_vOriginalRotation;
	m_vOpenRotation.y += D3DXToRadian(m_fOpenAngle);  // Y축 기준 회전

	return S_OK;
}

void CDoor::Update(_float fTimeDelta)
{
	// 문 애니메이션 업데이트
	if (m_eDoorState == DOOR_STATE::OPENING)
	{
		m_fDoorOpenAmount += m_fDoorOpenSpeed * fTimeDelta;
		if (m_fDoorOpenAmount >= 1.0f)
		{
			m_fDoorOpenAmount = 1.0f;
			m_eDoorState = DOOR_STATE::OPEN;
		}
		UpdateDoorTransform();
	}
	else if (m_eDoorState == DOOR_STATE::CLOSING)
	{
		m_fDoorOpenAmount -= m_fDoorOpenSpeed * fTimeDelta;
		if (m_fDoorOpenAmount <= 0.0f)
		{
			m_fDoorOpenAmount = 0.0f;
			m_eDoorState = DOOR_STATE::CLOSED;
		}
		UpdateDoorTransform();
	}

	// 충돌 처리
	if (m_pTransformCom && m_pColliderCom)
	{
		m_pColliderCom->Set_WorldMat(m_pTransformCom->Get_WorldMat());
		m_pColliderCom->Update_Collider(TEXT("Com_Transform"), m_pTransformCom->Compute_Scaled());
	}

	m_pGameInstance->Add_Collider(CG_DOOR, m_pColliderCom);
}

void CDoor::Late_Update(_float fTimeDelta)
{
	// 렌더링 그룹에 추가
	m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CDoor::Render()
{
	if (FAILED(m_pMaterialCom->Bind_Resource()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Resource(0)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	SetUp_RenderState();

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	Release_RenderState();

	return S_OK;
}

HRESULT CDoor::SetUp_RenderState()
{
	// 렌더 상태 설정
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);

	return S_OK;
}

HRESULT CDoor::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}

HRESULT CDoor::On_Collision(CCollisionObject* other)
{
	// 비활성 상태에서는 충돌 처리하지 않음
	if (!m_bIsActive)
		return S_OK;

	// 플레이어인지 확인
	if (other->Get_Type() != CG_PLAYER)
		return S_OK;

	// 여기서는 충돌만 감지하고, 실제 열기 동작은 외부 입력(스페이스바 등)에 의해 처리

	return S_OK;
}

void CDoor::Open_Door()
{
	if (m_eDoorState == DOOR_STATE::CLOSED || m_eDoorState == DOOR_STATE::CLOSING)
	{
		m_eDoorState = DOOR_STATE::OPENING;
	}
}

void CDoor::Close_Door()
{
	if (m_eDoorState == DOOR_STATE::OPEN || m_eDoorState == DOOR_STATE::OPENING)
	{
		m_eDoorState = DOOR_STATE::CLOSING;
	}
}

bool CDoor::TryOpen(CGameObject* pPlayer)
{
	// 열쇠가 필요한 문인 경우
	if (m_eDoorType == DOOR_TYPE::KEY && !m_stKeyItemTag.empty())
	{
		// 플레이어의 인벤토리에서 열쇠 확인
		CPlayer* pPlayerObj = dynamic_cast<CPlayer*>(pPlayer);
		if (pPlayerObj)
		{
			// 인벤토리에 아이템이 있는지 확인
			//bool hasKey = pPlayerObj->HasItem(m_stKeyItemTag);
			bool hasKey = true;  // 테스트용 임시 코드

			if (hasKey)
			{
				Open_Door();
				return true;
			}
			else
			{
				//"열쇠가 필요합니다"
				return false;
			}
		}
		return false;
	}
	else
	{
		// 일반 문은 바로 열림
		Open_Door();
		return true;
	}
}

void CDoor::UpdateDoorTransform()
{
	// 현재 열림 정도에 따라 회전 보간
	_float3 currentRotation;

	// 선형 보간: original + t * (open - original)
	currentRotation.x = m_vOriginalRotation.x + m_fDoorOpenAmount * (m_vOpenRotation.x - m_vOriginalRotation.x);
	currentRotation.y = m_vOriginalRotation.y + m_fDoorOpenAmount * (m_vOpenRotation.y - m_vOriginalRotation.y);
	currentRotation.z = m_vOriginalRotation.z + m_fDoorOpenAmount * (m_vOpenRotation.z - m_vOriginalRotation.z);

	// 새 회전값 적용
	m_pTransformCom->Rotate_EulerAngles(currentRotation);
}

HRESULT CDoor::Ready_Components()
{
	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC TransformDesc{ 1.0f, D3DXToRadian(90.0f) };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* For.Com_Collider */
	CCollider_Cube::COL_CUBE_DESC ColliderDesc = {};
	ColliderDesc.pOwner = this;
	ColliderDesc.fScale = { 1.0f, 1.0f, 1.f }; 
	ColliderDesc.fLocalPos = { 0.0f, 0.0f, 0.0f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Prototype_Component_VIBuffer_TexturedCube",
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Material */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Material"),
		TEXT("Com_Material"), reinterpret_cast<CComponent**>(&m_pMaterialCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (!m_tObjDesc.stProtTextureTag.empty())
	{
		if (FAILED(__super::Add_Component(m_tObjDesc.iProtoLevel, m_tObjDesc.stProtTextureTag,
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}

	return S_OK;
}

CDoor* CDoor::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CDoor* pInstance = new CDoor(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDoor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDoor::Clone(void* pArg)
{
	CDoor* pInstance = new CDoor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDoor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDoor::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pMaterialCom);
}

json CDoor::Serialize()
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

	// 문 관련 데이터 직렬화
	j["door_type"] = static_cast<int>(m_eDoorType);
	j["door_state"] = static_cast<int>(m_eDoorState);
	j["open_angle"] = m_fOpenAngle;
	j["open_speed"] = m_fDoorOpenSpeed;

	// 열쇠 태그 저장
	if (!m_stKeyItemTag.empty())
	{
		j["key_item_tag"] = ISerializable::WideToUtf8(m_stKeyItemTag);
	}

	return j;
}

void CDoor::Deserialize(const json& j)
{
	SET_TRANSFORM(j, m_pTransformCom);

	if (j.contains("door_type"))
		m_eDoorType = static_cast<DOOR_TYPE>(j["door_type"].get<int>());

	if (j.contains("door_state"))
		m_eDoorState = static_cast<DOOR_STATE>(j["door_state"].get<int>());

	if (j.contains("open_angle"))
		m_fOpenAngle = j["open_angle"].get<float>();

	if (j.contains("open_speed"))
		m_fDoorOpenSpeed = j["open_speed"].get<float>();

	if (j.contains("is_active"))
		m_bIsActive = j["is_active"].get<bool>();

	if (j.contains("key_item_tag"))
		m_stKeyItemTag = ISerializable::Utf8ToWide(j["key_item_tag"].get<std::string>());

	// 원래 회전값과 열린 회전값 다시 계산
	m_vOriginalRotation = m_pTransformCom->Get_EulerAngles();
	m_vOpenRotation = m_vOriginalRotation;
	m_vOpenRotation.y += D3DXToRadian(m_fOpenAngle);

	// 현재 상태에 따라 문 위치 조정
	if (m_eDoorState == DOOR_STATE::OPEN)
	{
		m_fDoorOpenAmount = 1.0f;
		UpdateDoorTransform();
	}
	else if (m_eDoorState == DOOR_STATE::CLOSED)
	{
		m_fDoorOpenAmount = 0.0f;
		UpdateDoorTransform();
	}
}