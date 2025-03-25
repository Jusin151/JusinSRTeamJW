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

        EPISODE_BUTTON = 5,  // 에피소드 선택창

        BUTTON_END 

    };
public:
    struct GamePlayer_Button_Desc 
    {
        UI_Parent_Desc Button_Desc{};
        wstring strTexture_Default_Tag{};
        wstring strUIName{};
        _bool bActive{};
        BUTTON_TYPE_ENUM Button_Type{}; 
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

    GamePlayer_Button_Desc m_Button_Info;

public:
    wstring m_strMouseOnText{};
    wstring m_str_Weapon_Click_Text{};
   
private:
    function<void()> m_OnMouse = { nullptr }; //마우스가 위에
    function<void()> m_OnClick = { nullptr }; //마우스 클릭 
    _bool m_bVisible = true; // 마우스 안올렸을때는 안보이게

public:
    static CGamePlay_Button* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

END