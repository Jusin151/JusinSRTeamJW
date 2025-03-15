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
    enum Button_type // 이 버튼은 어느 메뉴창의 버튼인지
    {
        Episode = 0, Point_Shop = 1, Spell_Shop = 2, Upgrade_Weapon = 3
    };
    enum Level_01_Type // 이 버튼은 어느 이미지를 사용할것인지
    {
        Stage_01_Default=0,
        Stage_02_Default,Stage_02_Selected,
        Stage_03_Default,Stage_03_Selected,
        Stage_04_Default,Stage_04_Selected,
        Stage_05_Default,Stage_05_Selected,
        Level_ICon_Defaul,Level_ICon_Selected,
    };
    typedef struct tagButton
    {
        UI_Parent_Desc Button_Desc{};
        wstring strTexture_Default_Tag{};
        wstring strTexture_Select_Tag{};
        wstring strUIName{};
        Button_type Button_type{};
        _bool bLevel_Icon_Button_Flag{}; 
        _bool bLevel_01_Stage_Button_Flag{};
        _bool bLevel_02_Stage_Button_Flag{};
        _bool bLevel_03_Stage_Button_Flag{};
        _bool bLevel_04_Stage_Button_Flag{};
        _bool bLevel_05_Stage_Button_Flag{};

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

    void Episode_Display_Button();
    void Point_Shop_Display_Button();
    void Spell_Shop_Display_Button();
    void Upgrade_Weapon_Display_Button();

protected:
    CTexture* m_GamePlay_Button_pTextureCom{};
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
    Level_01_Type Current_Image{};
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

