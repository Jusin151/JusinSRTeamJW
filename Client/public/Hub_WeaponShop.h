#pragma once
#include "Shop.h"
#include "Observer.h"

BEGIN(Client)
class CHub_WeaponShop : public CShop
{
private:
    CHub_WeaponShop(LPDIRECT3DDEVICE9 pGraphic_Device);
    CHub_WeaponShop(const CHub_WeaponShop& Prototype);
    virtual ~CHub_WeaponShop() = default;

public:
    // 기본 상속 함수
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;

    virtual void Priority_Update(_float fTimeDelta)override;
    virtual void Update(_float fTimeDelta)override;
    virtual void Late_Update(_float fTimeDelta)override;
    virtual HRESULT Render()override;


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
    static CHub_WeaponShop* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
private:
   // class CUI_WeaponShop_UI* m_pUI_WeaponShop = { nullptr };


private: //옵저버 관련
    vector<CObserver*> m_pObservers{};
public:
	void Add_Observer(CObserver* pObserver)
	{
		m_pObservers.push_back(pObserver);
	}
	virtual void Notify(void* pArg, const wstring& type) 
	{
		for (auto& pObserver : m_pObservers)
		{
			pObserver->OnNotify(pArg, type);
		}
	}
 
};
END