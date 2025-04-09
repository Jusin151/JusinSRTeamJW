#include "CHellBoss_SkillBase.h"
#include "CHellBoss_Skill_Warp.h"


CHellBoss_Skill_Warp::CHellBoss_Skill_Warp(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CEffect_Base{pGraphic_Device}
{
}

CHellBoss_Skill_Warp::CHellBoss_Skill_Warp(const CHellBoss_Skill_Warp& Prototype)
	:CEffect_Base( Prototype )
 
{
}

HRESULT CHellBoss_Skill_Warp::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CHellBoss_Skill_Warp::Initialize(void* pArg)
{
	return E_NOTIMPL;
}

void CHellBoss_Skill_Warp::Priority_Update(_float fTimeDelta)
{
}

void CHellBoss_Skill_Warp::Update(_float fTimeDelta)
{
}

void CHellBoss_Skill_Warp::Process_Input()
{
}

void CHellBoss_Skill_Warp::Late_Update(_float fTimeDelta)
{
}

HRESULT CHellBoss_Skill_Warp::Render()
{
	return E_NOTIMPL;
}

HRESULT CHellBoss_Skill_Warp::SetUp_RenderState()
{
	return E_NOTIMPL;
}

HRESULT CHellBoss_Skill_Warp::Release_RenderState()
{
	return E_NOTIMPL;
}

HRESULT CHellBoss_Skill_Warp::Ready_Components()
{
	return E_NOTIMPL;
}

CHellBoss_Skill_Warp* CHellBoss_Skill_Warp::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	return nullptr;
}

CGameObject* CHellBoss_Skill_Warp::Clone(void* pArg)
{
	return nullptr;
}

void CHellBoss_Skill_Warp::Free()
{
}
