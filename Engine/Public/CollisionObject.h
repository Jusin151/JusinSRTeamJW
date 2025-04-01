#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCollisionObject abstract : public CGameObject
{

protected:
	CCollisionObject(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCollisionObject(const CCollisionObject& Prototype);
	virtual ~CCollisionObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT On_Collision(CCollisionObject* other) = 0;

	_int Get_Damage() { return m_iAp; }
	_int Get_Hp() { return m_iHp; }
	COLLIDERGROUP Get_Type() { return m_eType; }
	_float Get_Length() { return m_fLength; }
	_float3 Get_Dir() { return m_vDir; }
	_float Get_Speed() { return m_fSpeed; }

	virtual void Set_Hp(_int iHp) { m_iHp = iHp; }
	virtual void Set_Ap(_int iAp) { m_iAp = iAp; }

	void Set_Type(COLLIDERGROUP eType) { m_eType = eType; }

	void Set_NextPos(_float3 fPos) { m_vNextPos = fPos; }

protected:

	// 다른 오브젝트에 데미지를 준다
	 void Take_Damage(CCollisionObject* other);
	

protected:
	_int m_iHp = 0;
	_int m_iAp = 0;
	COLLIDERGROUP m_eType = CG_END;

	// 현재 위치, 다음 위치
	_float3 m_vCurPos = {};
	_float3 m_vNextPos = {};

	// 방향과 길이
	_float m_fLength = { 0.f };
	_float3 m_vDir = { 0.f, 0.f, 0.f };

	// ...
	_float m_fSpeed = { 0.f };

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free();


};

END