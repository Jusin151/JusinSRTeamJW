#include "CthulhuAttackMag.h"
#include "Cthulhu.h"
#include <CthulhuMissile.h>
#include <Transform.h>
#include "Cthulhu_Tentacle.h"
#include "Cthulhu_Big_Tentacle.h"
#include "Spike.h"
#include "GameInstance.h"


CCthulhuAttackMag::CCthulhuAttackMag(CCthulhu* pOwner)
	: m_pOwner(pOwner)
{
}

NodeStatus CCthulhuAttackMag::Execute_MissileAttack()
{
	if (!m_pOwner->IsPlayerVisible() || m_pOwner->m_bIsMultiAttack)
		return NodeStatus::FAIL;

	if (m_pOwner->m_fAttackCoolTime < m_pOwner->m_fAttackCoolDown)
		return NodeStatus::FAIL;

	m_pOwner->m_fAttackCoolTime = 0.f;
	m_pOwner->m_eState = CCthulhu::STATE::ATTACK;
	m_pOwner->m_fFrame = 0.f;
	m_pOwner->m_iCurrentFrame = m_pOwner->m_mapStateTextures[m_pOwner->m_eState][0];

	auto vPos = m_pOwner->m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos.y -= 1.2f;

	CTransform* pTransform = dynamic_cast<CTransform*>(m_pOwner->m_pTarget->Get_Component(TEXT("Com_Transform")));
	_float3 vDir = pTransform->Get_State(CTransform::STATE_POSITION) - vPos;
	vDir.Normalize();

	CCthulhuMissile::PROJ_DESC prjDesc{ vPos, vDir, 20.f };
	m_pOwner->m_pGameInstance->Add_GameObject(LEVEL_BOSS, TEXT("Prototype_GameObject_CthulhuMissile"), LEVEL_BOSS, TEXT("Layer_CthulhuMissile"), &prjDesc);

	return NodeStatus::SUCCESS;
}

NodeStatus CCthulhuAttackMag::Execute_MultiMissileAttack()
{
	if (!m_pOwner->m_bIsMultiAttack && m_pOwner->m_fMultiAttackCoolTime < m_pOwner->m_fMultiAttackCoolDown)
		return NodeStatus::FAIL;

	if (m_pOwner->m_iHp < m_pOwner->m_fPhaseThreshold && m_pOwner->IsPlayerVisible())
	{
		m_pOwner->m_fMultiAttackCoolTime = 0.f;

		if (!m_pOwner->m_bIsMultiAttack)
		{
			m_pOwner->m_bIsMultiAttack = true;
			m_pOwner->m_eState = CCthulhu::STATE::MULTI_ATTACK;
			m_pOwner->m_iMultiMissilesToFire = 7;
			m_pOwner->m_fMultiMissileTimer = 0.f;
			m_pOwner->m_fFrame = 0.f;
			m_pOwner->m_iCurrentFrame = m_pOwner->m_mapStateTextures[m_pOwner->m_eState][0];
		}

		m_pOwner->m_fMultiMissileTimer += m_pOwner->m_fDelta;

		if (m_pOwner->m_fMultiMissileTimer >= 0.1f && m_pOwner->m_iMultiMissilesToFire > 0)
		{
			auto vPos = m_pOwner->m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			vPos.y -= 1.2f;

			CTransform* pTransform = dynamic_cast<CTransform*>(m_pOwner->m_pTarget->Get_Component(TEXT("Com_Transform")));
			_float3 vDir = pTransform->Get_State(CTransform::STATE_POSITION) - vPos;
			vDir.Normalize();

			CCthulhuMissile::PROJ_DESC prjDesc{ vPos, vDir, 60.f };
			m_pOwner->m_pGameInstance->Add_GameObject(LEVEL_BOSS, TEXT("Prototype_GameObject_CthulhuMissile"), LEVEL_BOSS, TEXT("Layer_CthulhuMissile"), &prjDesc);

			m_pOwner->m_iMultiMissilesToFire--;
			m_pOwner->m_fMultiMissileTimer = 0.f;
		}

		if (m_pOwner->m_iMultiMissilesToFire == 0)
		{
			m_pOwner->m_bIsMultiAttack = false;
			m_pOwner->m_eState = CCthulhu::STATE::IDLE;
			m_pOwner->m_fFrame = 0.f;
			m_pOwner->m_iCurrentFrame = m_pOwner->m_mapStateTextures[m_pOwner->m_eState][0];
		}
		return NodeStatus::SUCCESS;
	}
	return NodeStatus::FAIL;
}

NodeStatus CCthulhuAttackMag::Execute_SpikeAttack()
{
	return m_pOwner->Attack_Spike();
}

NodeStatus CCthulhuAttackMag::Execute_TentacleDeploy()
{
	return m_pOwner->Deploy_Tentacles();
}

NodeStatus CCthulhuAttackMag::Execute_BigTentacleDeploy()
{
	return m_pOwner->Deploy_BigTentacles();
}
