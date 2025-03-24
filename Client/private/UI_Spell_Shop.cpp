#include "UI_Spell_Shop.h"

CUI_Spell_Shop::CUI_Spell_Shop(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CUI_Shop_Base{pGraphic_Device}
{
}

CUI_Spell_Shop::CUI_Spell_Shop(const CUI_Spell_Shop& Prototype)
	: CUI_Shop_Base(Prototype)
{
}

HRESULT CUI_Spell_Shop::Initialize_Prototype()
{
	
	return S_OK;
}

HRESULT CUI_Spell_Shop::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Texture()))
		return E_FAIL; 

	return S_OK;
}

void CUI_Spell_Shop::Priority_Update(_float fTimeDelta)
{
}

void CUI_Spell_Shop::Update(_float fTimeDelta)
{
}

void CUI_Spell_Shop::Late_Update(_float fTimeDelta)
{
}

HRESULT CUI_Spell_Shop::Ready_Texture()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Hp_Bar"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

CUI_Spell_Shop* CUI_Spell_Shop::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CUI_Spell_Shop* pInstance = new CUI_Spell_Shop(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("체력바 UI 원본 생성 실패 ");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Spell_Shop::Clone(void* pArg)
{
	CUI_Spell_Shop* pInstace = new CUI_Spell_Shop(*this);

	if (FAILED(pInstace->Initialize(pArg)))
	{
		MSG_BOX("체력바 UI 복제 실패");
		Safe_Release(pInstace);
	}

	return pInstace;
}

void CUI_Spell_Shop::Free()
{
	__super::Free();


}

HRESULT CUI_Spell_Shop::Register_Buttons()
{

	return S_OK;
}
