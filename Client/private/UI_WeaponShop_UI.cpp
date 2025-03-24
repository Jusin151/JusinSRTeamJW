#include "UI_WeaponShop_UI.h"

CUI_WeaponShop_UI::CUI_WeaponShop_UI(LPDIRECT3DDEVICE9 pGraphic_Device)
    :CUI_Shop_Base{pGraphic_Device}
{
}

CUI_WeaponShop_UI::CUI_WeaponShop_UI(const CUI_WeaponShop_UI& Prototype)
    :CUI_Shop_Base(Prototype)
{
}

HRESULT CUI_WeaponShop_UI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_WeaponShop_UI::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) // 고정 컴포넌트만 부모에서
		return E_FAIL;

	if (FAILED(Ready_Texture()))
		return E_FAIL;

	return S_OK;
}

void CUI_WeaponShop_UI::Priority_Update(_float fTimeDelta)
{

}

void CUI_WeaponShop_UI::Update(_float fTimeDelta)
{

}

void CUI_WeaponShop_UI::Late_Update(_float fTimeDelta)
{

}

HRESULT CUI_WeaponShop_UI::Ready_Texture()
{

    return S_OK;
}

CUI_WeaponShop_UI* CUI_WeaponShop_UI::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CUI_WeaponShop_UI* pInstance = new CUI_WeaponShop_UI(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Point_Shop");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_WeaponShop_UI::Clone(void* pArg)
{
	CUI_WeaponShop_UI* pInstance = new CUI_WeaponShop_UI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_WeaponShop_UI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_WeaponShop_UI::Free()
{
	__super::Free();
}

HRESULT CUI_WeaponShop_UI::Register_Buttons()
{

	return S_OK;
}
