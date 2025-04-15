#pragma once
#include "Deco_Base.h"

class CBombBox : public CDeco_Base
{
private:
	CBombBox(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBombBox(const CBombBox& Prototype);
	virtual ~CBombBox() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
private:
	// texturecom는 각 monster에서 추가
	HRESULT Ready_Components();

public:
	virtual void Priority_Update(_float fTimeDelta)override;
	virtual void Update(_float fTimeDelta)override;
	virtual void Late_Update(_float fTimeDelta)override;

public:
	virtual HRESULT Render()override;
	virtual HRESULT On_Collision(CCollisionObject* other);



private:
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
	virtual void Select_State()override;

	void	Explosion();

private:
	CCollider_Sphere* m_pAttackColliderCom = { nullptr };

	_bool m_bExplosion = { false };

public:
	void Set_Explosion(_bool bType){m_bExplosion = bType;}
	static CBombBox* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject* Clone(void* pArg) override;
	virtual void Free();


};

