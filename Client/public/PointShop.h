#pragma once
#include "Shop.h"

BEGIN(Client)
class CPoint_Shop : public CShop
{
private:
    CPoint_Shop(LPDIRECT3DDEVICE9 pGraphic_Device);
    CPoint_Shop(const CPoint_Shop& Prototype);
    virtual ~CPoint_Shop() = default;

public:

    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;

    virtual void Priority_Update(_float fTimeDelta)override;
    virtual void Update(_float fTimeDelta)override;
    virtual void Late_Update(_float fTimeDelta)override;
    virtual HRESULT Render()override;

public: //상점관련
    virtual HRESULT Open_Shop() override;
    virtual HRESULT Close_Shop() override;
    virtual HRESULT Purchase_Item(const _uint iItemID, const _uint iCount = 1) override;
    virtual HRESULT Sell_Item(const _uint iItemID, const _uint iCount = 1) override;
    virtual void Refresh_Shop_Items() override;
    virtual _bool Can_Purchase(_uint iItemID, _uint iCount = 1) override;
    void Buy_Stat(_uint iStatIndex);
    HRESULT SetUp_RenderState();
    HRESULT Release_RenderState();

protected:
    virtual HRESULT Ready_ShopItems() override;
private:
    HRESULT Ready_Components();

public:
    static CPoint_Shop* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
private:
    class CUI_Point_Shop* m_pUI_Point_Hub = { nullptr };

};
END