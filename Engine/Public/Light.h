#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CLight final : public CComponent
{
	enum LIGHTTYPE {LT_POINT = 1, LT_SPOT, LT_DIR, LT_END};
	typedef struct tagLightDesc{
		LIGHTTYPE m_eType = {};
		_float4 m_fDiffuse = {};
		_float4 m_fAmbient = {};
		_float4 m_fSpecular = {};
		_float3 m_fPosition = {};
		_float3 m_fDirection = {};
		_float	m_fAttenuation0 = {};
		_float	m_fAttenuation1 = {};
		_float	m_fAttenuation2 = {};
		_float  m_fFalloff = {};
		_float	m_fRange = {};
		_float	m_fTheta = {};
		_float	m_fPhi = {};
	}LIGHT_DESC;
private:
	CLight(PDIRECT3DDEVICE9 pGraphic_Device);
	CLight(const CLight& Prototype);
	virtual ~CLight() = default;
public:
	void Set_Position(const _float3& position) { memcpy(&m_tInfo.m_fPosition, &position, sizeof(position)); }

public:
	virtual HRESULT Initialize_Prototype(LIGHTTYPE eType);
	virtual HRESULT Initialize(void* pArg);

public:
	HRESULT Bind_Resouce(_uint iIndex);
	virtual void Reset() {}; // 차후에 오브젝트 풀링때 SetActive가 true가 될 때 기본정보 다시 셋팅을 위한 함수
	
private:
	LIGHTTYPE	m_eType = {};
	LIGHT_DESC	m_tInfo = {};
public:
	static CLight*	Create(LPDIRECT3DDEVICE9 pGraphic_Device, LIGHTTYPE eType);
	virtual CLight* Clone(void* pArg) override;
	virtual void	Free() override;
};

END

