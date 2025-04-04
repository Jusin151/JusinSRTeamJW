#include "Hub_Episode.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "UI_Episode_Hub.h"

CHub_Episode::CHub_Episode(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CShop(pGraphic_Device)
{
}

CHub_Episode::CHub_Episode(const CHub_Episode& Prototype)
    : CShop(Prototype)
{
}

HRESULT CHub_Episode::Initialize_Prototype()
{


    return S_OK;
}

HRESULT CHub_Episode::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(10.5f, 0.6f, 0.5f));

    m_pTransformCom->Set_Scale(1.5f, 1.5f, 2.f);


    m_pUI_Episode_Hub = static_cast<CUI_Episode_Hub*>(CUI_Manager::GetInstance()->GetUI(L"Episode_Hub_UI"));

    if (m_pUI_Episode_Hub == nullptr)
        return E_FAIL;

	if (auto pEpisodeHubUI = dynamic_cast<CObserver*>(CUI_Manager::GetInstance()->GetUI(L"Episode_Hub_UI")))
		Add_Observer(pEpisodeHubUI); 
     
 /*   if (m_pUI_Episode_Hub == nullptr)
        return E_FAIL;*/

    return S_OK;
}


void CHub_Episode::Priority_Update(_float fTimeDelta)
{
    m_bIsActive = true;
}

void CHub_Episode::Update(_float fTimeDelta)
{

    __super::Update(fTimeDelta); 

 /*   m_pUI_Episode_Hub->SetActive(false);
    m_pUI_Episode_Hub->Button_Set_Active(false);
    m_pUI_Episode_Hub->m_bOnUI = false;*/

    if (m_bIsOpen)
    {
        Open_Shop();
    }
    else
    {
        Close_Shop();
    }
}

void CHub_Episode::Late_Update(_float fTimeDelta)
{

    __super::Late_Update(fTimeDelta);
}

HRESULT CHub_Episode::Render()
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

HRESULT CHub_Episode::Ready_ShopItems()
{


    return S_OK;
}

HRESULT CHub_Episode::Open_Shop()
{
    //// 이미 열려있다면 무시
    //if (m_bIsOpen)
    //    return S_OK;

   // m_bIsOpen = true;



    if (m_pUI_Episode_Hub)
    {
        m_pUI_Episode_Hub->SetActive(true); // 보이게 설정
        m_pUI_Episode_Hub->Button_Set_Active(true);
        m_pUI_Episode_Hub->Button_Map_Set_Active(true);
        m_pUI_Episode_Hub->m_bOnUI = true;
    }


    // 상점 아이템 새로고침
    Refresh_Shop_Items();

    return S_OK;
}//뭘봐 ㅋ

HRESULT CHub_Episode::Close_Shop()
{
    // 이미 닫혀있다면 무시




    if (m_pUI_Episode_Hub)
    {
    m_pUI_Episode_Hub->SetActive(false);
    m_pUI_Episode_Hub->Button_Set_Active(false);
    m_pUI_Episode_Hub->Button_Map_Set_Active(false);
    m_pUI_Episode_Hub->m_bOnUI = false;
    }



    return S_OK;
}

HRESULT CHub_Episode::Purchase_Item(const _uint iItemID, const _uint iCount)
{
    // 상점이 닫혀있다면 구매 실패
    if (!m_bIsOpen)
        return E_FAIL;

    // 구매 가능 여부 확인
    if (!Can_Purchase(iItemID, iCount))
        return E_FAIL;
    return S_OK;
}

HRESULT CHub_Episode::Sell_Item(const _uint iItemID, const _uint iCount)
{
    return S_OK;
}

void CHub_Episode::Refresh_Shop_Items()
{


}
HRESULT CHub_Episode::SetUp_RenderState()
{
    // 일단 추가해보기

    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); // 알파 값이 기준보다 크면 픽셀 렌더링
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200); // 기준값 설정 (0~255)

    return S_OK;
}

HRESULT CHub_Episode::Release_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    return S_OK;
}

_bool CHub_Episode::Can_Purchase(_uint iItemID, _uint iCount)
{
    // 상점이 닫혀있으면 구매 불가
    if (!m_bIsOpen)
        return false;

    return true;
}


HRESULT CHub_Episode::Ready_Components()
{

    if (FAILED(__super::Add_Component(LEVEL_HUB, TEXT("Prototype_Component_Texture_Episode_Hub"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;


    return S_OK;
}

CHub_Episode* CHub_Episode::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CHub_Episode* pInstance = new CHub_Episode(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CHub_Episode");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CHub_Episode::Clone(void* pArg)
{
    CHub_Episode* pInstance = new CHub_Episode(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CHub_Episode");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CHub_Episode::Free()
{
    __super::Free();
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pTransformCom);
    Safe_Release(m_pVIBufferCom);
   

}

