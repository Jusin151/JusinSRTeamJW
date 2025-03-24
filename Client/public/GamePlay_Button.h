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
    struct GamePlayer_Button_Desc 
    {
        UI_Parent_Desc Button_Desc{};
        wstring strTexture_Default_Tag{};
        wstring strUIName{};
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

public: // 클릭 관련 함수
    void SetOnClickCallback(function<void()> callback) {m_OnClick = callback;}
    void SetOnMouseCallback(function<void()> callback) { m_OnMouse = callback; }
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
   
private:
    function<void()> m_OnMouse = { nullptr }; //마우스가 위에 있는지
    function<void()> m_OnClick = { nullptr }; //마우스가 클릭 했는지
    _bool m_bVisible = true;

public:
    static CGamePlay_Button* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

END