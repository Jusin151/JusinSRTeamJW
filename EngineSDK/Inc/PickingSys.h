#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CPickingSys final : public CBase
{
	DECLARE_SINGLETON(CPickingSys)
private:
	struct Ray
	{
		_float3 vOrigin;
		_float3 vDir;
	};
private:
	CPickingSys();
	virtual ~CPickingSys() = default;

public:
	virtual HRESULT Initialize(HWND hWnd, LPDIRECT3DDEVICE9 pGraphic_Device, class CGameInstance* pGameInstance);
	void Update();

public:
	void Set_CameraTransform(class CTransform* pCamTransform) { m_pCamTransform = pCamTransform; Safe_AddRef(m_pCamTransform); }
	_bool Spherelntersection(class CCollider_Sphere* pColliderSp);

protected:
	LPDIRECT3DDEVICE9	 m_pGraphic_Device = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };
	class CTransform* m_pCamTransform = { nullptr };

private:
	HRESULT CalcPickingRay(_ulong iX, _ulong iY);

private:
	HWND m_Hwnd = { nullptr };
	Ray m_Ray;

private:
	virtual void Free() override;
};

END