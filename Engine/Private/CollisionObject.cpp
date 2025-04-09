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
	

	// MTV 크기 클램프 (너무 밀리지 않도록)
	const _float maxMtvLength = 1.0f;
	_float mtvLength = m_vObjectMtvSum.Length();
	if (mtvLength > maxMtvLength)
		m_vObjectMtvSum = m_vObjectMtvSum.GetNormalized() * maxMtvLength;

	m_vNextPos += m_vObjectMtvSum;

	
	m_vWallMtvs.clear();
	m_vObjectMtvSum = { 0.f, 0.f,0.f };

	m_vNextPos.y = m_fOffset;
}

void CCollisionObject::Take_Damage(CCollisionObject* other) //other
{

	if (other == nullptr)
		return;

	if (other->Get_Type() == CG_PLAYER)
	{
		other->Take_Damage(Get_Damage());
		return;
	}
	if (Get_Damage() >= 0)
		other->Set_Hp(other->Get_Hp() - Get_Damage());


}


void CCollisionObject::Free()
{
	__super::Free();
}
