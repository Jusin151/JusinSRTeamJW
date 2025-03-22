#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Ranged_Weapon.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;  // UI는 사각형으로
END

BEGIN(Client)
class CMagnum final : public CRanged_Weapon
{
private:
	CMagnum(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMagnum(const CMagnum& Prototype);
	virtual ~CMagnum() = default;

public:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);
	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Attack(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);
	HRESULT Render();
	Weapon_DESC m_Magnum_INFO{};

private:
	HRESULT Ready_Components();


public:
	static CMagnum* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg)override;
	virtual void Free();

	_float t = {}; //
	_float speed = { 0.1f }; //
	_float3 m_vInitialPos = {};



};
END