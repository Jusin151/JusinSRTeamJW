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


    void SetOnClickCallback(function<void()> callback);

protected:
    HRESULT Ready_Components();
    _bool isPick(HWND hWnd);

private:
    CTexture* m_pTextureCom = nullptr;
    CVIBuffer_Rect* m_pVIBufferCom = nullptr;
    CTransform* m_pTransformCom = nullptr;

    GamePlayer_Button_Desc m_Button_Info;
    function<void()> m_OnClick;

    _bool m_bVisible = true;

public:
    static CGamePlay_Button* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

END