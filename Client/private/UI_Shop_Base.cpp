#include "UI_Shop_Base.h"
#include "GameInstance.h"

CUI_Shop_Base::CUI_Shop_Base(LPDIRECT3DDEVICE9 pGraphic_Device)
    :CUI_Base(pGraphic_Device)
{
}

CUI_Shop_Base::CUI_Shop_Base(const CUI_Shop_Base& Prototype)
    :CUI_Base(Prototype)
{
}

void CUI_Shop_Base::Font_Render()
{
}

void CUI_Shop_Base::Stat_Render()
{
}

void CUI_Shop_Base::Button_TexT(_int CurrentButtonType)
{
}

_bool CUI_Shop_Base::isPick(HWND hWnd)
{
    POINT ptMouse{};
    GetCursorPos(&ptMouse);
    ScreenToClient(hWnd, &ptMouse);
     

    RECT rcClient;
    GetClientRect(hWnd, &rcClient);
    LONG screenWidth = rcClient.right - rcClient.left;
    LONG screenHeight = rcClient.bottom - rcClient.top;

    // 내 코드는 화면 중앙의 0,0을 기준으로 하는게 더 편한거 같아서
    // 마우스커서의 좌표도 0,0으로 바꿔준거임
    _float2 vMousePos =
    {
        static_cast<_float>(ptMouse.x - screenWidth / 2),
        static_cast<_float>(screenHeight / 2 - ptMouse.y)
    };

    RECT rcUI =
    {
       _long(m_vPos.x - m_vSize.x * 0.5f),
       _long(m_vPos.y - m_vSize.y * 0.5f),
       _long(m_vPos.x + m_vSize.x * 0.5f),
       _long(m_vPos.y + m_vSize.y * 0.5f)
    };

    return PtInRect(&rcUI, POINT{ static_cast<LONG>(vMousePos.x), static_cast<LONG>(vMousePos.y) });
}

HRESULT CUI_Shop_Base::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Shop_Base::Initialize(void* pArg)
{
    if (FAILED(Ready_Components()))
        return E_FAIL;


    return S_OK;
}

void CUI_Shop_Base::Priority_Update(_float fTimeDelta)
{
}

void CUI_Shop_Base::Update(_float fTimeDelta)
{
}

void CUI_Shop_Base::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI_BACKGROUND, this)))
        return; 
}

HRESULT CUI_Shop_Base::Render()
{
    D3DXMATRIX matOldView, matOldProj;
    m_pGraphic_Device->GetTransform(D3DTS_VIEW, &matOldView);
    m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &matOldProj);

    D3DXMATRIX matView;
    D3DXMatrixIdentity(&matView);
    m_pGraphic_Device->SetTransform(D3DTS_VIEW, &matView);

    D3DXMATRIX matProj;
    D3DXMatrixOrthoLH(&matProj, g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);
    m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &matProj);

    m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);


    if (FAILED(m_pTransformCom->Bind_Resource()))
        return E_FAIL;
    if (FAILED(m_pTextureCom->Bind_Resource(0)))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_pGraphic_Device->SetTransform(D3DTS_VIEW, &matOldView);
    m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &matOldProj);
    m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);

    return S_OK;
}

HRESULT CUI_Shop_Base::Register_Buttons()
{

    return S_OK;
}

void CUI_Shop_Base::Open_ShopUI()
{
}

void CUI_Shop_Base::Close_ShopUI()
{
}

HRESULT CUI_Shop_Base::Ready_Components()
{
    /* Transform Component */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
        return E_FAIL;

    /* VIBuffer Component */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
        return E_FAIL;

    return S_OK;
}

CGameObject* CUI_Shop_Base::Clone(void* pArg)
{
    return nullptr;
}

void CUI_Shop_Base::Free()
{
    __super::Free();


    Safe_Release(m_pTransformCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);

}

