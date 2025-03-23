#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
class CCollider_Sphere;
END

BEGIN(Client)
class CShop : public CGameObject
{
protected:
	CShop(LPDIRECT3DDEVICE9 pGraphic_Device);
	CShop(const CShop& Prototype);
	virtual ~CShop() = default;

public:
	virtual HRESULT Initialize_Prototype()PURE;
	virtual HRESULT Initialize(void* pArg)PURE;
	virtual void Priority_Update(_float fTimeDelta)PURE;
	virtual void Update(_float fTimeDelta)PURE;
	virtual void Late_Update(_float fTimeDelta)PURE;
	virtual HRESULT Render()override;

	// 상점 공통 기능
	virtual HRESULT Open_Shop() = 0;  //  상점 열기
	virtual HRESULT Close_Shop() = 0;  //  상점 닫기
	virtual HRESULT Purchase_Item(const _uint iItemID, const _uint iCount = 1) = 0;  // 아이템 구매
	virtual HRESULT Sell_Item(const _uint iItemID, const _uint iCount = 1) = 0;  // 아이템 판매

	virtual void Refresh_Shop_Items() = 0;  // 상점 아이템 새로고침
	virtual _bool Can_Purchase(_uint iItemID, _uint iCount = 1) = 0;  // 구매 가능 여부 확인

	virtual void LookAtPlayer(_float fTimeDelta);


	HRESULT On_Collision();
protected:
	CTexture* m_pTextureCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CCollider_Sphere* m_pColliderCom = { nullptr };
	_bool m_bIsOpen = { false };

protected:
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
	HRESULT Ready_Components();

	// 상점별로 다를 수 있는 아이템 초기화 함수
	virtual HRESULT Ready_ShopItems() = 0;

	CGameObject* m_pPlayer = { nullptr }; // 상점이 늘 플레이어를 쳐다보게 하기 위해서 선언
	_bool bPlayer_Check = {};
public:
	////static CShop* Create(LPDIRECT3DDEVICE9 pGraphic_Device); 자식에서 구현
	//virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();
};
END

