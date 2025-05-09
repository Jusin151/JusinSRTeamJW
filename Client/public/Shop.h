﻿#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Camera_FirstPerson.h"

BEGIN(Engine)
class CLight;
class CShader;
class CTexture;
class CMaterial;
class CTransform;
class CVIBuffer_Rect;
class CCollider_Sphere;
END


enum class PurchaseStatus : uint8_t
{
	Success = 0,         // 구매 가능(성공)
	NotEnoughPoint = 1,    // 구매 금액 부족
	AlreadyOwned = 2,      // 이미 소유중
	ShopClosed = 3,        // 상점이 닫힘
	UnknownError = 4
};

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

	virtual void LookAtPlayer(_float fTimeDelta);

private:
	_bool m_bSpacePressed = false; // 스페이스가 이미 눌려있었는지 체크

	HRESULT On_Collision();
protected:
	CLight* m_pLightCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CMaterial* m_pMaterialCom = { nullptr };
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
	CCamera_FirstPerson* m_pFirstPersonCamera = { nullptr }; // 카메라 움직임제어용
	_bool bPlayer_Check = {};
public:
	////static CShop* Create(LPDIRECT3DDEVICE9 pGraphic_Device); 자식에서 구현
	//virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();
};
END

