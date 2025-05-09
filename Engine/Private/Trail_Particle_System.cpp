﻿#include "Trail_Particle_System.h"

CTrail_Particle_System::CTrail_Particle_System(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CParticle_System{ pGraphic_Device }
{
	
}

CTrail_Particle_System::CTrail_Particle_System(const CTrail_Particle_System& Prototype)
	: CParticle_System { Prototype }
	, m_vDir{ Prototype.m_vDir }
	, m_fWidth{ Prototype.m_fWidth }
	, m_fDistance{ Prototype.m_fDistance }
{

}

HRESULT CTrail_Particle_System::Initialize_Prototype()
{
 	return S_OK;
}

HRESULT CTrail_Particle_System::Initialize(void* pArg)
{
	TRAILDESC desc = *reinterpret_cast<TRAILDESC*>(pArg);
	m_vPos = { 0.f, 0.f, 0.f };
	m_fSize = 0.8f;
	m_VBSize = 2048;
	m_VBOffset = 0;
	m_VBBatchSize = 512;
	m_iMaxParticles = desc.iNumParticles;
	m_fWidth = desc.fWidth;
	m_fDistance = desc.fDistance;

	PARTICLEDESC pDesc = { m_VBSize, desc.strShaderPath, desc.strTexturePath };

	if (FAILED(__super::Initialize(&pDesc)))
		return E_FAIL;
	Add_Particle();
	return S_OK;
}

void CTrail_Particle_System::Reset_Particle(ATTRIBUTE* pAttribute, void* pArg)
{
	pAttribute->bIsAlive = true;
	pAttribute->fAge = 0;
	pAttribute->vPosition = m_vDir * m_fDistance;
	pAttribute->vVelocity = -m_vDir;
	pAttribute->fLifetime = 2.0f;

	pAttribute->fSize = m_fSize;
	pAttribute->vCurrentColor = D3DCOLOR_COLORVALUE(1.0f, 1.0f, 1.0f, 1.0f);
	pAttribute->vColorFade = D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f, 0.0f);
	pAttribute->iIndex = 0; //rand() % m_pTexture->Get_NumTextures();
}

void CTrail_Particle_System::Update(float fTimeDelta)
{
	Reset_Particle(&*m_Particles.begin());
	for (auto& i : m_Particles)
	{
		if (i.bIsAlive)
		{
			i.vPosition += i.vVelocity * fTimeDelta;
		}
	}
	__super::Late_Update(fTimeDelta);
}

HRESULT CTrail_Particle_System::Render()
{
	__super::Pre_Render();

	PARTICLE* v = 0;

	m_pTexture->Bind_Resource(0);
	m_pGraphic_Device->SetFVF(D3DFVF_XYZ | D3DFVF_PSIZE | D3DFVF_DIFFUSE | D3DFVF_TEX1);
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
	//v[0].fSize = 1.f;
	v[0].vColor = i.vCurrentColor;
	v[0].vTexCoord = { 0, 0 };
	v[1].vPosition = m_vPos + vNormal * m_fWidth / 2.f;
	//v[1].fSize = 1.f;
	v[1].vColor = i.vCurrentColor;
	v[1].vTexCoord = { 1, 0 };

	v[2].vPosition = i.vPosition - vNormal * m_fWidth / 2.f;
	//v[2].fSize = 1.f;
	v[2].vColor = i.vColorFade;
	v[2].vTexCoord = { 0, 1 };

	v[3].vPosition = i.vPosition + vNormal * m_fWidth / 2.f;
	//v[3].fSize = 1.f;
	v[3].vColor = i.vColorFade;
	v[3].vTexCoord = { 1, 1 };
	m_PointVB->Unlock();
	m_pGraphic_Device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);


	__super::Post_Render();
	return S_OK;
}

CTrail_Particle_System* CTrail_Particle_System::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTrail_Particle_System* pInstance = new CTrail_Particle_System(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTrail_Particle_System");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CTrail_Particle_System* CTrail_Particle_System::Clone(void* pArg)
{
	CTrail_Particle_System* pInstance = new CTrail_Particle_System(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CTrail_Particle_System");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrail_Particle_System::Free()
{
	__super::Free();
}
