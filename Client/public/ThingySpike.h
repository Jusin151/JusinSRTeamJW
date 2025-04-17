

#pragma once
#include "Projectile_Base.h"
BEGIN(Engine)
class CShader;
class CMaterial;
END

BEGIN(Client)
class CThingySpike final : public CProjectile_Base
{
	
private:
	CThingySpike(LPDIRECT3DDEVICE9 pGraphic_Device);
	CThingySpike(const CThingySpike& Prototype);
	virtual ~CThingySpike() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Priority_Update(_float fTimeDelta)override;
	virtual void Update(_float fTimeDelta)override;
	virtual void Late_Update(_float fTimeDelta)override;
	virtual HRESULT Render()override;
	virtual HRESULT On_Collision(CCollisionObject* other)override;

private:

	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
	HRESULT Ready_Components();
	void Billboarding(_float fTimeDelta);
	void Update_Animation(_float fTimeDelta);
	
private:
	_bool m_bIsHit = false;

	CGameObject* m_pTarget = { nullptr };

	// 애니메이션 돌리기 위해.. 한 프레임 시간, 현재 프레임 수
	_float m_fElapsedTime = { 0.f };
	_int m_iCurrentFrame = { 0 };

public:
	static CThingySpike* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject* Clone(void* pArg) override;
	virtual void Free();

};
END
