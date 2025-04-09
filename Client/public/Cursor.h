#pragma once

#include "UI_Base.h"
BEGIN(Engine)
class CShader;
class CTexture;
class CMaterial;
class CTransform;
END

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

private:
    HRESULT SetUp_RenderState();
    HRESULT Release_RenderState();
    virtual HRESULT Ready_Components() override;

private:
    _float2 m_vMousePos = { 0.f, 0.f };
    _bool m_bMousePressed{ false };
    _float4x4 m_MatOldView;
    _float4x4 m_MatOldProj;
    _float4x4 m_MatView;
    _float4x4 m_MatProj;
    D3DVIEWPORT9 m_Viewport;
public:
    static CCursor* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free();
};

END
