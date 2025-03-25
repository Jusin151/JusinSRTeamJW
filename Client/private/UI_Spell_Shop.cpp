#include "UI_Spell_Shop.h"
#include "GameInstance.h"
#include "GamePlay_Button.h"



CUI_Spell_Shop::CUI_Spell_Shop(LPDIRECT3DDEVICE9 pGraphic_Device)
    :CUI_Shop_Base(pGraphic_Device)
{
}

CUI_Spell_Shop::CUI_Spell_Shop(const CUI_Spell_Shop& Prototype)
    :CUI_Shop_Base(Prototype)
{
}

HRESULT CUI_Spell_Shop::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Spell_Shop::Initialize(void* pArg)
{

    if (pArg != nullptr)
        m_Shop_INFO = *reinterpret_cast<UI_Desc*>(pArg);
    else
        return E_FAIL;

    m_bOnUI = false;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Texture()))
        return E_FAIL;

    if (FAILED(Register_Buttons()))
        return E_FAIL;

    if (FAILED(Ready_Skill_Button_Text()))
        return E_FAIL;

    if (FAILED(Ready_Stat_Button_Text()))
        return E_FAIL;



    m_pTransformCom->Set_Scale(m_Shop_INFO.vSize.x, m_Shop_INFO.vSize.y, 1.f);

    m_pTransformCom->Set_State(CTransform::STATE_POSITION,
        _float3(m_Shop_INFO.vPos.x, m_Shop_INFO.vPos.y, 0.f));

    return S_OK;
}
HRESULT CUI_Spell_Shop::Register_Buttons()
{
    Create_SkillButton();
    Create_StatButton();

    return S_OK;
}
void CUI_Spell_Shop::Priority_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

void CUI_Spell_Shop::Update(_float fTimeDelta)
{
    m_bIsActive = m_bOnUI;
}

void CUI_Spell_Shop::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);

}
HRESULT CUI_Spell_Shop::Ready_Texture()
{

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Point_Shop"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Spell_Shop::Ready_Skill_Button_Text()
{
    

    return S_OK;
}

HRESULT CUI_Spell_Shop::Ready_Stat_Button_Text()
{
  

    return S_OK;
}


HRESULT CUI_Spell_Shop::Render()
{
  
    return S_OK;
}
void CUI_Spell_Shop::Create_SkillButton() //오른쪽 특성 버튼
{
   
}

void CUI_Spell_Shop::Create_StatButton() // 왼쪽 스탯버튼 
{
    


}

CUI_Spell_Shop* CUI_Spell_Shop::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CUI_Spell_Shop* pInstance = new CUI_Spell_Shop(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CUI_Spell_Shop");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_Spell_Shop::Clone(void* pArg)
{
    CUI_Spell_Shop* pInstance = new CUI_Spell_Shop(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CUI_Spell_Shop");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_Spell_Shop::Free()
{
    __super::Free();
}

