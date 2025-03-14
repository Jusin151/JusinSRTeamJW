#include "PickingSys.h"
#include "Transform.h"
#include "Collider_Sphere.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CPickingSys)
CPickingSys::CPickingSys()
{
}

HRESULT CPickingSys::Initialize(HWND hWnd, LPDIRECT3DDEVICE9 pGraphic_Device, CGameInstance* pGameInstance)
{
	m_Hwnd = hWnd;
	m_pGraphic_Device = pGraphic_Device;
	m_pGameInstance = pGameInstance;
	Safe_AddRef(m_pGraphic_Device);
	Safe_AddRef(m_pGameInstance);
	return S_OK;
}

void CPickingSys::Update()
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(m_Hwnd, &pt);
	CalcPickingRay(pt.x, pt.y);

	auto colliderVec = m_pGameInstance->Get_Colliders();

	for (auto& colliderList : colliderVec)
	{
		for (auto& collider : colliderList)
		{
			if (GetKeyState(VK_LBUTTON) & 0x8000)
			{

			if (Spherelntersection(static_cast<CCollider_Sphere*>(collider)))
			{

				int a = 10;
			}
		}
			}
	}
}

_bool CPickingSys::Spherelntersection(CCollider_Sphere* pColliderSp)
{
	if (pColliderSp == nullptr) return false;
	_float3 v = m_Ray.vOrigin - pColliderSp->Get_State(CTransform::STATE_POSITION);

	_float a = m_Ray.vDir.Dot(m_Ray.vDir);     // a = 1 (정규화된 방향 벡터라면)
	_float b = 2.0f * m_Ray.vDir.Dot(v);
	//_float c = v.Dot(v) - (pColliderSp->Get_Radius() * pColliderSp->Get_Radius());
	_float c = v.Dot(v) - (pColliderSp->Get_Radius());
	_float discriminant = (b * b) - (4.0f * a * c);
	if (discriminant < 0.f)
		return false;


	discriminant = sqrtf(discriminant);
	float s0 = (-b + discriminant) / 2.0f;
	float s1 = (-b - discriminant) / 2.0f;
	if (s0 >= 0.f || s1 >= 0.f)
		return true;


	return false;
}

HRESULT CPickingSys::CalcPickingRay(_ulong iX, _ulong iY)
{
	if (!m_pCamTransform) return E_FAIL;

	_float px = 0.f;
	_float py = 0.f;
	D3DVIEWPORT9 vp;
	m_pGraphic_Device->GetViewport(&vp);
	_float4x4 proj;
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &proj);
	px = (((2.0f * iX) / vp.Width) - 1.f) / proj(0, 0);
	py = (((-2.0f * iY) / vp.Height) + 1.f) / proj(1, 1);

	m_Ray.vOrigin = _float3(0.f, 0.f, 0.f);
	m_Ray.vDir = _float3(px, py, 1.f);

	// 카메라의 월드 위치를 Ray의 원점으로 설정
	m_Ray.vOrigin = m_pCamTransform->Get_State(CTransform::STATE_POSITION);

	// 월드 방향 계산: 카메라의 right, up, look 벡터를 사용하여 뷰 공간 방향을 월드 공간으로 변환
	_float3 vRight = m_pCamTransform->Get_State(CTransform::STATE_RIGHT);
	_float3 vUp = m_pCamTransform->Get_State(CTransform::STATE_UP);
	_float3 vLook = m_pCamTransform->Get_State(CTransform::STATE_LOOK);

	// 뷰 공간 방향 벡터를 월드 공간 방향 벡터로 변환
	m_Ray.vDir = vRight * px + vUp * py + vLook * m_Ray.vDir.z;
	m_Ray.vDir.Normalize(); // 방향 벡터 정규화, 월드로 바꾸기

	return S_OK;
}

void CPickingSys::Free()
{
	__super::Free();

	Safe_Release(m_pCamTransform);
	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_pGameInstance);
}
