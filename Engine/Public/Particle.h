#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CParticle : public CGameObject
{
private:
	CParticle(LPDIRECT3DDEVICE9 pGraphic_Deivce);
	CParticle(const CParticle& Prototype);
	virtual ~CParticle() = default;
public:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	HRESULT Render() override;

public:
	static CParticle* Create(LPDIRECT3DDEVICE9 pGraphic_Deivce);
	static CParticle* Clone(void* pArg);
	virtual void Free() override;
};

END

