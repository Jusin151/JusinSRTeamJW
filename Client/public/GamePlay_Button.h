#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "CUI_Base.h"
#include "UI_Structs.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;  // UI는 사각형으로
END

BEGIN(Client)

class CGamePlay_Button : public CUI_Base
{
public:
  
 

    typedef struct tagButton
    {
        UI_Parent_Desc Button_Desc{}; //버튼의 월드정보
        wstring strTexture_Default_Tag{}; // 버튼의 텍스쳐키
        wstring strTexture_Select_Tag{}; // 버튼의 두번째 텍스쳐키 ( 잘안씀 )
        wstring strUIName{}; // UI의 이름
        Button_type Button_type{}; // 이 버튼이 4종류의 상점중 어느 상점의 버튼인지
        Episode_Button_Type_Desc Episode_Button_Type{}; // 이 버튼의 에피소드 버튼 타입
        _bool bDisplay_On{};
        _bool Point_Shop_Seleted[16]{};
        _int Point_Shop_Num{};
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
public:
    void Episode_UI_Update();
    void Point_Shop_Update();
    void Spell_Shop_Update();
public:
    void Font_Render();
    void Stat_Render();
    void Button_TexT(_int CurrentButtonType);

public:
    void Episode_Display_Button();
    void Point_Shop_Display_Button();
    void Spell_Shop_Display_Button();

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
    bool m_bChange_Click{};
    _bool m_bIsVisible = {};
    _bool m_bKeyPressed = {};
    _bool m_bOpen_Display = {};
private:
    _bool m_bPoint_Shop_Display{};
    _bool m_bSpell_Shop_Display{};
    _bool m_Upgrade_UI_Display{};
 public:
     _float3 m_fFont_Rgb{};
     _wstring strMin_Stat_box{}; // 요구 스탯 뜨는 창
     _wstring strStat_Tag_box{}; // 스탯 설명창 
     _wstring strItem_Tag{}; // 아이템 설명

  

  
     static _int s_fStrength_Point;// 근력
     static _int s_fLife_Point;    // 생맹력
     static _int s_fSprit_Point;   // 정신력
     static _int s_fCapacity_Point;// 용량  

     static _int s_fStat_Point; // 잔여 스탯 포인트
     static _int s_fLevel_Point;// 레벨 포인트 
     static _int s_fLive_Point; // 목숨 갯수  

};
END