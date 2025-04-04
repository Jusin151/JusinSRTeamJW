#include "PickingSys.h"
#include "Transform.h"
#include "Collider_Cube.h"
#include "Collider_Sphere.h"
#include "GameInstance.h"
#include "CollisionObject.h"

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

	auto colliderVec = m_pGameInstance->Get_Colliders();
	return S_OK;
}
void CPickingSys::Update()
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(m_Hwnd, &pt);
	CalcPickingRay(pt.x, pt.y);
}

_bool CPickingSys::Cube_Intersection(CCollider_Cube* pColliderCu)
{
	if (pColliderCu == nullptr || pColliderCu->Get_Owner() == m_pPlayer) return false;


    _float fScaleX = pColliderCu->Get_Desc().fAxisX.Length();
    _float fScaleY = pColliderCu->Get_Desc().fAxisY.Length();
    _float fScaleZ = pColliderCu->Get_Desc().fAxisZ.Length();

    // 방향 벡터 정규화
    _float3 vDirX = pColliderCu->Get_Desc().fAxisX.GetNormalized();
    _float3 vDirY = pColliderCu->Get_Desc().fAxisY.GetNormalized();
    _float3 vDirZ = pColliderCu->Get_Desc().fAxisZ.GetNormalized();

    // 스케일이 적용된 축 벡터 생성
    _float fHalfSize[3] = {
        fScaleX * 0.5f,  // 0.5f는 중심에서 가장자리까지의 거리
        fScaleY * 0.5f,
        fScaleZ * 0.5f
    };
    _float3 vDir[3] = {
      vDirX,
      vDirY,
      vDirZ
    };

    // OBB의 로컬 좌표계로 변환된 광선 원점
    _float3 vLocalOrigin = m_Ray.vOrigin - pColliderCu->Get_Desc().fPos;

    // 로컬 좌표계 기준 광선 방향과 원점 계산
    _float3 vLocalDir, vLocalPos;
    for (int i = 0; i < 3; i++)
    {
        vLocalDir[i] = m_Ray.vDir.Dot(vDir[i]);
        vLocalPos[i] = vLocalOrigin.Dot(vDir[i]);
    }

    //  각 축별 슬랩 교차 검사
    _float fMin = -FLT_MAX;
    _float fMax = FLT_MAX;


    for (int i = 0; i < 3; i++)
    {
        if (abs(vLocalDir[i]) < FLT_EPSILON)
        {
            // 광선이 이 축과 평행하면, 원점이 박스 범위 내에 있어야 함
            if (vLocalPos[i] < -fHalfSize[i] || vLocalPos[i] > fHalfSize[i])
                return false;
        }
        else
        {
            // t 값 계산 (슬랩 진입/이탈 지점)
            _float t1 = (-fHalfSize[i] - vLocalPos[i]) / vLocalDir[i];
            _float t2 = (fHalfSize[i] - vLocalPos[i]) / vLocalDir[i];

            // t1이 항상 더 작은 값이 되도록 함
            if (t1 > t2)
            {
                _float temp = t1;
                t1 = t2;
                t2 = temp;
            }

            // 교차 구간 업데이트
            fMin = max(fMin, t1);
            fMax = min(fMax, t2);

            // 교차 구간이 유효하지 않으면 충돌 없음
            if (fMin > fMax)
                return false;
        }
    }

    // 모든 슬랩을 통과하면 충돌 발생
    return fMin <= fMax && fMax >= 0.0f;
}

_bool CPickingSys::Ray_Intersection(CCollider* pCollider)
{
    CCollider_Sphere*  pSphere = dynamic_cast<CCollider_Sphere*>(pCollider);

    if (pSphere)
    {
        return Sphere_lntersection(pSphere);
    }

    CCollider_Cube* pCube = dynamic_cast<CCollider_Cube*>(pCollider);

    if (pCube)
    {
        return Cube_Intersection(pCube);
    }

    return false;
}

