#pragma once
#include "Base.h"
BEGIN(Engine)
// 구 형태의 경계 볼륨을 정의하는 구조체
typedef struct tagBoundingSphere
{
    D3DXVECTOR3 m_vCenter; // 구의 중심점 좌표
    float       m_fRadius; // 구의 반지름

    // 기본 생성자: 중심 (0,0,0), 반지름 0으로 초기화
    tagBoundingSphere() : m_vCenter(0.f, 0.f, 0.f), m_fRadius(0.f) {}

    // 중심과 반지름을 지정하는 생성자
    tagBoundingSphere(const D3DXVECTOR3& center, float radius) : m_vCenter(center), m_fRadius(radius) {}

    /**
     * @brief 주어진 점이 구 내부에 있는지 확인합니다.
     * @param p 확인할 점의 좌표
     * @return 점이 구 내부에 있으면 true, 그렇지 않으면 false
     */
    bool Is_Point_Inside(const D3DXVECTOR3& p) const
    {
        // 중심점에서 주어진 점까지의 벡터 계산
        D3DXVECTOR3 diff = p - m_vCenter;

        // 벡터의 길이 제곱 계산 (sqrt 보다 효율적)
        float distSq = D3DXVec3LengthSq(&diff);

        // 거리 제곱이 반지름 제곱보다 작거나 같으면 구 내부에 있음
        return distSq <= (m_fRadius * m_fRadius);
    }

    // (선택 사항) AABB로부터 최소 경계 구를 계산하는 함수
    static tagBoundingSphere CreateFromAABB(const tagBoundingBox& box)
    {
        tagBoundingSphere sphere;
        // AABB의 중심을 구의 중심으로 설정
        sphere.m_vCenter = (box.m_vMin + box.m_vMax) * 0.5f;

        // AABB의 대각선 절반 길이를 반지름으로 설정 (가장 간단한 방법)
        // 더 정확한 최소 경계 구 계산은 복잡할 수 있음
        D3DXVECTOR3 halfDiagonal = (box.m_vMax - box.m_vMin) * 0.5f;
        sphere.m_fRadius = D3DXVec3Length(&halfDiagonal);

        return sphere;
    }

} BOUNDINGSPHERE;
END