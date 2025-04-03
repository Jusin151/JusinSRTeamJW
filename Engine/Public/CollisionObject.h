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

	virtual void Set_Hp(_int iHp) { m_iHp = iHp; }
	virtual void Set_Ap(_int iAp) { m_iAp = iAp; }

	void Set_Type(COLLIDERGROUP eType) { m_eType = eType; }
	
	CGameObject* Get_Trigger() { return m_pTrigger; }
	void Set_Trigger(CGameObject* pTrigger) { m_pTrigger = pTrigger; }

	

protected:

	// 다른 오브젝트에 데미지를 준다
	 void Take_Damage(CCollisionObject* other);
	

protected:
	_int m_iHp = 0;
	_int m_iAp = 0;
	COLLIDERGROUP m_eType = CG_END;

	CGameObject* m_pTrigger = { nullptr };


public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free();


};

END