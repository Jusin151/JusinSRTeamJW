#pragma once

#include "Client_Defines.h"
#include "CUI_Base.h"
#include <functional>

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

//얘는 버튼 기능만 담당
class CGamePlay_Button : public CUI_Base
{
public:
    enum BUTTON_TYPE_ENUM
    {
        POINT_SHOP_SKILL = 1,POINT_SHOP_STAT = 2,   // 포인트상점

        WEAPON_SHOP_BUTTON = 3 , // 업그레이드웨폰 상점

        SPELL_SHOP_BUTTON = 4, // 스펠 상점

        EPISODE_BUTTON_SELECTED = 5,  // 에피소드 초록색 선택박스

        EPISODE_BUTTON_MAP_IMAGE = 6,  // 에피소드 맵 이미지

        EPISODE_BUTTON = 7, // 에피소드 왼쪽 에피소드 선택 버튼

        BUTTON_END 

    };
public:
    struct GamePlayer_Button_Desc 
    {
        UI_Parent_Desc Button_Desc{}; 
        wstring strTexture_Default_Tag{}; // 프로토타입텍스쳐컴포넌트의 태그
        wstring strUIName{}; 
        wstring strToolTip{}; // 마우스에 상관없이 상점 UI를 켰을때 무조건 보여야하는 텍스트
        _bool bActive={true}; // 장원이형이 만든 Active 활성화 할래 말래
        _bool bRender = { false }; // 이 버튼이 무조건 보이게 할래 말래
        _uint iCurrentImageNum = { 0 };
        BUTTON_TYPE_ENUM Button_Type{}; //어떤 타입의 버튼인지
    };

private:
    CGamePlay_Button(LPDIRECT3DDEVICE9 pGraphic_Device);
    CGamePlay_Button(const CGamePlay_Button& rhs);
    virtual ~CGamePlay_Button() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;
    void Render_MouseOn_Button_TexT();
    void Render_MouseClick_Button_TexT();
    void Render_ToolTip_Button(); // 
public: 
    void SetOnClickCallback(function<void()> callback)
    {
        m_OnClick = callback ;
    }
    void SetOnMouseCallback(function<void()> callback)
    { 
        m_OnMouse = callback; 
    }
protected:
    HRESULT Ready_Components();
    _bool isPick(HWND hWnd);

private:
    CTexture* m_pTextureCom = nullptr;
    CVIBuffer_Rect* m_pVIBufferCom = nullptr;
    CTransform* m_pTransformCom = nullptr;
public:
    GamePlayer_Button_Desc m_Button_Info;


public:
    wstring m_strMouseOnText{};
    wstring m_strToolTipText{}; // 마우스 여부와 상관없이 버튼 위에 무조건 보이게할 버튼
    wstring m_str_Weapon_Click_Text{};
   

private:
    function<void()> m_OnMouse = { nullptr }; //마우스가 위에
    function<void()> m_OnClick = { nullptr }; //마우스 클릭 
    _bool m_bVisible = { true }; // 마우스 안올렸을때는 안보이게
public:
    _bool m_bVisible_Click = { false };
    _bool m_bClickLocked = { false };

public:
    static CGamePlay_Button* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

END