// Ray_Intersection()를 오버로드하거나 시그니처 수정
bool CPickingSys::Ray_Intersection(CCollider* pCollider, _float3* pOutHitPos)
{
    // 예시: Sphere라면
    CCollider_Sphere* pSphere = dynamic_cast<CCollider_Sphere*>(pCollider);
    if (pSphere)
    {
        // (1) 판별식(discriminant) 구하기
        _float3 vCenter = pSphere->Get_State(CTransform::STATE_POSITION);
        _float3 vOc = m_Ray.vOrigin - vCenter;
        float r = pSphere->Get_Radius(); // 구의 반지름

        float a = m_Ray.vDir.Dot(m_Ray.vDir);        // 보통 1
        float b = 2.f * m_Ray.vDir.Dot(vOc);
        float c = vOc.Dot(vOc) - r * r;
        float discriminant = b * b - 4.f * a * c;
        if (discriminant < 0.f)
            return false; // 충돌X

        // (2) t값 2개 중 하나 선택(가장 작은 양수)
        discriminant = sqrtf(discriminant);
        float t1 = (-b - discriminant) / (2.f * a);
        float t2 = (-b + discriminant) / (2.f * a);

        // 둘 중 더 작은 양수 t를 채택
        float t = -1.f;
        if (t1 >= 0.f && t2 >= 0.f) t = min(t1, t2);
        else if (t1 >= 0.f)        t = t1;
        else if (t2 >= 0.f)        t = t2;
        else return false; // 둘 다 음수 => 뒤쪽 충돌

        // (3) 진짜 충돌 위치
        _float3 vHitPos = m_Ray.vOrigin + m_Ray.vDir * t;
        if (pOutHitPos)
            *pOutHitPos = vHitPos;

        return true;
    }

    CCollider_Cube* pCube = dynamic_cast<CCollider_Cube*>(pCollider);
    if (pCube)
    {
        // pCube의 축 정보와 반치 크기를 가져옴
        _float3 vDirX = pCube->Get_Desc().fAxisX.GetNormalized();
        _float3 vDirY = pCube->Get_Desc().fAxisY.GetNormalized();
        _float3 vDirZ = pCube->Get_Desc().fAxisZ.GetNormalized();
        _float fHalfSize[3] = {
            pCube->Get_Desc().fAxisX.Length() * 0.5f,
            pCube->Get_Desc().fAxisY.Length() * 0.5f,
            pCube->Get_Desc().fAxisZ.Length() * 0.5f
        };

        // OBB의 로컬 좌표계로 변환: pCube->Get_Desc().fPos는 OBB 중심 위치
        _float3 vLocalOrigin = m_Ray.vOrigin - pCube->Get_Desc().fPos;

        _float3 vLocalDir, vLocalPos;
        vLocalDir.x = m_Ray.vDir.Dot(vDirX);
        vLocalDir.y = m_Ray.vDir.Dot(vDirY);
        vLocalDir.z = m_Ray.vDir.Dot(vDirZ);
        vLocalPos.x = vLocalOrigin.Dot(vDirX);
        vLocalPos.y = vLocalOrigin.Dot(vDirY);
        vLocalPos.z = vLocalOrigin.Dot(vDirZ);

        _float fMin = -FLT_MAX;
        _float fMax = FLT_MAX;
        for (int i = 0; i < 3; i++)
        {
            float localDir = (&vLocalDir.x)[i];
            float localPos = (&vLocalPos.x)[i];
            float halfSize = fHalfSize[i];

            if (fabs(localDir) < FLT_EPSILON)
            {
                if (localPos < -halfSize || localPos > halfSize)
                    return false;
            }
            else
            {
                float t1 = (-halfSize - localPos) / localDir;
                float t2 = (halfSize - localPos) / localDir;
                if (t1 > t2)
                {
                    float temp = t1;
                    t1 = t2;
                    t2 = temp;
                }
                fMin = max(fMin, t1);
                fMax = min(fMax, t2);
                if (fMin > fMax)
                    return false;
            }
        }
        if (fMin <= fMax && fMax >= 0.0f)
        {
            float t;
            if (fMin >= 0.0f)
                t = fMin;
            else
                t = fMax;
            if (pOutHitPos)
                *pOutHitPos = m_Ray.vOrigin + m_Ray.vDir * t;
            return true;
        }
        return false;
    }
    return false;
}

_float3 CPickingSys::Get_Mouse_Effect()
{
    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(m_Hwnd, &pt);

    D3DVIEWPORT9 vp;
    m_pGraphic_Device->GetViewport(&vp);

    D3DXMATRIX matProj, matView, matWorld;
    m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &matProj);
    m_pGraphic_Device->GetTransform(D3DTS_VIEW, &matView);
    D3DXMatrixIdentity(&matWorld);

    D3DXVECTOR3 vMousePos((float)pt.x, (float)pt.y, 0.0f);
    D3DXVECTOR3 vMousePosFar((float)pt.x, (float)pt.y, 1.0f);

    D3DXVECTOR3 vRayOrigin, vRayDir;
    D3DXVec3Unproject(&vRayOrigin, &vMousePos, &vp, &matProj, &matView, &matWorld);
    D3DXVec3Unproject(&vRayDir, &vMousePosFar, &vp, &matProj, &matView, &matWorld);
    vRayDir -= vRayOrigin;
    D3DXVec3Normalize(&vRayDir, &vRayDir);

    // Assuming the ray intersects with a plane at z = 0
    float t = -vRayOrigin.z / vRayDir.z;
    D3DXVECTOR3 vIntersection = vRayOrigin + t * vRayDir;

    return _float3(vIntersection.x, vIntersection.y, vIntersection.z);

}
_bool CPickingSys::Sphere_lntersection(CCollider_Sphere* pColliderSp)
{
	if (pColliderSp == nullptr|| pColliderSp->Get_Owner() == m_pPlayer) return false;
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
