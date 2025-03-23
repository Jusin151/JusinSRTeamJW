#pragma once

#include "Projectile_Base.h"


class CHarpoon : public CProjectile_Base
{
private:
	CHarpoon(LPDIRECT3DDEVICE9 pGraphic_Device);
	CHarpoon(const CHarpoon& Prototype);
	virtual ~CHarpoon() = default;

public:


	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Priority_Update(_float fTimeDelta)override;
	virtual void Update(_float fTimeDelta)override;
	virtual void Late_Update(_float fTimeDelta)override;
	virtual HRESULT Render()override;
	virtual HRESULT On_Collision(_float fTimeDelta)override;


private:

	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
	HRESULT Ready_Components();

private:
	_float m_fRotateTime = {};
public:
	static CHarpoon* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject* Clone(void* pArg) override;
	virtual void Free();

};


