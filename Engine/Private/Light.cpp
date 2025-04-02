#include "Light.h"

CLight::CLight(PDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent { pGraphic_Device }
{
}

CLight::CLight(const CLight& Prototype)
	:	CComponent{ Prototype }
	,	m_tInfo{ Prototype.m_tInfo }
{
}

HRESULT CLight::Initialize_Prototype(LIGHTTYPE eType)
{
	m_tInfo.eType			= eType; // 빛 타입은 이미 설정되어 있다고 가정
	m_tInfo.fDiffuse		= { 1.0f, 1.0f, 1.0f, 1.0f }; // 흰색 확산광
	m_tInfo.fAmbient		= { 0.2f, 0.2f, 0.2f, 1.0f }; // 약한 주변광
	m_tInfo.fSpecular		= { 1.0f, 1.0f, 1.0f, 1.0f }; // 흰색 반사광
	m_tInfo.fPosition		= { 0.0f, 10.0f, 0.0f }; // 빛의 위치 (위에서 아래로 비추는 방향을 가정)
	m_tInfo.fDirection		= { 0.0f, -1.0f, 0.0f }; // 빛의 방향 (아래쪽)
	m_tInfo.fAttenuation0	= 1.0f; // 감쇠 상수
	m_tInfo.fAttenuation1	= 0.0f; // 감쇠 선형
	m_tInfo.fAttenuation2	= 0.0f; // 감쇠 2차
	m_tInfo.fFalloff		= 1.0f; // 스포트라이트 감쇠
	m_tInfo.fRange			= 100.0f; // 빛의 범위
	m_tInfo.fTheta			= D3DX_PI / 4; // 스포트라이트 내부 각도 (45도)
	m_tInfo.fPhi			= D3DX_PI / 2; // 스포트라이트 외부 각도 (90도)
	return S_OK;
}

HRESULT CLight::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		//LIGHT_DESC in = *reinterpret_cast<LIGHT_DESC*>(pArg);
		//memcpy(&m_tInfo, &in, sizeof(LIGHT_DESC));
		LIGHT_INIT file = *reinterpret_cast<LIGHT_INIT*>(pArg);
		ifstream f(file.strLightPath);
		json data = json::parse(f);
		if (data["Type"] == "Point") m_tInfo.eType = LT_POINT;
		else if (data["Type"] == "Spot") m_tInfo.eType = LT_SPOT;
		else if (data["Type"] == "Direction") m_tInfo.eType = LT_DIR;
		m_tInfo.fDiffuse = { data["Diffuse"]["x"], data["Diffuse"]["y"], data["Diffuse"]["z"], data["Diffuse"]["w"] };
		m_tInfo.fAmbient = { data["Ambient"]["x"], data["Ambient"]["y"], data["Ambient"]["z"], data["Ambient"]["w"] };
		m_tInfo.fSpecular = { data["Specular"]["x"], data["Specular"]["y"], data["Specular"]["z"], data["Specular"]["w"] };
		m_tInfo.fDirection = { data["Direction"]["x"], data["Direction"]["y"], data["Direction"]["z"] };
		m_tInfo.fAttenuation0 = { data["Attenuation0"] };
		m_tInfo.fAttenuation1 = { data["Attenuation1"] };
		m_tInfo.fAttenuation2 = { data["Attenuation2"] };
		m_tInfo.fFalloff = { data["Falloff"] };
		m_tInfo.fRange = { data["Range"] };
		m_tInfo.fTheta = { D3DX_PI / data["Theta"] };
		m_tInfo.fPhi = { D3DX_PI / data["Phi"] };
	}
	return S_OK;
}

HRESULT CLight::Bind_Resouce(_uint iIndex)
{
	D3DLIGHT9 light = {};
	light.Type = (D3DLIGHTTYPE)m_tInfo.eType;
	memcpy(&light.Diffuse,		&m_tInfo.fDiffuse,		sizeof(_float4));
	memcpy(&light.Ambient,		&m_tInfo.fAmbient,		sizeof(_float4));
	memcpy(&light.Specular,		&m_tInfo.fSpecular,		sizeof(_float4));
	memcpy(&light.Position,		&m_tInfo.fPosition,		sizeof(_float3));
	memcpy(&light.Direction,	&m_tInfo.fDirection,	sizeof(_float3));
	memcpy(&light.Attenuation0, &m_tInfo.fAttenuation0,	sizeof(_float));
	memcpy(&light.Attenuation1, &m_tInfo.fAttenuation1,	sizeof(_float));
	memcpy(&light.Attenuation2, &m_tInfo.fAttenuation2,	sizeof(_float));
	memcpy(&light.Falloff,		&m_tInfo.fFalloff,		sizeof(_float));
	memcpy(&light.Range,		&m_tInfo.fRange,		sizeof(_float));
	memcpy(&light.Theta,		&m_tInfo.fTheta,		sizeof(_float));
	memcpy(&light.Phi,			&m_tInfo.fPhi,			sizeof(_float));

	m_pGraphic_Device->SetLight(iIndex, &light);

	return S_OK;
}

void CLight::DecreaseIntensity(_uint iFrame)
{
	//0123
	m_tInfo.fAttenuation1 = iFrame;
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
