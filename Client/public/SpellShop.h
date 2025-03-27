#pragma once
#include "Shop.h"

BEGIN(Client)
class CSpellShop : public CShop
{
private:
    CSpellShop(LPDIRECT3DDEVICE9 pGraphic_Device);
    CSpellShop(const CSpellShop& Prototype);
    virtual ~CSpellShop() = default;

public:
    // 기본 상속 함수
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;

    virtual void Priority_Update(_float fTimeDelta)override;
    virtual void Update(_float fTimeDelta)override;
    virtual void Late_Update(_float fTimeDelta)override;
    virtual HRESULT Render()override;

    // Shop 인터페이스 구현

    virtual HRESULT Open_Shop() override;
    virtual HRESULT Close_Shop() override;
    virtual HRESULT Purchase_Item(const _uint iItemID, const _uint iCount = 1) override;
    virtual HRESULT Sell_Item(const _uint iItemID, const _uint iCount = 1) override;
    virtual void Refresh_Shop_Items() override;
    virtual _bool Can_Purchase(_uint iItemID, _uint iCount = 1) override;
    HRESULT SetUp_RenderState();
    HRESULT Release_RenderState();


protected:
    virtual HRESULT Ready_ShopItems() override;
private:
    HRESULT Ready_Components();

public:
    static CSpellShop* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
private:
    class CUI_Spell_Shop* m_pUI_SpellShop = { nullptr };

};
END