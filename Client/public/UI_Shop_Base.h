#pragma once

#include "Client_Defines.h"
#include "UI_Base.h"
#include "GamePlay_Button.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CUI_Shop_Base : public CUI_Base
{
public:
    typedef struct tagShop_INFO
    {
        _float2 vPos = { 0.f, 0.f };     // 상점 UI의 위치
        _float2 vSize = { 0.f, 0.f };    // 상점 UI의 크기

    }SHOP_DESC;


protected:
    CUI_Shop_Base(LPDIRECT3DDEVICE9 pGraphic_Device);
    CUI_Shop_Base(const CUI_Shop_Base& rhs);
    virtual ~CUI_Shop_Base() = default;
protected:
    void Font_Render();
    void Stat_Render();
    void Button_TexT(_int CurrentButtonType);
    _bool isPick(HWND hWnd);
protected:
    virtual HRESULT Initialize_Prototype() PURE;
    virtual HRESULT Initialize(void* pArg) PURE;
    virtual void Priority_Update(_float fTimeDelta) PURE;
    virtual void Update(_float fTimeDelta) PURE;
    virtual void Late_Update(_float fTimeDelta) PURE;
    virtual HRESULT Render()override;

protected:
    // 자식이 버튼들을 채우도록 순수 가상함수 선언
    virtual HRESULT Register_Buttons()PURE;

public:
    // UI 열고 닫기
    virtual void Open_ShopUI();
    virtual void Close_ShopUI();

protected:
    vector<class CGamePlay_Button*> m_vecButtons;
public:
    void Button_Set_Active(_bool bActive)
    {
        m_bIsActive = bActive;

        for (auto& pButton : m_vecButtons)
        {
            if (pButton)
                pButton->SetActive(bActive); // 버튼도 같이 on/off
        }

    }

protected:
    HRESULT Ready_Components();

    _bool m_bOnUI = { false };

public:
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

END
