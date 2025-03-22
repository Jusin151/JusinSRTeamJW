#include "PointShop.h"
#include "GameInstance.h"
#include "Player.h"  // 플레이어 정보 접근용

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
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CPointShop::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    // 포인트 상점 전용 텍스처 컴포넌트 추가
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Point_Shop_UI"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    // 상점 위치 및 크기 설정
    m_pTransformCom->Set_Scale(804.f, 482.f, 1.0f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(0.f, 0.f, 0.f));

    return S_OK;
}

HRESULT CPointShop::Open_Shop()
{
    // 이미 열려있다면 무시
    if (m_bIsOpen)
        return S_OK;

    m_bIsOpen = true;

    // 상점 버튼 활성화 (기존 Ready_Layer_Point_Shop_Button 함수 호출 대신)
    for (int i = 0; i < 12; ++i)
    {
        // 일반 아이템 버튼 활성화
        CGameObject* pButton = m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_Point_Shop_Selected"));
        if (pButton)
            pButton->SetActive(true);
    }

    for (int i = 0; i < 4; ++i)
    {
        // 특별 아이템 버튼 활성화
        CGameObject* pSmallButton = m_pGameInstance->Find_Object(LEVEL_GAMEPLAY,
            TEXT("Layer_Point_Shop_Selected") + to_wstring(i));
        if (pSmallButton)
            pSmallButton->SetActive(true);
    }

    // 포인트 상점 디스플레이 활성화
    CGameObject* pDisplay = m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_Point_Shop_Display"));
    if (pDisplay)
        pDisplay->SetActive(true);

    // 상점 아이템 새로고침
    Refresh_Shop_Items();

    return S_OK;
}

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
    return E_NOTIMPL;
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

HRESULT CPointShop::Ready_ShopItems()
{
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
}