#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "CUI_Base.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;  // UI는 사각형으로
END

BEGIN(Client)

class CGamePlay_Button : public CUI_Base
{
public:
    enum Button_type
    {
        Episode
    };
    typedef struct tagButton
    {
        UI_Parent_Desc Button_Desc{};
        wstring strTexture_Default_Tag{};
        wstring strTexture_Select_Tag{};
        wstring strUIName{};
        _bool bLevel_Icon_Button_Flag{}; 
        _bool bLevel_1_Button_Flag{};
        _bool bLevel_2_Button_Flag{};
        _bool bLevel_3_Button_Flag{};
        _bool bLevel_4_Button_Flag{};
        _bool bLevel_5_Button_Flag{};
    }GamePlayer_Button_Desc;

protected:
    CGamePlay_Button(LPDIRECT3DDEVICE9 pGraphic_Device);
    CGamePlay_Button(const CGamePlay_Button& Prototype);
    virtual ~CGamePlay_Button() = default;

public:
    virtual HRESULT Initialize_Prototype()override;
    virtual HRESULT Initialize(void* pArg)override;
    virtual void Priority_Update(_float fTimeDelta)override;
    virtual void Update(_float fTimeDelta)override;
    virtual void Late_Update(_float fTimeDelta)override;
    virtual HRESULT Render()override;

protected:
    CTexture* m_GamePlay_Button_pTextureCom{};
    CTexture* m_GamePlayer_Button_pTextureCom_Second{};
    CVIBuffer_Rect* m_GamePlayer_Button_pVIBufferCom{};
    CTransform* m_GamePlayer_Button_pTransformCom{};

    GamePlayer_Button_Desc m_Button_INFO{};


    _bool isPick(HWND hWnd)
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
protected:
    HRESULT Ready_Components();


public:
    static CGamePlay_Button* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free();

private:
    bool m_bIsVisible = {};
    bool m_bKeyPressed = {};
private:
    _bool m_bEpisode_Display{};
    _bool m_bPoint_Shop_Display{};
    _bool m_bSpell_Shop_Display{};
    _bool m_Upgrade_UI_Display{};
};


END

