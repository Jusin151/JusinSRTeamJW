#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CLight final : public CComponent
{
	enum LIGHTTYPE {LT_DIR, LT_POINT, LT_SPOT, LT_END};
private:
	CLight(PDIRECT3DDEVICE9 pGraphic_Device);
	CLight(const CLight& Prototype);
	virtual ~CLight() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	HRESULT Bind_Resouce(_uint iIndex);
	virtual void Reset() {}; // 차후에 오브젝트 풀링때 SetActive가 true가 될 때 기본정보 다시 셋팅을 위한 함수
	
private:
	LIGHTTYPE m_eType = {};
	_float4 m_fDiffuse = {};
	_float4 m_fAmbient = {};
	_float4 m_fSpecular = {};
public:
	static CLight*	Create(LPDIRECT3DDEVICE9 pGraphic_Device, LIGHTTYPE eType);
	virtual CLight* Clone(void* pArg) override;
	virtual void	Free() override;
};

END

