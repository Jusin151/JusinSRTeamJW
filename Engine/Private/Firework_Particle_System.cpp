#include "Firework_Particle_System.h"

CFirework_Particle_System::CFirework_Particle_System(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CParticle_System{ pGraphic_Device }
{
	
}

CFirework_Particle_System::CFirework_Particle_System(const CParticle_System& Prototype)
	: CParticle_System { Prototype }
{

}

HRESULT CFirework_Particle_System::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFirework_Particle_System::Initialize(void* pArg)
{
	FIREWORKDESC desc = *reinterpret_cast<FIREWORKDESC*>(pArg);
	m_Bounding_Box = desc.Bounding_Box;
	m_vPos = desc.vOrigin;
	m_fSize = desc.fSize;
	m_VBSize = 2048;
	m_VBOffset = 0;
	m_VBBatchSize = 512;
	m_iMaxParticles = desc.iNumParticles;

	

	//PARTICLEDESC pDesc = { m_VBSize, desc.strShaderPath, desc.strTexturePath, };

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	for (_uint i = 0; i < desc.iNumParticles; ++i)
	{
		Add_Particle();
	}
	return S_OK;
}

void CFirework_Particle_System::Reset_Particle(ATTRIBUTE* pAttribute)
{
	pAttribute->bIsAlive = true;
	pAttribute->vPosition = m_vPos;

	GetRandomVector(&pAttribute->vVelocity, &m_Bounding_Box.m_vMin, &m_Bounding_Box.m_vMax);

	D3DXVec3Normalize(&pAttribute->vVelocity, &pAttribute->vVelocity);

	pAttribute->vColor = D3DXCOLOR(
		GetRandomFloat(0.0f, 1.0f),
		GetRandomFloat(0.0f, 1.0f),
		GetRandomFloat(0.0f, 1.0f),
		1.0f);

	pAttribute->fAge = 0.0f;
	pAttribute->fLifetime = 2.0f; // lives for 2 seconds
	pAttribute->fSize = m_fSize;
	pAttribute->iIndex = 0; //rand() % m_pTexture->Get_NumTextures();
}

void CFirework_Particle_System::Update(float fTimeDelta)
{
	for (auto& i : m_Particles)
	{
		if (i.bIsAlive)
		{
			i.vPosition += i.vVelocity * fTimeDelta;
			_float4 temp = DWORDToFloat4_Color(i.vColor);
			temp.w *= 0.99f;
			i.vColor = D3DCOLOR_COLORVALUE(temp.x, temp.y, temp.z, temp.w);
			i.fAge += fTimeDelta;

			if (i.fAge > i.fLifetime)
				i.bIsAlive = false;
		}
	}

	__super::Late_Update(fTimeDelta);
}

HRESULT CFirework_Particle_System::Pre_Render()
{
	__super::Pre_Render();

	//m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	//m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); // 알파 값이 기준보다 크면 픽셀 렌더링
	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 155); // 기준값 설정 (0~255)

	// read, but don't write particles to z-buffer
	//m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, false);

	return S_OK;
}

HRESULT CFirework_Particle_System::Post_Render()
{
	__super::Post_Render();

	//m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, true);
	return S_OK;
}

CFirework_Particle_System* CFirework_Particle_System::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CFirework_Particle_System* pInstance = new CFirework_Particle_System(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFirework_Particle_System");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CFirework_Particle_System* CFirework_Particle_System::Clone(void* pArg)
{
	CFirework_Particle_System* pInstance = new CFirework_Particle_System(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CFirework_Particle_System");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFirework_Particle_System::Free()
{
	__super::Free();
}
