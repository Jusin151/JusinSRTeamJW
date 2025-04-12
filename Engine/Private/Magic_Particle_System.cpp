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
	m_vPos = { 0.f, 0.f, 0.f };
	m_fSize = 0.1f;
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

void CMagic_Particle_System::Reset_Particle(ATTRIBUTE* pAttribute)
{
	pAttribute->bIsAlive = true;
	GetRandomVector(&pAttribute->vPosition, &m_Bound.m_vCenter, m_Bound.m_fRadius);
	pAttribute->vPosition.z = -1.f;
	pAttribute->vVelocity = { GetRandomFloat(-1.f, 1.0f), GetRandomFloat(0.f, 5.0f), 0.f };
	pAttribute->vAcceleration = { 1.5f, 1.2f, 0.0f };
	pAttribute->fAge = 0;
	pAttribute->fLifetime = 2.0f;
	pAttribute->iIndex = rand() % m_pTexture->Get_NumTextures();

	pAttribute->fSize = m_fSize / D3DXVec3Length(&pAttribute->vPosition);
	pAttribute->vCurrentColor = 0xFF883932;
}

void CMagic_Particle_System::Update(float fTimeDelta)
{
	for (auto& i : m_Particles)
	{
		if (i.bIsAlive)
		{
			i.vPosition += (i.vVelocity * i.vAcceleration.x) * fTimeDelta;
			i.vVelocity.y -= GRAVITY * fTimeDelta;
			i.fAge += fTimeDelta;
			i.fSize = m_fSize / D3DXVec3Length(&i.vPosition);
			if (i.fAge > i.fLifetime)
				i.bIsAlive = false;
			//Reset_Particle(&i);
		}
	}
}

HRESULT CMagic_Particle_System::Pre_Render()
{
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, false);
	m_pGraphic_Device->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
	m_pGraphic_Device->SetRenderState(D3DRS_POINTSCALEENABLE, true);
	//m_pGraphic_Device->SetRenderState(D3DRS_POINTSIZE, FtoDW(m_fSize));

	m_pGraphic_Device->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDW(0.0f));

	m_pGraphic_Device->SetRenderState(D3DRS_POINTSCALE_A, FtoDW(0.0f));
	m_pGraphic_Device->SetRenderState(D3DRS_POINTSCALE_B, FtoDW(0.0f));
	m_pGraphic_Device->SetRenderState(D3DRS_POINTSCALE_C, FtoDW(1.0f));

	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1); // 정점 색상만 사용
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);    // 텍스쳐 색상 지정

	//m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE); // 텍스처 알파값 사용
	//m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);  // 텍스처에서 알파값 가져옴
	//m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);  // 정점에서 알파값 가져옴

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
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
