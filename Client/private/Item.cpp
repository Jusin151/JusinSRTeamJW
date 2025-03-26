
﻿
#include "Item.h"
#include "GameInstance.h"
#include "Collider_Sphere.h"
#include "Collider_Cube.h"
#include "Player.h"

CItem::CItem(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CCollisionObject{ pGraphic_Device }
{
}

CItem::CItem(const CItem& Prototype)

	:CCollisionObject{ Prototype },
	m_mapTextureTag{ Prototype.m_mapTextureTag }

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


	m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_Player"))); 
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(0.f, 0.6f, 0.f));


	return S_OK;
}

void CItem::Priority_Update(_float fTimeDelta)
{
}

void CItem::Update(_float fTimeDelta)
{
	m_pColliderCom->Set_WorldMat(m_pTransformCom->Get_WorldMat());

	if (m_bIsCubeCollider)
	{
		(m_pColliderCom)->Update_Collider(TEXT("Com_Collider_Cube"), m_pTransformCom->Compute_Scaled());
	}
	m_pGameInstance->Add_Collider(CG_ITEM, m_pColliderCom);



	static _float testOffset = 0.f;
	static _float fIsUp = 1.f;
	static _bool bFlag = false;
	_float fOffsetY = fIsUp * cos(D3DXToRadian(60.f));
 	testOffset += fOffsetY * fTimeDelta;

	if (testOffset > 1.5f&& !bFlag)
	{
		fIsUp = -fIsUp;
		bFlag = true;
	}
	else if (testOffset < 0.f)
	{
		fIsUp = -fIsUp;
		bFlag = false;
	}

	auto vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPosition.y = testOffset;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

}

void CItem::Late_Update(_float fTimeDelta)
{
	Billboarding(fTimeDelta);
	m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CItem::Render()
{


	if (FAILED(m_pTextureCom->Bind_Resource(m_mapTextureTag[ITEM_TYPE::HP][L"HP_Big"])))

		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	SetUp_RenderState();
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	Release_RenderState();

	m_pColliderCom->Render();
	return S_OK;
}

HRESULT CItem::On_Collision(_float fTimeDelta)
{
	if (nullptr == m_pColliderCom)
		return E_FAIL;


	return E_FAIL;
}

void CItem::Billboarding(_float fTimeDelta)
{

	//_float4x4 vViewMatrix{};
	//_float3 vScale = m_pTransformCom->Compute_Scaled();
	//m_pGraphic_Device->GetTransform(D3DTS_VIEW, &vViewMatrix);
	//m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *reinterpret_cast<_float3*>(&vViewMatrix.m[0][0]) );
	//m_pTransformCom->Set_State(CTransform::STATE_UP, *reinterpret_cast<_float3*>(&vViewMatrix.m[1][0]) );
	//m_pTransformCom->Set_State(CTransform::STATE_LOOK, *reinterpret_cast<_float3*>(&vViewMatrix.m[2][0]) * vScale.z);

	//_float3 vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//vPosition.y = 0.5f;
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	CTransform* pPlayerTransform = static_cast<CPlayer*>(m_pPlayer)->Get_TransForm();

	// �÷��̾��� look ���� �������� (�÷��̾ �ٶ󺸴� ����)
	_float3 vPlayerLook = pPlayerTransform->Get_State(CTransform::STATE_LOOK);

	vPlayerLook.y = 0.f;
	vPlayerLook.Normalize();

	_float3 vShopLook = -vPlayerLook;  // ���� ���� ����

	// ���� ������ ����Ͽ� Ʈ������ ����
	_float3 vUp = _float3(0.0f, 1.0f, 0.0f);  // ���� �� ����
	_float3 vRight = vUp.Cross(vShopLook);
	vRight.Normalize();

	// ���� ������ �����ϱ� ���� �� ���� ����
	_float3 vNewUp = vShopLook.Cross(vRight);
	vNewUp.Normalize();

	// ������ ȸ�� ��� ����
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vNewUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vShopLook);

}

void CItem::Bind_ResourceByType()
{
}

void CItem::Use_Item()
{
	switch (m_eType)
	{
	case Client::CItem::ITEM_TYPE::HP:
		break;
	case Client::CItem::ITEM_TYPE::MP:
		break;
	case Client::CItem::ITEM_TYPE::AMMO:
		break;
	case Client::CItem::ITEM_TYPE::EXP:
		break;
	case Client::CItem::ITEM_TYPE::STAT:
		break;
	case Client::CItem::ITEM_TYPE::MAX:
		break;
	default:
		break;
	}
}


void CItem::Init_TextureTag()
{
	m_mapTextureTag[ITEM_TYPE::HP][L"HP_Big"] = 36;
	m_mapTextureTag[ITEM_TYPE::HP][L"HP_Small"] = 37;
	m_mapTextureTag[ITEM_TYPE::MP][L"MP_Big"] = 38;
	m_mapTextureTag[ITEM_TYPE::MP][L"MP_Small"] = 39;
	m_mapTextureTag[ITEM_TYPE::AMMO][L"Pistol"] = 2;
	m_mapTextureTag[ITEM_TYPE::EXP][L"EXP"] = 3;
	m_mapTextureTag[ITEM_TYPE::STAT][L"STAT"] = 80;
}



HRESULT CItem::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); 
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200);

	return S_OK;
}

HRESULT CItem::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
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


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Items"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	CCollider_Cube::COL_CUBE_DESC	ColliderDesc = {};
	ColliderDesc.pOwner = this;

	ColliderDesc.fScale = { 1.f, 1.f, 1.f };
	ColliderDesc.fLocalPos = { 0.f, 0.f, 0.f };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;


	return S_OK;
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
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pMaterialCom);
}

json CItem::Serialize()
{
	return json();
}

void CItem::Deserialize(const json& j)
{
}

HRESULT CItem::On_Collision(CCollisionObject* other)
{
	return E_NOTIMPL;
}
