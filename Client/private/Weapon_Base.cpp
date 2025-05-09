﻿#include "Weapon_Base.h"

#include "GameInstance.h"
#include "Melee_Weapon.h"
#include "Player.h"


CWeapon_Base::CWeapon_Base(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CCollisionObject{ pGraphic_Device }
{
}

CWeapon_Base::CWeapon_Base(const CWeapon_Base& Prototype)
	: CCollisionObject(Prototype)
{


}

void CWeapon_Base::Priority_Update(_float fTimeDelta)
{
	static _bool bInit = { false };

	if (!bInit)
	{
		m_bIsActive = false;

		bInit = true;
	}
}

void CWeapon_Base::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_VIEWMODEL, this)))
		return;
}

void CWeapon_Base::Free()
{
	__super::Free();

}

void CWeapon_Base::Move_Hand(_float fTimeDelta)
{
	if (GetAsyncKeyState('W') & 0x8000 || 
		GetAsyncKeyState('A') & 0x8000 || 
		GetAsyncKeyState('S') & 0x8000 || 
		GetAsyncKeyState('D') & 0x8000 )
	{
		t += speed;
	}

	float v = 20.0f;  // 폭을 설정 하는변수
	_float3 vNewPos;
	vNewPos.x = m_vInitialPos.x + (1 + v * cosf(t / 2)) * cosf(t);
	vNewPos.y = m_vInitialPos.y + (1 + v * cosf(t / 2)) * sinf(t);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vNewPos);
}

HRESULT CWeapon_Base::On_Collision(CCollisionObject* other)
{
	switch (other->Get_Type()) //여기서 디버깅 잡히나?
	{
	case CG_MONSTER:

		// 나중에 공격력 만들어서 추가하는 식으로
		Take_Damage(other);
		
		break;

	case CG_OBJECT:

		// 나중에 공격력 만들어서 추가하는 식으로
		Take_Damage(other);

		break;

	default:
		break;
	
	}

	return S_OK;
}


HRESULT CWeapon_Base::Ready_Components()
{
	
	return S_OK;
}
