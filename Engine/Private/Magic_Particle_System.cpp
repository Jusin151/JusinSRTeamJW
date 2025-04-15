#include "Magic_Particle_System.h"

CMagic_Particle_System::CMagic_Particle_System(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CParticle_System{ pGraphic_Device }
{
}

CMagic_Particle_System::CMagic_Particle_System(const CMagic_Particle_System& Prototype)
	: CParticle_System{ Prototype }
	, m_Bound{ Prototype.m_Bound }
{
}

HRESULT CMagic_Particle_System::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMagic_Particle_System::Initialize(void* pArg)
{
	MAGICDESC desc = *reinterpret_cast<MAGICDESC*>(pArg);
	m_Bound = desc.Bound;
	m_vPos = desc.vOrigin;
	m_fSize = desc.fSize;
	m_VBSize = 2048;
	m_VBOffset = 0;
	m_VBBatchSize = 512;
	m_iMaxParticles = desc.iNumParticles;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	for (_uint i = 0; i < m_iMaxParticles; ++i)
	{
		Add_Particle();
	}
	return S_OK;
}

void CMagic_Particle_System::Reset_Particle(ATTRIBUTE* pAttribute, void* pArg)
{
	pAttribute->bIsAlive = true;
	GetRandomVector(&pAttribute->vPosition, &m_Bound.m_vCenter, m_Bound.m_fRadius);
	pAttribute->vPosition.z = -1.f;
	pAttribute->vVelocity = { GetRandomFloat(-1.f, 1.0f), GetRandomFloat(-1.f, 1.0f), GetRandomFloat(-1.f, 1.0f) };
	//pAttribute->vAcceleration = { 1.5f, 1.2f, 0.0f };
	pAttribute->fAge = 0;
	pAttribute->fLifetime = 2.0f;
	pAttribute->iIndex = 0;

	pAttribute->vInitialColor = 0xFFFFbeFF;
	pAttribute->vCurrentColor = pAttribute->vInitialColor;
	pAttribute->vColorFade = 0x00FFbeFF;

	pAttribute->fSize = m_fSize / D3DXVec3Length(&pAttribute->vPosition);
}

void CMagic_Particle_System::Update(float fTimeDelta)
{
	for (auto& i : m_Particles)
	{
		if (i.bIsAlive)
		{
			// --- 위치 업데이트 (변경된 속도 적용) ---
			i.vPosition += i.vVelocity * fTimeDelta;
			i.fSize *= 0.90f;

			// --- 색상 페이드 아웃 및 수명 관리 (기존과 동일) ---
			float ratio = i.fAge / i.fLifetime;
			i.vCurrentColor = ColorLerp(i.vInitialColor, i.vColorFade, ratio);
			i.fAge += fTimeDelta;
		}
		if (i.fAge > i.fLifetime)
			i.bIsAlive = false;
	}

	__super::Late_Update(fTimeDelta);
}

HRESULT CMagic_Particle_System::Pre_Render()
{
	__super::Pre_Render();
	return S_OK;
}

CMagic_Particle_System* CMagic_Particle_System::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMagic_Particle_System* pInstance = new CMagic_Particle_System(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMagic_Particle_System");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMagic_Particle_System* CMagic_Particle_System::Clone(void* pArg)
{
	CMagic_Particle_System* pInstance = new CMagic_Particle_System(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CMagic_Particle_System");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMagic_Particle_System::Free()
{
	__super::Free();
}
