#include "Projectile_Particle_System.h"

CProjectile_Particle_System::CProjectile_Particle_System(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CParticle_System{ pGraphic_Device }
{
	
}

CProjectile_Particle_System::CProjectile_Particle_System(const CParticle_System& Prototype)
	: CParticle_System { Prototype }
{

}

HRESULT CProjectile_Particle_System::Initialize_Prototype()
{
 	return S_OK;
}

HRESULT CProjectile_Particle_System::Initialize(void* pArg)
{
	TRAILDESC desc = *reinterpret_cast<TRAILDESC*>(pArg);
	m_vPos = { 0.f, 0.f, 0.f };
	m_fSize = 0.8f;
	m_VBSize = 2048;
	m_VBOffset = 0;
	m_VBBatchSize = 512;
	m_iMaxParticles = desc.iNumParticles;
	m_fInterval = desc.fIntervaltime;
	m_fWidth = 2.f;

	PARTICLEDESC pDesc = { m_VBSize, desc.strShaderPath, desc.strTexturePath };

	if (FAILED(__super::Initialize(&pDesc)))
		return E_FAIL;
	Add_Particle();
	return S_OK;
}

void CProjectile_Particle_System::Reset_Particle(ATTRIBUTE* pAttribute)
{
	pAttribute->bIsAlive = true;
	pAttribute->fAge = 0;
	pAttribute->vPosition = m_vDir * 5.f;
	pAttribute->vVelocity = -m_vDir;
	pAttribute->fLifetime = 2.0f;

	pAttribute->vColor = D3DCOLOR_COLORVALUE(1.0f, 1.0f, 1.0f, 1.0f);
	pAttribute->vColorFade = D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f, 0.0f);
}

void CProjectile_Particle_System::Update(float fTimeDelta)
{
	Reset_Particle(&*m_Particles.begin());
	m_fElapsedTime += fTimeDelta;
	for (auto& i : m_Particles)
	{
		if (i.bIsAlive)
		{
			i.vPosition += i.vVelocity * fTimeDelta;
			//_float4 temp = DWORDToFloat4_Color(i.vColor);
			//temp.w *= 0.99f;
			//i.vColor = D3DCOLOR_COLORVALUE(temp.x, temp.y, temp.z, temp.w);
			i.fAge += fTimeDelta;

			/*if (i.fAge > i.fLifetime)
				i.bIsAlive = false;*/
		}
	}
	/*if (m_fElapsedTime >= 0.1f)
	{
		Add_Particle();
		m_fElapsedTime = 0.f;
	}
	if (m_Particles.size() > m_iMaxParticles)
	{
		m_Particles.pop_front();
	}*/

	__super::Late_Update(fTimeDelta);
}

HRESULT CProjectile_Particle_System::Render()
{
	__super::Pre_Render();

	PARTICLE* v = 0;

	m_pGraphic_Device->SetTexture(0, m_pTexture);
	m_pGraphic_Device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	m_pGraphic_Device->SetStreamSource(0, m_PointVB, 0, sizeof(PARTICLE));

	m_PointVB->Lock(
		m_VBOffset * sizeof(PARTICLE),
		m_VBBatchSize * sizeof(PARTICLE),
		(void**)&v,
		m_VBOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

	ATTRIBUTE i = *m_Particles.begin();
	_float3 vNormal = {};
	D3DXVec3Normalize(&vNormal, &m_vDir);
	D3DXVECTOR3 up = { 0.0f, 1.0f, 0.0f };
	D3DXVec3Cross(&vNormal, &m_vDir, &up);
	D3DXVec3Normalize(&vNormal, &vNormal);
	v[0].vPosition = m_vPos - vNormal * m_fWidth / 2.f;
	v[0].vColor = i.vColor;
	v[0].vTexCoord = { 0, 0 };
	v[1].vPosition = m_vPos + vNormal * m_fWidth / 2.f;
	v[1].vColor = i.vColor;
	v[1].vTexCoord = { 1, 0 };
	v[2].vPosition = i.vPosition - vNormal * m_fWidth / 2.f;
	v[2].vColor = i.vColorFade;
	v[2].vTexCoord = { 0, 1 };
	v[3].vPosition = i.vPosition + vNormal * m_fWidth / 2.f;
	v[3].vColor = i.vColorFade;
	v[3].vTexCoord = { 1, 1 };
	m_PointVB->Unlock();
	m_pGraphic_Device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);


	__super::Post_Render();
	return S_OK;
}

CProjectile_Particle_System* CProjectile_Particle_System::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CProjectile_Particle_System* pInstance = new CProjectile_Particle_System(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CProjectile_Particle_System");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CProjectile_Particle_System* CProjectile_Particle_System::Clone(void* pArg)
{
	CProjectile_Particle_System* pInstance = new CProjectile_Particle_System(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CProjectile_Particle_System");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CProjectile_Particle_System::Free()
{
	__super::Free();
}
