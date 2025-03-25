#include "PointShop.h"
#include "GameInstance.h"
#include "Player.h"  // 플레이어 정보 접근용
#include "UI_Point_Shop.h"

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

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(-7.2f, 0.6f, 1.7f));

    m_pTransformCom->Set_Scale(1.5f, 1.5f, 2.f);


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

    // 상점 버튼 활성화 (기존 Ready_Layer_Point_Shop_Button 함수 호출 대신)
   

    // 포인트 상점 디스플레이 활성화
    CGameObject* pDisplay = m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_UI_Point_Shop"));
    if (pDisplay)
    {
        static_cast<CUI_Point_Shop*>(pDisplay)->SetActive(true);
        static_cast<CUI_Point_Shop*>(pDisplay)->m_bOnUI = true;
    }
    // 상점 아이템 새로고침
     Refresh_Shop_Items();

    return S_OK;
}//뭘봐 ㅋ

HRESULT CPointShop::Close_Shop()
{
    // 이미 닫혀있다면 무시
    if (!m_bIsOpen)
        return S_OK;

    m_bIsOpen = false;

    // 모든 상점 UI 비활성화
    for (int i = 0; i < 12; ++i)
    {
        CGameObject* pButton = m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_Point_Shop_Selected"));
        if (pButton)
            pButton->SetActive(false);
    }

    for (int i = 0; i < 4; ++i)
    {
        CGameObject* pSmallButton = m_pGameInstance->Find_Object(LEVEL_GAMEPLAY,
            TEXT("Layer_Point_Shop_Selected") + to_wstring(i));
        if (pSmallButton)
            pSmallButton->SetActive(false);
    }

    CGameObject* pDisplay = m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_Point_Shop_Display"));
    if (pDisplay)
        pDisplay->SetActive(false);


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

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Point_Shop"),
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

