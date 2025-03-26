#include "PointShop.h"
#include "GameInstance.h"
#include "Player.h"  
#include  "UI_Point_Shop.h"
#include "CUI_Manager.h"

CPointShop::CPointShop(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CShop(pGraphic_Device)
{
}

CPointShop::CPointShop(const CPointShop& Prototype)
    : CShop(Prototype)
{
}

HRESULT CPointShop::Initialize_Prototype()
{


    return S_OK;
}

HRESULT CPointShop::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(0.f, 0.6f, -3.4f));

    m_pTransformCom->Set_Scale(1.5f, 1.5f, 2.f);

    m_pUI_PointShop = static_cast<CUI_Point_Shop*>(CUI_Manager::GetInstance()->GetUI(L"Point_Shop_UI"));

    return S_OK;
}


void CPointShop::Priority_Update(_float fTimeDelta)
{
    m_bIsActive = true;
}

void CPointShop::Update(_float fTimeDelta)
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

void CPointShop::Late_Update(_float fTimeDelta)
{

    __super::Late_Update(fTimeDelta);
}

HRESULT CPointShop::SetUp_RenderState()
{
    // 일단 추가해보기

    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); // 알파 값이 기준보다 크면 픽셀 렌더링
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200); // 기준값 설정 (0~255)

    return S_OK;
}

HRESULT CPointShop::Release_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    return S_OK;
}

HRESULT CPointShop::Render()
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

HRESULT CPointShop::Ready_ShopItems()
{


    return S_OK;
}

HRESULT CPointShop::Open_Shop()
{
    //// 이미 열려있다면 무시
    //if (m_bIsOpen)
    //    return S_OK;

   // m_bIsOpen = true;


    //m_pUI_PointShop = static_cast<CUI_Point_Shop*>(CUI_Manager::GetInstance()->GetUI(L"Point_Shop_UI")); 

    if (m_pUI_PointShop)
    {
        m_pUI_PointShop->SetActive(true); // 보이게 설정
        m_pUI_PointShop->Button_Set_Active(true);
        m_pUI_PointShop->m_bOnUI = true;
    }


    // 상점 아이템 새로고침
    Refresh_Shop_Items();

    return S_OK;
}//뭘봐 ㅋ

HRESULT CPointShop::Close_Shop()
{
    // 이미 닫혀있다면 무시


    //m_pUI_PointShop = static_cast<CUI_Point_Shop*>(CUI_Manager::GetInstance()->GetUI(L"Point_Shop_UI"));

    if (m_pUI_PointShop)
    {
        m_pUI_PointShop->SetActive(false);
        m_pUI_PointShop->Button_Set_Active(false);
        m_pUI_PointShop->m_bOnUI = false;
    }



    return S_OK;
}

HRESULT CPointShop::Purchase_Item(const _uint iItemID, const _uint iCount)
{
    // 상점이 닫혀있다면 구매 실패
    if (!m_bIsOpen)
        return E_FAIL;

    // 구매 가능 여부 확인
    if (!Can_Purchase(iItemID, iCount))
        return E_FAIL;
    return S_OK;
}

HRESULT CPointShop::Sell_Item(const _uint iItemID, const _uint iCount)
{
    return S_OK;
}

void CPointShop::Refresh_Shop_Items()
{


}

_bool CPointShop::Can_Purchase(_uint iItemID, _uint iCount)
{
    // 상점이 닫혀있으면 구매 불가
    if (!m_bIsOpen)
        return false;

    return true;
}


HRESULT CPointShop::Ready_Components()
{

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Weapon_Shop"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;


    return S_OK;
}

CPointShop* CPointShop::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CPointShop* pInstance = new CPointShop(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CPointShop");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPointShop::Clone(void* pArg)
{
    CPointShop* pInstance = new CPointShop(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CPointShop");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPointShop::Free()
{
    __super::Free();
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pTransformCom);
    Safe_Release(m_pVIBufferCom);

}

