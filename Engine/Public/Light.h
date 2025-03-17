#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CLight final : public CGameObject
{
	enum LightType {LT_DIR, LT_POINT, LT_SPOT, LT_END};
private:
	CLight();
	CLight(const CLight& Prototype);
	virtual ~CLight() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Reset() {}; // 차후에 오브젝트 풀링때 SetActive가 true가 될 때 기본정보 다시 셋팅을 위한 함수
	
private:
	_float4 m_fDiffuse = {};
	_float4 m_fAmbient = {};
	_float4 m_fSpecular = {};
public:
	virtual CLight* Clone(void* pArg) override;
	virtual void	Free() override;
};

END

