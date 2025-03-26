#pragma once
#include "Projectile_Base.h"

BEGIN(Engine)
class CMaterial;
class CParticle_System;
END

BEGIN(Client)

class CGameObject_Projectile_Test final : public CProjectile_Base
{
private:
	CGameObject_Projectile_Test(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject_Projectile_Test(const CGameObject_Projectile_Test& Prototype);
	virtual ~CGameObject_Projectile_Test() = default;

public:
	virtual HRESULT Initialize_Prototype()			override;
	virtual HRESULT Initialize(void* pArg)			override;

	HRESULT Ready_Components();
public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta)			override;
	virtual void Late_Update(_float fTimeDelta)		override;

public:
	virtual HRESULT Pre_Render();
	virtual HRESULT Render() override;
	virtual HRESULT Post_Render();
public:

	virtual HRESULT On_Collision(CCollisionObject* other) { return S_OK; }

private:
	CMaterial*			m_pMaterialCom = { nullptr };
	CParticle_System*	m_pParticleCom = { nullptr };

public:
	static	CGameObject_Projectile_Test* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject_Projectile_Test* Clone(void* pArg);
	virtual void Free();

public:
	virtual json Serialize() override;
	virtual void Deserialize(const json& j) override;
};


END