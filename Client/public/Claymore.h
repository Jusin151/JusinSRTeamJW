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
	 HRESULT Initialize_Prototype();
	 HRESULT Initialize(void* pArg);
	 void Priority_Update(_float fTimeDelta);
	 void Update(_float fTimeDelta);
	 void Late_Update(_float fTimeDelta);
	 HRESULT Render();
	 Weapon_DESC m_Claymore_INFO{};

private:
	HRESULT Ready_Components();


public:
	static CClaymore* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) ;
	virtual void Free();
private:
	_float m_fElapsedTime = 0.0f;
	_int m_iCurrentFrame = 0;
	const _float m_fFrameDuration = 2.0f;  
};
END