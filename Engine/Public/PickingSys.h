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
	_bool Ray_Intersection(class CCollider* pCollider);
	_bool Ray_Intersection(CCollider* pCollider, _float3* pHitpos);
	bool RayPlaneIntersection(const _float3& vRayOrigin, const _float3& vRayDir, const _float3& vPlanePos, const _float3& vPlaneNormal, _float3* pOutHitPos);
	_float3 Get_Mouse_Effect();
	void Set_Player(class CCollisionObject* pPlayer) { m_pPlayer = pPlayer; }
	const _float3& Get_Mouse_WorldPos() const { return m_Ray.vOrigin; };
	const Ray& Get_Ray() const { return m_Ray; }

protected:
	LPDIRECT3DDEVICE9	 m_pGraphic_Device = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };
	class CTransform* m_pCamTransform = { nullptr };

private:
	HRESULT CalcPickingRay(_ulong iX, _ulong iY);
	_bool Sphere_lntersection(class CCollider_Sphere* pColliderSp);
	_bool Cube_Intersection(class CCollider_Cube* pColliderCu);

private:
	HWND m_Hwnd = { nullptr };
	Ray m_Ray;
	class CCollisionObject* m_pPlayer = { nullptr };
private:
	virtual void Free() override;
};

END