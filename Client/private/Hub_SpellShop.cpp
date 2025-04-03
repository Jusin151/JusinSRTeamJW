#include "Hub_SpellShop.h"
#include "GameInstance.h"
#include "Player.h"  // 플레이어 정보 접근용
#include "UI_Spell_Shop.h"
#include "UI_Manager.h"

CHub_SpellShop::CHub_SpellShop(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CShop(pGraphic_Device)
{
}

CHub_SpellShop::CHub_SpellShop(const CHub_SpellShop& Prototype)
    : CShop(Prototype)
{
}

HRESULT CHub_SpellShop::Initialize_Prototype()
{


    return S_OK;
}

HRESULT CHub_SpellShop::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(-6.5f, 0.6f, 4.2f));

    m_pTransformCom->Set_Scale(1.5f, 1.5f, 2.f);

   // m_pUI_SpellShop = static_cast<CUI_Spell_Shop*>(CUI_Manager::GetInstance()->GetUI(L"Spell_Shop_UI"));

    if (auto pSpellShopUI = dynamic_cast<CObserver*>(CUI_Manager::GetInstance()->GetUI(L"Spell_Shop_UI")))
        Add_Observer(pSpellShopUI);

    return S_OK;
}


void CHub_SpellShop::Priority_Update(_float fTimeDelta)
{
    m_bIsActive = true;
}

void CHub_SpellShop::Update(_float fTimeDelta)
{
   
    __super::Update(fTimeDelta);

    if (m_bIsOpen)
    {
        Open_Shop();
    }
    else
    {
        Close_Shop();
    }
}

void CHub_SpellShop::Late_Update(_float fTimeDelta)
{

    __super::Late_Update(fTimeDelta);
}

HRESULT CHub_SpellShop::SetUp_RenderState()
{
    // 일단 추가해보기

    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); // 알파 값이 기준보다 크면 픽셀 렌더링
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200); // 기준값 설정 (0~255)

    return S_OK;
}

HRESULT CHub_SpellShop::Release_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    return S_OK;
}

HRESULT CHub_SpellShop::Render()
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


    return S_OK;
}

HRESULT CHub_SpellShop::Ready_ShopItems()
{


    return S_OK;
}

HRESULT CHub_SpellShop::Open_Shop()
{
    Notify(nullptr, L"Open");

  
    // 상점 아이템 새로고침
     Refresh_Shop_Items();

    return S_OK;
}//뭘봐 ㅋ

HRESULT CHub_SpellShop::Close_Shop()
{
    Notify(nullptr, L"Close");

    return S_OK;
}

HRESULT CHub_SpellShop::Purchase_Item(const _uint iItemID, const _uint iCount)
{
    // 상점이 닫혀있다면 구매 실패
    if (!m_bIsOpen)
        return E_FAIL;

    // 구매 가능 여부 확인
    if (!Can_Purchase(iItemID, iCount))
        return E_FAIL;
    return S_OK;
}

HRESULT CHub_SpellShop::Sell_Item(const _uint iItemID, const _uint iCount)
{
    return S_OK;
}

void CHub_SpellShop::Refresh_Shop_Items()
{
    
    
}

_bool CHub_SpellShop::Can_Purchase(_uint iItemID, _uint iCount)
{
    // 상점이 닫혀있으면 구매 불가
    if (!m_bIsOpen)
        return false;

    return true;
}


HRESULT CHub_SpellShop::Ready_Components()
{

    if (FAILED(__super::Add_Component(LEVEL_HUB, TEXT("Prototype_Component_Texture_Spell_Shop"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;


    return S_OK;
}

CHub_SpellShop* CHub_SpellShop::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CHub_SpellShop* pInstance = new CHub_SpellShop(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CHub_SpellShop");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CHub_SpellShop::Clone(void* pArg)
{
    CHub_SpellShop* pInstance = new CHub_SpellShop(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CHub_SpellShop");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CHub_SpellShop::Free()
{
    __super::Free();
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pTransformCom);
    Safe_Release(m_pVIBufferCom);

}

