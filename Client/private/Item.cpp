#include "Item.h"
#include "GameInstance.h"
#include "Collider_Sphere.h"
#include "Collider_Cube.h"
#include "Player.h"

CItem::CItem(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject{ pGraphic_Device }
{
}

CItem::CItem(const CItem& Prototype)
	:CGameObject{ Prototype }
{
}

HRESULT CItem::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CItem::Initialize(void* pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_bIsCubeCollider = (dynamic_cast<CCollider_Cube*>(m_pColliderCom) != nullptr);

	m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_Player"))); // 약한 참조

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
}

void CItem::Late_Update(_float fTimeDelta)
{
	Billboarding(fTimeDelta);
	m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CItem::Render()
{

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

	m_pColliderCom->Render();
	return S_OK;
}

HRESULT CItem::On_Collision(_float fTimeDelta)
{
	if (nullptr == m_pColliderCom)
		return E_FAIL;

	// 안바뀌면 충돌 안일어남
	if (m_pColliderCom->Get_Other_Type() == CG_END)
		return S_OK;

	if (m_pColliderCom->Get_Other_Type() == CG_PLAYER)
	{
		Use_Item();
	}
	// 충돌 처리 하고 다시 type을 수정
	m_pColliderCom->Set_Other_Type(CG_END);
	return E_FAIL;
}

void CItem::Billboarding(_float fTimeDelta)
{
	_float4x4 vViewMatrix{};
	_float3 vScale = m_pTransformCom->Compute_Scaled();
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &vViewMatrix);
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *reinterpret_cast<_float3*>(&vViewMatrix.m[0][0]) * vScale.x);
	m_pTransformCom->Set_State(CTransform::STATE_UP, *reinterpret_cast<_float3*>(&vViewMatrix.m[1][0]) * vScale.y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *reinterpret_cast<_float3*>(&vViewMatrix.m[2][0]) * vScale.z);

	_float3 vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPosition.y = 0.0f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
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

HRESULT CItem::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); // 알파 값이 기준보다 크면 픽셀 렌더링
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200); // 기준값 설정 (0~255)

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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Point_Shop"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
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
