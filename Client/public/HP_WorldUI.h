#pragma once
#include "UI_Base.h"
#include "Observer.h"

BEGIN(Engine)
class CShader;
class CMaterial;
END

BEGIN(Client)
class CHP_WorldUI final : public CUI_Base,public CObserver
{
private:
    CHP_WorldUI(LPDIRECT3DDEVICE9 pGraphic_Device);
    CHP_WorldUI(const CHP_WorldUI& Prototype);
    virtual ~CHP_WorldUI() = default;


public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Priority_Update(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render()override;
    virtual void Reset() override;
    void Set_Parent(CGameObject* pParent) { m_pOwner = pParent; }
private:
    virtual HRESULT Ready_Components();
    HRESULT SetUp_RenderState();
    HRESULT Release_RenderState();
    void Setting_Position();
    void Billboarding(_float fTimeDelta);
private:
    CShader* m_pShaderCom{ nullptr };
    CMaterial* m_pMaterialCom{ nullptr };
    CTexture* m_pBarTextureCom{ nullptr };
    CGameObject* m_pOwner{ nullptr };
    _bool m_bIsCloned{ false };
    _float m_fHpRatio{ 1.f };
    _float m_fFloatingTime{ 0.f };

public:
    void OnNotify(void* pArg, const wstring& type) override;
    static CHP_WorldUI* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;
};
END

