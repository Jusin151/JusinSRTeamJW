#include "Item_Icon.h"
#include "GameInstance.h"


CItem_Icon::CItem_Icon(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CItem_Icon::CItem_Icon(const CItem_Icon& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CItem_Icon::Initialize_Prototype()
{
	if (FAILED(Ready_Components()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CItem_Icon::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))

		return E_FAIL;



	m_pTransformCom->Set_Scale(m_Icon_INFO.vSize.x, m_Icon_INFO.vSize.y, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_Icon_INFO.vPos.x, m_Icon_INFO.vPos.y, 0.f));
	return S_OK;
}

void CItem_Icon::Priority_Update(_float fTimeDelta)
{
}

void CItem_Icon::Update(_float fTimeDelta)
{

}

void CItem_Icon::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this)))
		return;
}


HRESULT CItem_Icon::Render()
{

	return __super::Render();

}

HRESULT CItem_Icon::Ready_Components()
{

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Hp_Bar"),
		TEXT("Com_Texture_HP"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_HP"),
		TEXT("Com_VIBuffer_HP"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform_HP"), reinterpret_cast<CComponent**>(&m_pTransformCom), &tDesc)))
		return E_FAIL;

	return S_OK;
}


CItem_Icon* CItem_Icon::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CItem_Icon* pInstance = new CItem_Icon(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("체력바 UI 원본 생성 실패 ");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CItem_Icon::Clone(void* pArg)
{
	CItem_Icon* pInstace = new CItem_Icon(*this);

	if (FAILED(pInstace->Initialize(pArg)))
	{
		MSG_BOX("체력바 UI 복제 실패");
		Safe_Release(pInstace);
	}

	return pInstace;
}

void CItem_Icon::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
}
