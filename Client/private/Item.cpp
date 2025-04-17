﻿#include "Item.h"
#include "Player.h"
#include "GameInstance.h"
#include "Collider_Sphere.h"
#include "Collider_Cube.h"
#include "Particles.h"
#include "Sound_Event.h"

namespace Item
{
	constexpr float ANIMATION_FRAME_RATE = 90.f;
	constexpr float MAX_FLOAT_OFFSET = 0.6f;
	constexpr float MIN_FLOAT_OFFSET = 0.2f;
	constexpr float FLOAT_FREQUENCY = 60.f;
}

CItem::CItem(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CCollisionObject{ pGraphic_Device }
{
}

CItem::CItem(const CItem& Prototype)
	:CCollisionObject( Prototype ),
	m_mapTextureTag{ Prototype.m_mapTextureTag }
	,m_pPlayer{Prototype.m_pPlayer}
{
}

HRESULT CItem::Initialize_Prototype()
{

	Init_TextureTag();

	return S_OK;
}

HRESULT CItem::Initialize(void* pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_bIsCubeCollider = (dynamic_cast<CCollider_Cube*>(m_pColliderCom) != nullptr);

	m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Player")));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(0.f, 0.6f, 0.f));
	m_pTransformCom->Set_Scale(0.5f, 0.5f, 0.5f);

	INIT_PARENT(pArg);

	if (pArg)
	{
		ITEM_DESC tItemDesc = *static_cast<ITEM_DESC*>(pArg);
		m_eItemType = tItemDesc.eType;
		m_strItemName = tItemDesc.strItemName;
	}
	else
	{
		//테스트용
		m_eItemType = ITEM_TYPE::AMMO;
		m_strItemName = L"Magnum_Ammo_Small";
	}


	if (m_eItemType == ITEM_TYPE::STAT || (m_strItemName.find(L"Staff") != _wstring::npos&& m_eItemType == ITEM_TYPE::AMMO))
	{
		m_bIsNeedAnim = true;
	}

	m_vDropVelocity.x = ((rand() % 100) / 100.f - 0.5f) * 5.f;  // -2.5 ~ 2.5
	m_vDropVelocity.z = ((rand() % 100) / 100.f - 0.5f) * 5.f;
	m_vDropVelocity.y = 2.f;

	return S_OK;
}

void CItem::Priority_Update(_float fTimeDelta)
{
	//if (m_pPlayer == nullptr)
	//{
	//	m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Player")));

	//}
}

void CItem::Update(_float fTimeDelta)
{
	m_pColliderCom->Set_WorldMat(m_pTransformCom->Get_WorldMat());
	if(nullptr != m_pParticleCom) 
		m_pParticleCom->Update(fTimeDelta);
	if (m_bIsCubeCollider)
	{
		(m_pColliderCom)->Update_Collider(TEXT("Com_Collider_Cube"), m_pTransformCom->Compute_Scaled());
	}
	m_pGameInstance->Add_Collider(CG_ITEM, m_pColliderCom);

	if (m_bIsDropping)
	{
		Update_Drop(fTimeDelta);
	}
	else
	{
		Float_Item(fTimeDelta);
	}
	m_pSoundCom->Update(fTimeDelta);
}

void CItem::Late_Update(_float fTimeDelta)
{
	Billboarding(fTimeDelta);
	m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
	if(nullptr != m_pLightCom)
		m_pGameInstance->Add_Light(m_pLightCom);

	if (m_bIsNeedAnim)
	{
		Play_Animation(fTimeDelta);
	}
}

HRESULT CItem::Render()
{
	if (FAILED(m_pTextureCom->Bind_Resource(m_mapTextureTag[m_eItemType][m_strItemName])))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	SetUp_RenderState();
	if (FAILED(m_pShaderCom->Bind_Texture(m_pTextureCom, m_mapTextureTag[m_eItemType][m_strItemName])))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Transform()))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Material(m_pMaterialCom)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Lights()))
		return E_FAIL;

	SetUp_RenderState();
	m_pShaderCom->Begin(1);
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;
	if(nullptr != m_pParticleCom)
	{
		if (FAILED(m_pParticleCom->Render()))
			return E_FAIL;
	}
	m_pShaderCom->End();
	Release_RenderState();
	return S_OK;
}

