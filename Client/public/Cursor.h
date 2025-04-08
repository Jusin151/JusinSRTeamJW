#pragma once

#include "UI_Base.h"

BEGIN(Client)

class CCursor final : public CUI_Base
{
private:
    explicit CCursor(LPDIRECT3DDEVICE9 pGraphic_Device);
    explicit CCursor(const CCursor& rhs);
    virtual ~CCursor() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;
    virtual HRESULT Ready_Components() override;

private:
    _float2 m_vMousePos = { 0.f, 0.f };

public:
    static CCursor* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
};

END
