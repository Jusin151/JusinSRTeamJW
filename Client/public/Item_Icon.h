#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Weapon_Base.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;  // UI는 사각형으로 할듯
END


BEGIN(Client)

class CItem_Icon final : public CGameObject
{
public:
    enum Weapon_Type
    {
        Claymore,Axe,ShotGun,Magnum,Staff
    };
    typedef struct tagIcon_INFO
    {
        _float2 vPos{};
        _float2 vSize{};
        _uint Icon_Image{}; // 몇번째 이미지  위의 열거체 참고
        _uint Icon_Index{}; // 인벤의 몇번째 칸에 넣을껀지
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
    virtual void Update(_float fTimeDelta)override;
    virtual void Late_Update(_float fTimeDelta)override;
    virtual HRESULT Render()override;
    
    void Set_Select()
    {
        m_pTransformCom->Set_Scale(m_Icon_INFO.vSize.x*1.5f, m_Icon_INFO.vSize.y*1.5f, 1.f);
    }
    void Set_Base_Size()
    {
        m_pTransformCom->Set_Scale(m_Icon_INFO.vSize.x / 1.5f, m_Icon_INFO.vSize.y / 1.5f, 1.f);
    }

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