void CItem::Billboarding(_float fTimeDelta)
{
	if (!m_pPlayer) return;


	CTransform* pPlayerTransform = static_cast<CPlayer*>(m_pPlayer)->Get_TransForm();

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

HRESULT CItem::On_Collision(CCollisionObject* other)
{
	if (!other || other->Get_Type() != COLLIDERGROUP::CG_PLAYER) return E_FAIL;

	Use_Item();

	return S_OK;
}

void CItem::Use_Item()
{
	if (!m_pPlayer) return;

	_int iAmount = 10; // 기본값

	if (m_strItemName.find(L"Big") != std::wstring::npos)
		iAmount = 20  ;
	if (m_strItemName.find(L"Minigun_Ammo") != std::wstring::npos)
		iAmount = 20;

	// HP Big 판별용
	_int iHpAmount = 10;
	if (m_strItemName.find(L"HP_Big") != std::wstring::npos)
		iHpAmount = 20;

	switch (m_eItemType)
	{
	case Client::CItem::ITEM_TYPE::HP:
		m_pPlayer->Add_HP(iHpAmount);
		m_pSoundCom->Play_Event(L"event:/Objects/potion")->SetVolume(0.5f);
		break;

	case Client::CItem::ITEM_TYPE::MP:
		m_pPlayer->Add_MP(iAmount);
		m_pSoundCom->Play_Event(L"event:/Objects/potion")->SetVolume(0.5f);
		break;

	case Client::CItem::ITEM_TYPE::AMMO:
		m_pPlayer->Add_Ammo(m_strItemName, iAmount);
		m_pSoundCom->Play_Event(L"event:/Objects/ammo_pickup")->SetVolume(0.5f);
		if (auto pUI_Event = dynamic_cast<CUI_Event*>(CUI_Manager::GetInstance()->GetUI(L"UI_Event")))
		{
			pUI_Event->ShowEventText(iAmount, m_strItemName);
		}
		break;

	case Client::CItem::ITEM_TYPE::STAT:
		m_pPlayer->Add_SkillPoint(5);
		m_pSoundCom->Play_Event(L"event:/Objects/Keys_Pickup")->SetVolume(0.5f);
		if (auto pUI_Event = dynamic_cast<CUI_Event*>(CUI_Manager::GetInstance()->GetUI(L"UI_Event")))
		{
			pUI_Event->ShowEventText(5, m_strItemName);
		}
		break;

	case Client::CItem::ITEM_TYPE::KEY:
		m_pSoundCom->Play_Event(L"event:/Objects/Keys_Pickup")->SetVolume(0.5f);
		if (FAILED(m_pPlayer->Add_Item(m_strItemName)))
			return;
		if (auto pUI_Event = dynamic_cast<CUI_Event*>(CUI_Manager::GetInstance()->GetUI(L"UI_Event")))
		{
			pUI_Event->ShowEventText(1, m_strItemName);
		}
		break;

	case Client::CItem::ITEM_TYPE::WEAPON:
		////event:/Objects/General_Weapon_Pickup_2
		m_pSoundCom->Play_Event(L"event:/Objects/General_Weapon_Pickup_2")->SetVolume(3.f);
		if (m_pPlayer->Has_Item(m_strItemName))
		{
			m_pPlayer->Add_Ammo(m_strItemName, 20);
		}
		else
		{
			m_pPlayer->Add_Weapon(m_strItemName);
			if (auto pUI_Event = dynamic_cast<CUI_Event*>(CUI_Manager::GetInstance()->GetUI(L"UI_Event")))
			{
				pUI_Event->ShowEventText(1, m_strItemName);
			}
		}
		break;

	case Client::CItem::ITEM_TYPE::MAX:
	default:
		break;
	}

	SetActive(false);
}


void CItem::Init_TextureTag()
{
	m_mapTextureTag[ITEM_TYPE::HP][L"HP_Big"] = 36;
	m_mapTextureTag[ITEM_TYPE::HP][L"HP_Small"] = 37;

	m_mapTextureTag[ITEM_TYPE::MP][L"MP_Big"] = 38;
	m_mapTextureTag[ITEM_TYPE::MP][L"MP_Small"] = 39;

	m_mapTextureTag[ITEM_TYPE::AMMO][L"Magnum_Ammo_Big"] = 2;
	m_mapTextureTag[ITEM_TYPE::AMMO][L"Magnum_Ammo_Small"] = 86;

	m_mapTextureTag[ITEM_TYPE::AMMO][L"Minigun_Ammo"] = 3;

	m_mapTextureTag[ITEM_TYPE::AMMO][L"ShotGun_Ammo_Small"] = 87;
	m_mapTextureTag[ITEM_TYPE::AMMO][L"ShotGun_Ammo_Big"] = 88;

	m_mapTextureTag[ITEM_TYPE::AMMO][L"Staff_Ammo_Big"] = 93;
	m_mapTextureTag[ITEM_TYPE::AMMO][L"Staff_Ammo_Small"] = 89;

	m_mapTextureTag[ITEM_TYPE::EXP][L"EXP"] = 3;
	m_mapTextureTag[ITEM_TYPE::STAT][L"Stat"] = 78;

	m_mapTextureTag[ITEM_TYPE::KEY][L"Red"] = 61;
	m_mapTextureTag[ITEM_TYPE::KEY][L"Blue"] = 60;
	m_mapTextureTag[ITEM_TYPE::KEY][L"Yellow"] = 62;

	m_mapTextureTag[ITEM_TYPE::WEAPON][L"ShotGun"] = 97;
	m_mapTextureTag[ITEM_TYPE::WEAPON][L"Staff"] = 98;
	m_mapTextureTag[ITEM_TYPE::WEAPON][L"Minigun"] = 99;
}

void CItem::Play_Animation(_float fTimeDelta) 
{
	m_fFrame += Item::ANIMATION_FRAME_RATE * fTimeDelta;
	if (m_fFrame >= Item::ANIMATION_FRAME_RATE)
		m_fFrame = 0.f;

	_float fSlice = 90.f / static_cast<_float>(Get_Max_AnimNum() - Get_Base_AnimNum() + 1);
	m_iCurrentTexture = (_uint)(m_fFrame / fSlice) + Get_Base_AnimNum();
	m_iCurrentTexture = min(Get_Max_AnimNum(), m_iCurrentTexture);
	m_mapTextureTag[m_eItemType][m_strItemName] = m_iCurrentTexture;
}

void CItem::Float_Item(_float fTimeDelta)
{

	_float fOffsetY = (m_bIsUp ? 1.f: -1.f) * cos(D3DXToRadian(Item::FLOAT_FREQUENCY));
	auto vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	auto vPrevPosition = vPosition;
	vPosition.y += fOffsetY * fTimeDelta;

	if (vPosition.y > Item::MAX_FLOAT_OFFSET && m_bIsUp)
	{
		m_bIsUp = false;
	}
	else if (vPosition.y < Item::MIN_FLOAT_OFFSET)
	{
		m_bIsUp = true;
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
}

_uint CItem::Get_Base_AnimNum()
{
	if (m_strItemName == L"Staff_Ammo_Small")
		return 89;
	else if (m_strItemName == L"Staff_Ammo_Big")
		return 93;

	return 78;
}

_uint CItem::Get_Max_AnimNum()
{
	if (m_strItemName == L"Staff_Ammo_Small")
		return 92;
	else if (m_strItemName == L"Staff_Ammo_Big")
		return 96;

	return 85;
}



HRESULT CItem::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200);
	_float2 ScaleFactor = { 1.0f, 1.0f };
	_float2 Offset = { 0.f, 0.f };
	m_pShaderCom->Set_UVScaleFactor(&ScaleFactor);
	m_pShaderCom->Set_UVOffsetFactor(&Offset);
	return S_OK;
}

