#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Weapon_Base.h"

BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;  // UI는 사각형으로 할듯
END


BEGIN(Client)

class CItem_Icon final : public CGameObject
{
    typedef struct tagIcon_INFO
    {
        _float2 vPos;
        _float2 vSize;
    }Icon_DESC;

public:
    enum HP_STATE { Default, Heated };
private:
    CItem_Icon(LPDIRECT3DDEVICE9 pGraphic_Device);
    CItem_Icon(const CItem_Icon& Prototype);
    virtual ~CItem_Icon() = default;

public:
    virtual HRESULT Initialize_Prototype()override;
    virtual HRESULT Initialize(void* pArg)override;
    virtual void Priority_Update(_float fTimeDelta)override;
    virtual void Update(_float fTimeDelta)override;
    virtual void Late_Update(_float fTimeDelta)override;
    virtual HRESULT Render()override;


private:
    HRESULT Ready_Components();

public:
    static CItem_Icon* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free();

protected:
    CTexture* m_pTextureCom = { nullptr };
    CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
    CTransform* m_pTransformCom = { nullptr };
    Icon_DESC m_Icon_INFO{};

};
END