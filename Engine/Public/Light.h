#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CLight final : public CComponent
{
public:
	enum LIGHTTYPE {LT_POINT = 1, LT_SPOT, LT_DIR, LT_END};
	typedef struct tagLightFilePath
	{
		_wstring strLightPath;
	} LIGHT_INIT;
	typedef struct tagLightDesc{
		LIGHTTYPE	eType			= {};
		_float4		fDiffuse		= {};
		_float4		fAmbient		= {};
		_float4		fSpecular		= {};
		_float3		fPosition		= {};
		_float3		fDirection		= {};
		_float		fAttenuation0	= {};
		_float		fAttenuation1	= {};
		_float		fAttenuation2	= {};
		_float		fFalloff		= {};
		_float		fRange			= {};
		_float		fTheta			= {};
		_float		fPhi			= {};
	}LIGHT_DESC;
private:
	CLight(PDIRECT3DDEVICE9 pGraphic_Device);
	CLight(const CLight& Prototype);
	virtual ~CLight() = default;
public:
	void Set_Position(const _float3& position) {
		m_tInfo.fPosition = { position.x, position.y, position.z }; 
	}

public:
	virtual HRESULT Initialize_Prototype(LIGHTTYPE eType);
	virtual HRESULT Initialize(void* pArg);
public:
	HRESULT Bind_Resouce(_uint iIndex);
	virtual void Reset() {}; // 차후에 오브젝트 풀링때 SetActive가 true가 될 때 기본정보 다시 셋팅을 위한 함수

public:
	void DecreaseIntensity(_uint iFrame);
	
private:
	LIGHT_DESC	m_tInfo = {};
public:
	static CLight*	Create(LPDIRECT3DDEVICE9 pGraphic_Device, LIGHTTYPE eType);
	virtual CLight* Clone(void* pArg) override;
	virtual void	Free() override;
};

END

