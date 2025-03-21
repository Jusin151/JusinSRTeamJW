#include "Light.h"

CLight::CLight(PDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent { pGraphic_Device }
{
}

CLight::CLight(const CLight& Prototype)
	:	CComponent{ Prototype },
		m_tInfo{ Prototype.m_tInfo }
{
}

HRESULT CLight::Initialize_Prototype(LIGHTTYPE eType)
{
	m_eType = eType;
	return S_OK;
}

HRESULT CLight::Initialize(void* pArg)
{
	LIGHT_DESC in = *reinterpret_cast<LIGHT_DESC*>(pArg);
	memcpy(&m_tInfo, &in, sizeof(LIGHT_DESC));
	return S_OK;
}

HRESULT CLight::Bind_Resouce(_uint iIndex)
{
	D3DLIGHT9 light = {};
	light.Type = (D3DLIGHTTYPE)m_eType;
	memcpy(&light.Diffuse,		&m_tInfo.m_fDiffuse,		sizeof(_float4));
	memcpy(&light.Ambient,		&m_tInfo.m_fAmbient,		sizeof(_float4));
	memcpy(&light.Specular,		&m_tInfo.m_fSpecular,		sizeof(_float4));
	memcpy(&light.Position,		&m_tInfo.m_fPosition,		sizeof(_float3));
	memcpy(&light.Direction,	&m_tInfo.m_fDirection,		sizeof(_float3));
	memcpy(&light.Attenuation0, &m_tInfo.m_fAttenuation0,	sizeof(_float));
	memcpy(&light.Attenuation1, &m_tInfo.m_fAttenuation1,	sizeof(_float));
	memcpy(&light.Attenuation2, &m_tInfo.m_fAttenuation2,	sizeof(_float));
	memcpy(&light.Falloff,		&m_tInfo.m_fFalloff,		sizeof(_float));
	memcpy(&light.Range,		&m_tInfo.m_fRange,			sizeof(_float));
	memcpy(&light.Theta,		&m_tInfo.m_fTheta,			sizeof(_float));
	memcpy(&light.Phi,			&m_tInfo.m_fPhi,			sizeof(_float));

	m_pGraphic_Device->SetLight(iIndex, &light);

	return S_OK;
}

CLight* CLight::Create(LPDIRECT3DDEVICE9 pGraphic_Device, LIGHTTYPE eType)
{
	CLight* pInstance = new CLight(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype(eType)))
	{
		MSG_BOX("Failed to Created : CLight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CLight* CLight::Clone(void* pArg)
{
	CLight* pInstance = new CLight(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CLight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLight::Free()
{
	__super::Free();
}