HRESULT CItem::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}

void CItem::Update_Drop(_float fTimeDelta)
{
	const _float GROUND_Y = 0.6f;

	// 중력 적용: 속도에 중력 가속도 반영
	m_vDropVelocity.y += -GRAVITY * fTimeDelta;

	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos += m_vDropVelocity * fTimeDelta;

	// 바닥에 닿으면 튕기는 효과 
	if (vPos.y < GROUND_Y)
	{
		vPos.y = GROUND_Y;
		// Y축 속도 반전 + 감쇠 
		m_vDropVelocity.y = -m_vDropVelocity.y * 0.5f;

		// X, Z 축 속도도 감쇠 처리
		m_vDropVelocity.x *= 0.5f;
		m_vDropVelocity.z *= 0.5f;

		if (abs(m_vDropVelocity.y) < 0.2f)
		{
			m_bIsDropping = false;
			//m_vDropVelocity = _float3(0.f, 0.f, 0.f);
		}
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
}

HRESULT CItem::Ready_Components()
{

	/* Transform Component */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* VIBuffer Component */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Items"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	CCollider_Cube::COL_CUBE_DESC	ColliderDesc = {};
	ColliderDesc.pOwner = this;

	ColliderDesc.fScale = { 1.f, 1.f, 1.f };
	ColliderDesc.fLocalPos = { 0.f, 0.f, 0.f };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	if (m_eItemType == ITEM_TYPE::EXP)
	{
		CGold_Particle_System::GOLDDESC goldDesc = {};
		goldDesc.Bounding_Box.m_vMin = { -1, -1, 0 };
		goldDesc.Bounding_Box.m_vMax = { 1, 1, 0 };
		goldDesc.iNumParticles = { 10 };
		goldDesc.strTexturePath = L"../../Resources/Textures/Particle/particle_gold.png";
		goldDesc.iNumTextures = { 1 };

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Particle_Gold"),
			TEXT("Com_Particle"), reinterpret_cast<CComponent**>(&m_pParticleCom), &goldDesc)))
			return E_FAIL;

		CLight::LIGHT_INIT lightDesc = { L"../../Resources/Lights/Gold.json" };

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Light"),
			TEXT("Com_Particle"), reinterpret_cast<CComponent**>(&m_pParticleCom), &lightDesc)))
			return E_FAIL;
	}
	m_eType = CG_ITEM;
	/* For.Com_Material */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Material"),
		TEXT("Com_Material"), reinterpret_cast<CComponent**>(&m_pMaterialCom))))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_BaseShader"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound_Source"),
		TEXT("Com_Sound_Source"), reinterpret_cast<CComponent**>(&m_pSoundCom))))
		return E_FAIL;

	return S_OK;
}

