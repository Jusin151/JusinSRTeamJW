#include "UI_Episode.h"
#include "GamePlay_Button.h"
#include "GameInstance.h"


CUI_Episode::CUI_Episode(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CUI_Base(pGraphic_Device) {
}

CUI_Episode::CUI_Episode(const CUI_Episode& Prototype)
    : CUI_Base(Prototype) {
}

HRESULT CUI_Episode::Initialize_Prototype() {
    return S_OK;
}

HRESULT CUI_Episode::Initialize(void* pArg)
{
    if (pArg != nullptr)
        m_Shop_INFO = *reinterpret_cast<UI_Desc*>(pArg);
    else
        return E_FAIL;


    if (FAILED(Ready_Components()))
        return E_FAIL;


    if (FAILED(Register_Buttons()))
        return E_FAIL;

    m_pTransformCom->Set_Scale(m_Shop_INFO.vSize.x, m_Shop_INFO.vSize.y, 1.f);


    m_pTransformCom->Set_State(CTransform::STATE_POSITION,
        _float3(m_Shop_INFO.vPos.x, m_Shop_INFO.vPos.y, 0.f));

    return S_OK;
}

void CUI_Episode::Priority_Update(_float fTimeDelta) {}

void CUI_Episode::Update(_float fTimeDelta)
{
    m_bIsActive = false;
    // 에피소드 버튼 클릭 등 처리
}

void CUI_Episode::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this)))
        return;
}

HRESULT CUI_Episode::Render() 
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



    return S_OK;
}

HRESULT CUI_Episode::Ready_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Episode"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;


    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_HP"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &tDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Episode::Register_Buttons()
{
    Create_Episode_Buttons();
    return S_OK;
}

void CUI_Episode::Create_Episode_Buttons()
{
    vector<CGamePlay_Button::GamePlayer_Button_Desc> vecEpisodeButtons(5);

    for (int i = 0; i < 5; ++i)
    {
        vecEpisodeButtons[i].Button_Desc.vSize = { 180.f, 40.f };
        vecEpisodeButtons[i].Button_Desc.vPos = { -300.f + i * 150.f, 200.f };
        vecEpisodeButtons[i].strTexture_Default_Tag = L"Prototype_Component_Texture_Episode_Button";
        vecEpisodeButtons[i].strUIName = L"Episode_Button_" + to_wstring(i);

        if (FAILED(m_pGameInstance->Add_GameObject(
            LEVEL_GAMEPLAY,
            TEXT("Prototype_GameObject_GamePlayer_Button"),
            LEVEL_LOGO,
            TEXT("Layer_Episode_UI"),
            &vecEpisodeButtons[i])))
            continue;

        CGamePlay_Button* pButton = static_cast<CGamePlay_Button*>(
            m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, vecEpisodeButtons[i].strUIName.c_str()));

        if (pButton)
        {
            pButton->SetOnClickCallback([i]()
                {
                OutputDebugString((L"[EpisodeUI] Stage Button Clicked: " + to_wstring(i) + L"\n").c_str());
                // TODO: 에피소드 진입 처리 또는 스테이지 전환
                });
        }
    }
}

CUI_Episode* CUI_Episode::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CUI_Episode* pInstance = new CUI_Episode(pGraphic_Device);
    if (FAILED(pInstance->Initialize_Prototype())) {
        MSG_BOX("Failed to Create: CUI_Episode");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_Episode::Clone(void* pArg)
{
    CUI_Episode* pInstance = new CUI_Episode(*this);
    if (FAILED(pInstance->Initialize(pArg))) {
        MSG_BOX("Failed to Clone: CUI_Episode");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CUI_Episode::Free()
{
    __super::Free();
}
