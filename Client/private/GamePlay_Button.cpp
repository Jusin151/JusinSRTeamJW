#include "GamePlay_Button.h"
#include "GameInstance.h"
#include "UI_Manager.h"

CGamePlay_Button::CGamePlay_Button(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CUI_Base(pGraphic_Device) {
}

CGamePlay_Button::CGamePlay_Button(const CGamePlay_Button& Prototype)
    : CUI_Base(Prototype),
    m_pTextureCom(Prototype.m_pTextureCom),
    m_pVIBufferCom(Prototype.m_pVIBufferCom),
    m_pTransformCom(Prototype.m_pTransformCom)
{
}

HRESULT CGamePlay_Button::Initialize_Prototype() 
{
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
    m_bIsActive = m_Button_Info.bActive;

    if (isPick(g_hWnd))
    {
        if (m_OnMouse)  
            m_OnMouse();

        if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
        {
            if (m_OnClick)
                m_OnClick();
            
           // m_strMouseOnText.clear();
            m_bVisible_Click = true;
        }
        m_bVisible = true;
    }
    else 
    { 
        if(!m_bVisible_Click)
        m_strMouseOnText.clear();
     

        m_bVisible = false;
    }


  
}

void CGamePlay_Button::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this)))
        return;
}

HRESULT CGamePlay_Button::Render()
{
  
    Render_ToolTip_Button(); // 마우스 여부와 상관없이 무조건 렌더


    if (!m_Button_Info.bRender)
    {
        if (!m_bVisible)
            return S_OK;
    }
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
    if (FAILED(m_pTextureCom->Bind_Resource(m_Button_Info.iCurrentImageNum))) return E_FAIL;
    if (FAILED(m_pVIBufferCom->Bind_Buffers())) return E_FAIL;
    if (FAILED(m_pVIBufferCom->Render())) return E_FAIL;


    Render_MouseOn_Button_TexT(); //마우스 올리기만했을때 뜨는 텍스트
    Render_MouseClick_Button_TexT(); // 마우스 클릭했을때 뜨는 텍스트

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_pGraphic_Device->SetTransform(D3DTS_VIEW, &matOldView);
    m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &matOldProj);

    return S_OK;
}

void CGamePlay_Button::Render_MouseOn_Button_TexT() // 마우스 올렸을때 뜨는 텍스트들
{
    if (m_OnMouse && !m_strMouseOnText.empty() && m_Button_Info.Button_Type == POINT_SHOP_STAT) //왼쪽 스탯 버튼 
    {
        m_pGameInstance->Render_Font_Size(
            L"MainFont",
            m_strMouseOnText,
            _float2(-360.f,120.f),
            _float2(8.f, 22.f), //글자 크기
            _float3(1.f, 1.f, 0.f)); // R G B
    }
 
  
}

void CGamePlay_Button::Render_MouseClick_Button_TexT() // 마우스 클릭했을때 다음 클릭때까지 계속 남아있는 텍스트들
{

    if (m_OnMouse && !m_strMouseOnText.empty() && m_Button_Info.Button_Type == WEAPON_SHOP_BUTTON) // 오른쪽 스탯 버튼
    {
        m_pGameInstance->Render_Font_Size(
            L"MainFont",
            m_strMouseOnText,
            _float2(135.f, -100.f),
            _float2(8.f, 22.f),
            _float3(1.f, 1.f, 0.f));
   
    }
   
}

void CGamePlay_Button::Render_ToolTip_Button() // 설명창
{
    if (m_OnClick && !m_strMouseOnText.empty() && m_Button_Info.Button_Type == SPELL_SHOP_BUTTON) // 스펠 설명 버튼
    {
        m_pGameInstance->Render_Font_Size(
            L"MainFont",
            m_strMouseOnText,
            _float2(-20.f, -40.f),
            _float2(8.f, 22.f),
            _float3(1.f, 1.f, 0.f));
    }

    if (m_Button_Info.Button_Type == EPISODE_BUTTON_MAP_IMAGE) // 스펠 설명 버튼
    {
        m_pGameInstance->Render_Font_Size(
            L"MainFont",
            m_strMouseOnText,
            _float2(-20.f, -40.f),
            _float2(8.f, 22.f),
            _float3(1.f, 1.f, 0.f));
    }

    if (m_Button_Info.Button_Type == POINT_SHOP_SKILL) // 스펠 설명 버튼
    {
        m_pGameInstance->Render_Font_Size(
            L"MainFont",
            m_strMouseOnText,
            _float2(30.f, 50.f),
            _float2(8.f, 22.f),
            _float3(1.f, 1.f, 0.f));
    }


}



HRESULT CGamePlay_Button::Ready_Components()
{
  
    if (FAILED(__super::Add_Component(LEVEL_HUB, m_Button_Info.strTexture_Default_Tag,
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
