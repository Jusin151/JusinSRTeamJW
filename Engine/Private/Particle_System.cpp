#include "Particle_System.h"
#include "Texture.h"


bool CParticle_System::IsEmpty()
{
	return m_Particles.empty();
}

bool CParticle_System::IsDead()
{
	if (!m_Particles.empty())
	{
		for (auto& particle : m_Particles)
		{
			if (particle.bIsAlive == true) return false;
		}
	}
	return true;
}

void CParticle_System::Remove_Dead_Particles()
{
	if (!m_Particles.empty())
	{
		m_Particles.erase(
			std::remove_if(m_Particles.begin(), m_Particles.end(),
				[](const ATTRIBUTE& particle) { return !particle.bIsAlive; }),
			m_Particles.end());
	}
}

CParticle_System::CParticle_System(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent{ pGraphic_Device }
{

}

CParticle_System::CParticle_System(const CParticle_System& Prototype)
	: CComponent{ Prototype },
	m_Bounding_Box{Prototype.m_Bounding_Box},
	m_fEmit_Rate{ Prototype.m_fEmit_Rate },
	m_fSize{ Prototype.m_fSize },
	m_iMaxParticles{ Prototype.m_iMaxParticles },
	m_VBSize{ Prototype.m_VBSize },
	m_VBOffset{ Prototype.m_VBOffset },
	m_VBBatchSize{ Prototype.m_VBBatchSize }
{
}

HRESULT CParticle_System::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticle_System::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		PARTICLEDESC desc = *reinterpret_cast<PARTICLEDESC*>(pArg);
		m_VBSize = desc.VBSize;
	}
	HRESULT hr = m_pGraphic_Device->CreateVertexBuffer(
		m_VBSize * sizeof(PARTICLE),
		D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
		D3DFVF_XYZ | D3DFVF_DIFFUSE,
		D3DPOOL_DEFAULT,
		&m_VB, 0);
	return hr;
}

void CParticle_System::Priority_Update(_float fTimeDelta)
{
}

void CParticle_System::Update(_float fTimeDelta)
{
}

void CParticle_System::Late_Update(_float fTimeDelta)
{
}

void CParticle_System::Add_Particle()
{
	ATTRIBUTE attribute = {};
	Reset_Particle(&attribute);
	
	m_Particles.push_back(attribute);

}

void CParticle_System::Reset()
{
	for (auto& iter : m_Particles)
	{
		Reset_Particle(&iter);
	}
}

void CParticle_System::Reset_Particle(ATTRIBUTE* pAttribute)
{
	
}

HRESULT CParticle_System::Pre_Render()
{
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, false);
	m_pGraphic_Device->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
	m_pGraphic_Device->SetRenderState(D3DRS_POINTSCALEENABLE, true);
	m_pGraphic_Device->SetRenderState(D3DRS_POINTSIZE, FtoDW(m_fSize));

	m_pGraphic_Device->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDW(0.0f));

	m_pGraphic_Device->SetRenderState(D3DRS_POINTSCALE_A, FtoDW(0.0f));
	m_pGraphic_Device->SetRenderState(D3DRS_POINTSCALE_B, FtoDW(0.0f));
	m_pGraphic_Device->SetRenderState(D3DRS_POINTSCALE_C, FtoDW(1.0f));
	//m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG); // 정점 색상만 사용
	//m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);    // 정점 색상 지정

	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1); // 텍스처 알파값 사용
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);  // 텍스처에서 알파값 가져옴

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	return S_OK;
}

HRESULT CParticle_System::Render()
{
	Pre_Render();
	if (!m_Particles.empty())
	{
		m_pGraphic_Device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
		m_pGraphic_Device->SetStreamSource(0, m_VB, 0, sizeof(PARTICLE));

		if (m_VBOffset >= m_VBSize)
			m_VBOffset = 0;

		PARTICLE* v = 0;

		m_VB->Lock(
			m_VBOffset * sizeof(PARTICLE),
			m_VBBatchSize * sizeof(PARTICLE),
			(void**)&v,
			m_VBOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

		DWORD numParticlesInBatch = 0;
		list<ATTRIBUTE>::iterator i = m_Particles.begin();
		while (i != m_Particles.end())
		{
			if (i->bIsAlive)
			{
				v->vPosition = i->vPosition;
				v->vColor = (D3DCOLOR)i->vColor;
				v++;
				numParticlesInBatch++;

				if (numParticlesInBatch == m_VBBatchSize)
				{
					m_VB->Unlock();

					m_pGraphic_Device->DrawPrimitive(D3DPT_POINTLIST, m_VBOffset, m_VBBatchSize);

					m_VBOffset += m_VBBatchSize;

					if (m_VBOffset >= m_VBSize) m_VBOffset = 0;

					m_VB->Lock(
						m_VBOffset * sizeof(PARTICLE),
						m_VBBatchSize * sizeof(PARTICLE),
						(void**)&v,
						m_VBOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

					numParticlesInBatch = 0;
				}
			}
			i++;
		}

		m_VB->Unlock();

		if (numParticlesInBatch)
		{
			m_pGraphic_Device->DrawPrimitive(D3DPT_POINTLIST, m_VBOffset, numParticlesInBatch);
		}

		m_VBOffset += m_VBBatchSize;
	}

	Post_Render();
	return S_OK;
}

HRESULT CParticle_System::Post_Render()
{
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, true);
	m_pGraphic_Device->SetRenderState(D3DRS_POINTSPRITEENABLE, false);
	m_pGraphic_Device->SetRenderState(D3DRS_POINTSCALEENABLE, false);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

	return S_OK;
}

CParticle_System* CParticle_System::Clone(void* pArg)
{
	return nullptr;
}

void CParticle_System::Free()
{
	__super::Free();
	Safe_Release(m_VB);
}