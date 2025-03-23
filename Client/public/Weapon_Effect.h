#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Effect_Base.h"


BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;  // UI는 사각형으로 할듯
END

BEGIN(Client)
class CWeapon_Effect final : public CEffect_Base
{

protected:
    CWeapon_Effect(LPDIRECT3DDEVICE9 pGraphic_Device);
    CWeapon_Effect(const CWeapon_Effect& Prototype);
    virtual ~CWeapon_Effect() = default;

public:
     HRESULT Initialize_Prototype()override;
     HRESULT Initialize(void* pArg)override;
     void Priority_Update(_float fTimeDelta)override;
     void Update(_float fTimeDelta)override;
     void Late_Update(_float fTimeDelta)override;
     HRESULT SetUp_RenderState();
     HRESULT Release_RenderState();
     HRESULT Ready_Components()override;
     static CWeapon_Effect* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
     CGameObject* Clone(void* pArg);
     HRESULT Render()override;
      void Free()override;

      CGameObject* m_pTarget{nullptr};
};
END
