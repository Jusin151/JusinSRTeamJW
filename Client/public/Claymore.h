#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Melee_Weapon.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;  // UI는 사각형으로
END

BEGIN(Client)
class CClaymore final : public CMelee_Weapon
{
private:
	CClaymore(LPDIRECT3DDEVICE9 pGraphic_Device);
	CClaymore(const CClaymore& Prototype);
	virtual ~CClaymore() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Priority_Update(_float fTimeDelta)override;
	virtual void Update(_float fTimeDelta)override;
	virtual void Late_Update(_float fTimeDelta)override;
	virtual HRESULT Render()override;


private:
	HRESULT Ready_Components();


public:
	static CClaymore* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();

private:
	_float m_fExp{};  // 체력
};
END