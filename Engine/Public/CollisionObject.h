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

	virtual void Take_Damage(_uint Damage) { m_iHp -= Damage; }

	// 다른 오브젝트에 데미지를 준다
	void Take_Damage(CCollisionObject* other);


	void Set_Type(COLLIDERGROUP eType) { m_eType = eType; }
	
	CGameObject* Get_Trigger() { return m_pTrigger; }
	void Set_Trigger(CGameObject* pTrigger) { m_pTrigger = pTrigger; }

	
	void Add_WallMtv(_float3 mtv) { m_vWallMtvs.push_back(mtv); }
	
	void Set_NextPos(_float3 fPos) { m_vNextPos = fPos; }
	void Set_Offset(_float offset) { m_fOffset = offset; }

	void Calc_Position();




protected:

	
	

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

	CGameObject* m_pTrigger = { nullptr };

	_float3 m_vObjectMtvSum = { 0.f, 0.f, 0.f };
	vector<_float3> m_vWallMtvs;

	_float m_fOffset = { 0.f };


public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free();


};

END