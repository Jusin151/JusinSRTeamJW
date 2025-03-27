#pragma once

#include "Collider.h"

BEGIN(Engine)

class ENGINE_DLL CCollider_Sphere : public CCollider
{
private:
	CCollider_Sphere(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCollider_Sphere(const CCollider_Sphere& Prototype);
	virtual ~CCollider_Sphere() = default;



public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render() override;

	virtual HRESULT Update_Collider(const _wstring& strLayerTag, _float3 fScale) override;
private:
	
	 

public:

	static CCollider_Sphere* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
