#include "GamePlay_Button.h"
#include "GameInstance.h"
#include "CUI_Manager.h"

CGamePlay_Button::CGamePlay_Button(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CUI_Base(pGraphic_Device) {
}

CGamePlay_Button::CGamePlay_Button(const CGamePlay_Button& rhs)
    : CUI_Base(rhs),
    m_pTextureCom(rhs.m_pTextureCom),
    m_pVIBufferCom(rhs.m_pVIBufferCom),
    m_pTransformCom(rhs.m_pTransformCom) {
}

HRESULT CGamePlay_Button::Initialize_Prototype() {
    return S_OK;
}

HRESULT CGamePlay_Button::Initialize(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    m_Button_Info = *reinterpret_cast<GamePlayer_Button_Desc*>(pArg);
    Set_Position(m_Button_Info.Button_Desc.vPos);
    Set_Size(m_Button_Info.Button_Desc.vSize);

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->Set_Scale(m_Button_Info.Button_Desc.vSize.x, m_Button_Info.Button_Desc.vSize.y, 1.f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_Button_Info.Button_Desc.vPos.x,
        m_Button_Info.Button_Desc.vPos.y, 0.f));

    return S_OK;
}

void CGamePlay_Button::Priority_Update(_float fTimeDelta)
{
}

void CGamePlay_Button::Update(_float fTimeDelta)
{
    if (isPick(g_hWnd))
    {
        if (m_OnMouse)  
            m_OnMouse();

        if (GetKeyState(VK_LBUTTON) & 0x8000)
        {
            if (m_OnClick) m_OnClick();
        }
    }
    else 
    { 
        m_strMouseOnText.clear();
    }
}

void CGamePlay_Button::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this)))
        return;
}

HRESULT CGamePlay_Button::Render()
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

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    if (FAILED(m_pTransformCom->Bind_Resource())) return E_FAIL;
    if (FAILED(m_pTextureCom->Bind_Resource(0))) return E_FAIL;
    if (FAILED(m_pVIBufferCom->Bind_Buffers())) return E_FAIL;
    if (FAILED(m_pVIBufferCom->Render())) return E_FAIL;

    if (m_OnMouse && !m_strMouseOnText.empty())
    { 
        m_pGameInstance->Render_Font_Size(
            L"MainFont",
            m_strMouseOnText,
            _float2(-20.f,100.f),
            _float2(12.f, 20.f),
            _float3(1.f, 1.f, 0.f));
    }

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_pGraphic_Device->SetTransform(D3DTS_VIEW, &matOldView);
    m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &matOldProj);

    return S_OK;
}

HRESULT CGamePlay_Button::Ready_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Button_Point_Shop"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
        return E_FAIL;

    return S_OK;
}

_bool CGamePlay_Button::isPick(HWND hWnd)
{
    POINT ptMouse;
    GetCursorPos(&ptMouse);
    ScreenToClient(hWnd, &ptMouse);

    RECT rcClient;
    GetClientRect(hWnd, &rcClient);
    LONG screenWidth = rcClient.right - rcClient.left;
    LONG screenHeight = rcClient.bottom - rcClient.top;

    _float2 vMousePos = {
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

    return PtInRect(&rcUI, POINT{ LONG(vMousePos.x), LONG(vMousePos.y) });
}

CGamePlay_Button* CGamePlay_Button::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CGamePlay_Button* pInstance = new CGamePlay_Button(pGraphic_Device);
    if (FAILED(pInstance->Initialize_Prototype())) 
    {
        MSG_BOX("GamePlay_Button Create Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CGamePlay_Button::Clone(void* pArg)
{
    CGamePlay_Button* pInstance = new CGamePlay_Button(*this);
    if (FAILED(pInstance->Initialize(pArg))) 
    {
        MSG_BOX("GamePlay_Button Clone Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CGamePlay_Button::Free()
{
    __super::Free();
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTransformCom);
}