json CItem::Serialize()
{
	json json = CGameObject::Serialize();

	json["Type"] = static_cast<_int>(m_eItemType);
	json["Item_Name"] = ISerializable::WideToUtf8(m_strItemName);

	auto pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	auto scale = m_pTransformCom->Compute_Scaled();
	auto angle = m_pTransformCom->Get_EulerAngles();

	auto round2 = [](float val) { return round(val * 100) / 100; };

	json["position"] = {
	round2(pos.x),
	round2(pos.y),
	round2(pos.z)
	};
	json["rotation"] = {
		round2(angle.x),
		round2(angle.y),
		round2(angle.z)
	};				  
	json["scale"] = { 
		round2(scale.x),
		round2(scale.y),
		round2(scale.z)
	};

	return json;
}

void CItem::Deserialize(const json& j)
{
	SET_TRANSFORM(j, m_pTransformCom);
	auto scale = _float3(j["scale"][0], j["scale"][1], j["scale"][2]);

	m_eItemType = static_cast<ITEM_TYPE>(j["Type"].get<_int>());

	m_strItemName = ISerializable::Utf8ToWide(j["Item_Name"].get<string>());

	if (m_eItemType == ITEM_TYPE::STAT)
	{
		m_bIsNeedAnim = true;
	}
	else
	{
		m_bIsNeedAnim = false;
	}
}

CItem* CItem::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CItem* pInstance = new CItem(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CItem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CItem::Clone(void* pArg)
{
	CItem* pInstance = new CItem(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CItem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItem::Free()
{
	__super::Free();
	Safe_Release(m_pSoundCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pMaterialCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pParticleCom);
}




