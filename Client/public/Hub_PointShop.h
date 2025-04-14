#pragma once
#include "Shop.h"
#include "Player.h"

BEGIN(Client)
class CHub_PointShop : public CShop
{
    enum STAT_ENUM
    {
        STR,MAXHP,SPRIT,CAPACITY
    };
private:
    CHub_PointShop(LPDIRECT3DDEVICE9 pGraphic_Device);
    CHub_PointShop(const CHub_PointShop& Prototype);
    virtual ~CHub_PointShop() = default;

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
    void  Buy_Stat(_int index);

    void Buy_Skill(_int index);


    HRESULT SetUp_RenderState();
    HRESULT Release_RenderState();

protected:
    virtual HRESULT Ready_ShopItems() override;
private:
    HRESULT Ready_Components();

public:
    static CHub_PointShop* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
private:
  // class CUI_Point_Shop* m_pUI_Point_Hub = { nullptr };
private: //옵저버 관련
    vector<CObserver*> m_pObservers{};
public:
    void Add_Observer(CObserver* pObserver)
    {
        m_pObservers.push_back(pObserver);
    }
    virtual void Notify(void* pArg, const wstring& type)
    {
   
        for (auto& obs : m_pObservers)
            obs->OnNotify(pArg, type);
    }


};
END