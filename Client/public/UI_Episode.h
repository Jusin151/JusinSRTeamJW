#pragma once

#include "Client_Defines.h"
#include "CUI_Base.h"
#include <functional>
#include <vector>

BEGIN(Client)

class CUI_Episode final : public CUI_Base
{
private:
    CUI_Episode(LPDIRECT3DDEVICE9 pGraphic_Device);
    CUI_Episode(const CUI_Episode& Prototype);
    virtual ~CUI_Episode() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

protected:
    HRESULT Ready_Components();
    HRESULT Register_Buttons();
    void Create_Episode_Buttons();

public:
    static CUI_Episode* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free();
};

END