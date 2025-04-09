#include "CollisionObject.h"

CCollisionObject::CCollisionObject(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{
}

CCollisionObject::CCollisionObject(const CCollisionObject& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CCollisionObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCollisionObject::Initialize(void* pArg)
{
	return S_OK;
}

void CCollisionObject::Priority_Update(_float fTimeDelta)
{
}

void CCollisionObject::Update(_float fTimeDelta)
{
}

void CCollisionObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CCollisionObject::Render()
{
	return S_OK;
}

void CCollisionObject::Calc_Position()
{
	if (m_vObjectMtvSum.LengthSq() > 1e-8f)
	{
		if (!m_vWallMtvs.empty())
		{
			for (auto& wallMTV : m_vWallMtvs)
			{
				auto wallNormal = wallMTV.GetNormalized();
				_float penetration = wallNormal.Dot(m_vObjectMtvSum);
				if (penetration < 0.f)
				{
					// 벽 방향으로 침범 중 → 해당 방향 성분 제거
					m_vObjectMtvSum -= wallNormal * penetration;
				}
			}
		
		}
		m_vNextPos += m_vObjectMtvSum;
		
	}
	m_vCurPos.y = m_fOffset;
	m_vNextPos.y = m_fOffset;

}

void CCollisionObject::Take_Damage(CCollisionObject* other) //other
{

	if (other == nullptr)
		return;

	if (Get_Damage() >= 0)
		other->Set_Hp(other->Get_Hp() - Get_Damage());


}

void CCollisionObject::Free()
{
	__super::Free();
}
