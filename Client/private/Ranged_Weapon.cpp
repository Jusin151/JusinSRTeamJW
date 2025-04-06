#include "Ranged_Weapon.h"
#include "GameInstance.h"
#include "PickingSys.h"
#include "Collider_Cube.h"
#include "Collider_Sphere.h"
#include "Effects.h"
#include "UI_Manager.h"
#include "Collider_Manager.h"
CRanged_Weapon::CRanged_Weapon(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CWeapon_Base{ pGraphic_Device }
{
}

CRanged_Weapon::CRanged_Weapon(const CRanged_Weapon& Prototype)
	: CWeapon_Base(Prototype)
{
}
void CRanged_Weapon::Priority_Update(_float fTimeDelta)
{
  /*  static _bool m_bOffItem = { false };

    if (!m_bOffItem)
    {
        m_bIsActive = false;

        m_bOffItem = true;
    }*/
}

void CRanged_Weapon::Update(_float fTimeDelta)
{
    __super::Move_Hand(fTimeDelta);

	

}

void CRanged_Weapon::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
   
    
}

HRESULT CRanged_Weapon::Render()
{
	D3DXMATRIX matOldView, matOldProj;
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &matOldView);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &matOldProj);

	D3DXMATRIX matView;
	D3DXMatrixIdentity(&matView);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &matView);

	D3DXMATRIX matProj;
	D3DXMatrixOrthoLH(&matProj, g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &matProj);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);


	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_Resource(m_iCurrentFrame)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &matOldView);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &matOldProj);


	return S_OK;
}
HRESULT CRanged_Weapon::Ready_Components()
{
	return E_NOTIMPL;
}
HRESULT CRanged_Weapon::Ready_Picking()
{
	m_pPickingSys = CPickingSys::Get_Instance();

	return S_OK;
}

void CRanged_Weapon::Free()
{
	__super::Free();
}


CGameObject* CRanged_Weapon::Clone(void* pArg)
{
	return nullptr;
}
void CRanged_Weapon::Move_Hand(_float fTimeDelta)
{
    __super::Move_Hand(fTimeDelta);
}



HRESULT CRanged_Weapon::Picking_Object(_uint EffectNum, _uint Damage)
{

    if (m_pPickingSys)
        m_pPickingSys->Update();

    // 가장 가까운 충돌 정보를 저장할 변수들
    CCollider* pClosestCollider = nullptr;
    _float fMinDist = m_fRange; // 최소 거리를 최대 사정거리로 초기화
    _float3 vClosestHitPos{};   // 가장 가까운 충돌 지점

    // 게임 인스턴스에서 모든 콜라이더 그룹 가져오기
    vector<list<CCollider*>> colliderGroups = m_pGameInstance->Get_Colliders();

    for (auto& group : colliderGroups)
    {
        for (auto* collider : group)
        {
       
            if (!collider || !collider->Get_Owner())
                continue;

     
            const wstring& tag = collider->Get_Owner()->Get_Tag();
            if (tag == L"Layer_Player" || tag.find(L"Floor") != wstring::npos||
                tag.find(L"Trigger") != wstring::npos)
                continue;

            _float3 vHitPos{};
            // 레이와 콜라이더의 교차 검사
            if (!m_pPickingSys->Ray_Intersection(collider, &vHitPos))
                continue;

            // 충돌 지점까지의 거리 계산
            _float3 vDiff = vHitPos - m_pPickingSys->Get_Ray().vOrigin;
            _float fDist = vDiff.Length();


            // 현재까지의 최소 거리보다 가까운 경우에만 정보 갱신
            if (fDist < fMinDist) // 현재 최소 거리보다 가까우면
            {
                fMinDist = fDist;             // 최소 거리 갱신
                pClosestCollider = collider;  // 가장 가까운 콜라이더 갱신
                vClosestHitPos = vHitPos;     // 가장 가까운 충돌 지점 갱신
            }
        }
    }

    // 가장 가까운 충돌체가 발견되었다면 처리
    if (pClosestCollider != nullptr)
    {
        // 가장 가까운 콜라이더의 소유 객체 태그 가져오기
        const wstring& closestTag = pClosestCollider->Get_Owner()->Get_Tag();

        // 태그에 따라 적절한 처리 수행
        if (closestTag.find(L"Wall") != wstring::npos)
        {
            // 벽과 충돌 처리 (EffectNum 변수는 여전히 필요합니다)
            Wall_Picking(pClosestCollider, EffectNum);
            m_bWall = true; // 필요하다면 플래그 설정
        }
        else if (closestTag.find(L"Ceil") != wstring::npos)
        {
            Wall_Picking(pClosestCollider, EffectNum);
            m_bWall = true; // 필요하다면 플래그 설정
        }
        else if ((closestTag.find(L"Monster") != wstring::npos))
        {
            CreateHitEffect(pClosestCollider, vClosestHitPos, Damage);
        
            m_bMonster = true; 
        }
        else if ((closestTag.find(L"Boss") != wstring::npos) || (closestTag == L"Layer_Cthulhu"))
        {
            CreateBossHitEffect(pClosestCollider, vClosestHitPos, Damage);

            m_bMonster = true;
        }
        if ((closestTag.find(L"Boss_Bullet") != wstring::npos))
        {
            CreateBossHitEffect(pClosestCollider, vClosestHitPos, Damage);

            m_bMonster = true;
        }
        // TODO: 다른 종류의 객체 태그에 대한 처리 로직 추가
    }

  

    return S_OK;
}
void CRanged_Weapon::Wall_Picking(CCollider* pCollider, _uint EffectNum)
{
    CTransform* pWallTransform = static_cast<CTransform*>(pCollider->Get_Owner()->Get_Component(L"Com_Transform"));
    if (!pWallTransform) return;

    _float3 vWallNormal = pWallTransform->Get_State(CTransform::STATE_LOOK).GetNormalized();
    _float3 vTilePos = pWallTransform->Get_State(CTransform::STATE_POSITION);
    _float3 vRayOrigin = m_pPickingSys->Get_Ray().vOrigin;
    _float3 vRayDir = m_pPickingSys->Get_Ray().vDir;

    float denom = vWallNormal.Dot(vRayDir);
    if (fabs(denom) < 1e-9f) return;

    float t = vWallNormal.Dot(vTilePos - vRayOrigin) / denom;
    if (t < 0.f) return;

    _float3 vPlaneHit = vRayOrigin + vRayDir * t;

    // 역행렬로 로컬 위치 변환
    _float4x4 worldMat = pWallTransform->Get_WorldMat();
    _float4x4 invWorld;
    if (D3DXMatrixInverse(&invWorld, nullptr, &worldMat) == nullptr)
        return;

    D3DXVECTOR3 vLocalHit;
    D3DXVec3TransformCoord(&vLocalHit, reinterpret_cast<D3DXVECTOR3*>(&vPlaneHit), &invWorld);

    if (vLocalHit.x < -0.55f || vLocalHit.x > 0.55f || vLocalHit.y < -0.55f || vLocalHit.y > 0.55f)
        return;
    

    CGameObject* pCamObj = m_pGameInstance->Find_Object(LEVEL_STATIC, L"Layer_Camera");
    if (!pCamObj) return;

    CTransform* pCamTransform = static_cast<CTransform*>(pCamObj->Get_Component(L"Com_Transform"));
    if (!pCamTransform) return;

    _float3 vCamPos = pCamTransform->Get_State(CTransform::STATE_POSITION);


    _float3 vToCamDir = vCamPos - vPlaneHit; 
    vToCamDir.Normalize(); 
    float dot = vWallNormal.Dot(vToCamDir);
    float offsetDir = (dot > 0.f) ? 1.f : -1.f;

    float wallThickness = pWallTransform->Compute_Scaled().z;
    _float3 vEffectPos = vPlaneHit + vWallNormal * offsetDir * (wallThickness * 0.5f + 0.01f);

#pragma region Effect 생성
    // 이펙트 생성
    CEffect_Base::EFFECT_DESC effectDesc;
    effectDesc.vRight = pWallTransform->Get_State(CTransform::STATE_RIGHT);
    effectDesc.vUp = pWallTransform->Get_State(CTransform::STATE_UP);
    effectDesc.vLook = pWallTransform->Get_State(CTransform::STATE_LOOK);
    effectDesc.vScale = { 0.5f, 0.5f, 0.5f };

    CHit_Effect::HIT_DESC hitDesc;
    hitDesc.vRight = effectDesc.vRight;
    hitDesc.vUp = effectDesc.vUp;
    hitDesc.vLook = effectDesc.vLook;
    hitDesc.vScale = effectDesc.vScale;
    hitDesc.type = rand() % 4;

    for (_uint i = 0; i < EffectNum; ++i)
    {
        _float offsetX = (((rand() % 100) / 100.f) - 0.5f) * 0.4f;
        _float offsetY = (((rand() % 100) / 100.f) - 0.5f) * 0.4f;
        _float3 vOffset = _float3(offsetX, offsetY, 0.f);

        effectDesc.vPos = vEffectPos + vOffset;
        hitDesc.vPos = vEffectPos + vOffset;

        m_pGameInstance->Add_GameObject(
            LEVEL_STATIC,
            TEXT("Prototype_GameObject_Weapon_Effect"),
            LEVEL_STATIC,
            TEXT("Layer_Weapon_Effect"),
            &effectDesc);

        m_pGameInstance->Add_GameObject(
            LEVEL_STATIC,
            TEXT("Prototype_GameObject_Hit_Effect"),
            LEVEL_STATIC,
            TEXT("Layer_Hit_Effect"),
            &hitDesc);
    }
#pragma endregion
}


void CRanged_Weapon::Monster_Hit(CCollider* pCollider, _uint Damage)
{
    if (auto pTarget = dynamic_cast<CCollisionObject*>(pCollider->Get_Owner()))
    {
        pTarget->Set_Hp(pTarget->Get_Hp() - static_cast<_int>(Damage));
        CTransform* pTargetTransform = static_cast<CTransform*>(pCollider->Get_Owner()->Get_Component(L"Com_Transform"));

        _float3 vWallNormal = pTargetTransform->Get_State(CTransform::STATE_LOOK).GetNormalized();
        _float3 vTilePos = pTargetTransform->Get_State(CTransform::STATE_POSITION);
        _float3 vRayOrigin = m_pPickingSys->Get_Ray().vOrigin;
        _float3 vRayDir = m_pPickingSys->Get_Ray().vDir;

        float denom = vWallNormal.Dot(vRayDir);
        if (fabs(denom) < 1e-7f) return;

        float t = vWallNormal.Dot(vTilePos - vRayOrigin) / denom;
        if (t < 0.f) return;

        _float3 vPlaneHit = vRayOrigin + vRayDir * t;

        _float4x4 worldMat = pTargetTransform->Get_WorldMat();
        _float4x4 invWorld;
        D3DXMatrixInverse(&invWorld, nullptr, &worldMat);
        D3DXVECTOR3 vLocalHit;
        D3DXVec3TransformCoord(&vLocalHit, reinterpret_cast<D3DXVECTOR3*>(&vPlaneHit), &invWorld);

        if (vLocalHit.x < -0.55f || vLocalHit.x > 0.55f || vLocalHit.y < -0.55f || vLocalHit.y > 0.55f)
            return;

        _float3 vCamPos = dynamic_cast<CTransform*>(
            m_pGameInstance->Find_Object(LEVEL_STATIC, L"Layer_Camera")->Get_Component(L"Com_Transform"))
            ->Get_State(CTransform::STATE_POSITION);

        _float3 vecTemp = vCamPos - vTilePos;
        _float3 vTileToCam = vecTemp.GetNormalized();
        float fOffset = (vWallNormal.Dot(vTileToCam) > 0.f) ? 0.01f : -0.01f;
        _float3 vEffectPos = vPlaneHit + vWallNormal * fOffset;
        float offsetRangeX = 0.f, offsetRangeY = 0.f;
        if (auto pSphere = dynamic_cast<CCollider_Sphere*>(pCollider))
        {
            // 구 콜라이더: 반지름을 사용 (전체 지름을 기준으로 랜덤하게)
            float radius = pSphere->Get_Radius();
            offsetRangeX = radius;
            offsetRangeY = radius;
        }
        else if (auto pCube = dynamic_cast<CCollider_Cube*>(pCollider))
        {
            // 큐브 콜라이더: 각 축의 길이의 절반을 사용
            offsetRangeX = pCube->Get_Desc().fAxisX.Length() * 0.5f;
            offsetRangeY = pCube->Get_Desc().fAxisY.Length() * 0.5f;
        }
        else
        {
            // 기본값 (보정 필요 시 수정)
            offsetRangeX = 0.55f;
            offsetRangeY = 0.55f;
        }

        // 랜덤 오프셋을 -offsetRange ~ +offsetRange 범위 내에서 결정
        _float offsetX = (((rand() % 100) / 100.f) - 0.5f) * (offsetRangeX * 2.0f);
        _float offsetY = (((rand() % 100) / 100.f) - 0.5f) * (offsetRangeY * 2.0f);
        // -------------------------------------------------------------------

        // 히트 이펙트 위치에 적용
        _float3 vFinalEffectPos = vEffectPos + _float3(offsetX, offsetY, 0.f);

#pragma region Effect 생성
        CEffect_Base::EFFECT_DESC effectDesc;
        effectDesc.vRight = pTargetTransform->Get_State(CTransform::STATE_RIGHT);
        effectDesc.vUp = pTargetTransform->Get_State(CTransform::STATE_UP);
        effectDesc.vLook = pTargetTransform->Get_State(CTransform::STATE_LOOK);
        effectDesc.vScale = { 0.5f, 0.5f, 0.5f };

        CHit_Effect::HIT_DESC hitDesc;
        hitDesc.vRight = pTargetTransform->Get_State(CTransform::STATE_RIGHT);
        hitDesc.vUp = pTargetTransform->Get_State(CTransform::STATE_UP);
        hitDesc.vLook = pTargetTransform->Get_State(CTransform::STATE_LOOK);
        hitDesc.vScale = { 0.5f, 0.5f, 0.5f };
        hitDesc.type = rand() % 2;

        effectDesc.vPos = vFinalEffectPos;
        hitDesc.vPos = vFinalEffectPos;

        m_pGameInstance->Add_GameObject(
            LEVEL_STATIC,
            TEXT("Prototype_GameObject_Blood_Effect"),
            LEVEL_STATIC,
            TEXT("Layer_Blood_Effect"),
            &hitDesc);
#pragma endregion
    }
}
void CRanged_Weapon::CreateHitEffect(CCollider* pClosestCollider, const _float3& vWorldHitPos, _uint Damage)
{

    if (!pClosestCollider) return;

    CGameObject* pOwner = pClosestCollider->Get_Owner();
    if (!pOwner) return;

    if (auto pTarget = dynamic_cast<CCollisionObject*>(pOwner)) 
    {
        pTarget->Set_Hp(pTarget->Get_Hp() - static_cast<_int>(Damage));
    }
    // else if (auto pWall = dynamic_cast<CWall*>(pOwner)) 
    // { 
    // 
    //  벽 관련 처리 
    // 
    // }

    CTransform* pTargetTransform = static_cast<CTransform*>(pOwner->Get_Component(L"Com_Transform"));
    if (!pTargetTransform) return;


    _float3 vSurfaceNormal = pTargetTransform->Get_State(CTransform::STATE_LOOK).GetNormalized();


    CTransform* pCameraTransform = dynamic_cast<CTransform*>(

        m_pGameInstance->Find_Object(LEVEL_STATIC, L"Layer_Camera")->Get_Component(L"Com_Transform")); 

    if (!pCameraTransform) 
        return; // 카메라 없으면 종료 

    _float3 vCamPos = pCameraTransform->Get_State(CTransform::STATE_POSITION);

  
    _float3 vHitToCamDir = (vCamPos - vWorldHitPos); // 방향 벡터
    if (vHitToCamDir.LengthSq() > 1e-6f) 
    {
        vHitToCamDir.Normalize(); 
    }
    else
    {
     
        vHitToCamDir = dynamic_cast<CTransform*>(m_pGameInstance->Find_Object(LEVEL_STATIC, L"Layer_Camera")->Get_Component(L"Com_Transform"))->Get_State(CTransform::STATE_LOOK);
        vHitToCamDir.Normalize();
    }


    float fOffset = (vSurfaceNormal.Dot(vHitToCamDir) > 0.f) ? 0.01f : -0.01f; // Z-fighting 방지 오프셋
    _float3 vBaseEffectPos = vWorldHitPos + vSurfaceNormal * fOffset; // 오프셋 적용된 기본 이펙트 위치

  //auto pTexture =  dynamic_cast<CTexture*>(pOwner->Get_Component(L"Com_Texture")); 
  //if (pTexture)
  //{

  //  D3DXCOLOR color;
  //  pTexture->GetPixelColor(vWorldHitPos, pTargetTransform->Compute_Scaled(), &color); // 텍스처에서 픽셀 색상을 가져오는 가정된 함수

  //  if (color.a <= 0.1f)
  //      return; // 알파 값이 낮으면 이펙트 생성하지 않음
  //}
#pragma region Effect 생성 (최적화 적용)

    // 7. C++ <random> 라이브러리 사용 (rand() 대체)
    //    g_randomEngine은 전역 또는 클래스 멤버 등으로 적절히 초기화되어 있어야 함
    // std::uniform_real_distribution<float> offsetDist(-0.2f, 0.2f); // (-0.5 * 0.4) ~ (0.5 * 0.4)
    // std::uniform_int_distribution<int> typeDist(0, 1); // 이펙트 타입 등
    //
    // _float offsetX = offsetDist(g_randomEngine);
    // _float offsetY = offsetDist(g_randomEngine);
    // int hitType = typeDist(g_randomEngine);

    // 임시로 기존 rand() 사용 (추후 <random>으로 교체 권장)
    _float offsetX_Ratio = ((rand() % 100) / 100.f) - 0.5f; // -0.5 ~ 0.5
    _float offsetY_Ratio = ((rand() % 100) / 100.f) - 0.5f; // -0.5 ~ 0.5
    _float randomSpread = 0.4f; // 무작위 분산 범위
    int hitType = rand() % 2;

    // 타겟의 로컬 축(Right, Up)을 기준으로 오프셋 적용
    _float3 vRight = pTargetTransform->Get_State(CTransform::STATE_RIGHT);
    _float3 vUp = pTargetTransform->Get_State(CTransform::STATE_UP);
    _float3 vFinalEffectPos = vBaseEffectPos + vRight * (offsetX_Ratio * randomSpread) + vUp * (offsetY_Ratio * randomSpread);


    // 8. 이펙트 설명자(Descriptor) 채우기
    //    Get_State 반복 호출 줄이기 위해 위에서 vRight, vUp 등을 미리 받아둠
    CHit_Effect::HIT_DESC hitDesc; // 예시: 피격 이펙트 설명자
    hitDesc.vRight = vRight;
    hitDesc.vUp = vUp;
    hitDesc.vLook = pTargetTransform->Get_State(CTransform::STATE_LOOK); // Look 벡터는 한 번 더 필요
    hitDesc.vScale = { 0.5f, 0.5f, 0.5f };
    hitDesc.vPos = vFinalEffectPos; // 최종 계산된 위치
    hitDesc.type = hitType; // 무작위 타입

    // TODO: 다른 이펙트 타입(예: 벽 파편)이 있다면 해당 DESC도 채우기
    // CEffect_Base::EFFECT_DESC effectDesc; // 만약 필요하다면

    // 9. 이펙트 생성 (오브젝트 풀링 사용 권장)
    // m_pEffectPoolManager->SpawnEffect(TEXT("Prototype_GameObject_Blood_Effect"), ..., &hitDesc); // 풀링 사용 예시

    // 오브젝트 풀링 미사용 시 기존 방식 (문자열 검색 최적화 고려)
    const wchar_t* effectPrototypeName = TEXT("Prototype_GameObject_Blood_Effect"); // 예시: 피 이펙트
    const wchar_t* effectLayerName = TEXT("Layer_Blood_Effect");

    // TODO: 충돌 대상(벽, 몬스터 등)에 따라 다른 프로토타입/레이어 이름 사용
    // if (pTarget && pTarget->IsMonster()) { ... } else if (pWall) { ... }

    m_pGameInstance->Add_GameObject(
        LEVEL_STATIC,         // 이펙트가 속할 레벨 (STATIC 또는 GAMEPLAY 등)
        effectPrototypeName,  // 사용할 이펙트 프로토타입 이름 (ID 기반 검색 권장)
        LEVEL_STATIC,       // 이펙트 오브젝트가 추가될 레벨
        effectLayerName,      // 이펙트 오브젝트가 속할 레이어 이름 (ID 기반 검색 권장)
        &hitDesc);            // 이펙트 초기화 데이터

#pragma endregion
}

void CRanged_Weapon::CreateBossHitEffect(CCollider* pClosestCollider, const _float3& vWorldHitPos, _uint Damage)
{
    if (!pClosestCollider) return;

    CGameObject* pOwner = pClosestCollider->Get_Owner();
    if (!pOwner) return;

    if (auto pTarget = dynamic_cast<CCollisionObject*>(pOwner))
        pTarget->Set_Hp(pTarget->Get_Hp() - static_cast<_int>(Damage));

    CTransform* pTargetTransform = static_cast<CTransform*>(pOwner->Get_Component(L"Com_Transform"));
    if (!pTargetTransform) return;

    _float3 vSurfaceNormal = pTargetTransform->Get_State(CTransform::STATE_LOOK).GetNormalized();

    // 정확히 맞은 위치에 피 이펙트 배치
    _float3 vCamPos = dynamic_cast<CTransform*>(
        m_pGameInstance->Find_Object(LEVEL_STATIC, L"Layer_Camera")->Get_Component(L"Com_Transform"))
        ->Get_State(CTransform::STATE_POSITION);

    _float3 vHitToCamDir = (vCamPos - vWorldHitPos);
    if (vHitToCamDir.LengthSq() > 1e-6f)
        vHitToCamDir.Normalize();
    else
        vHitToCamDir = vSurfaceNormal * -1.f;

    float fOffset = (vSurfaceNormal.Dot(vHitToCamDir) > 0.f) ? 0.01f : -0.01f;
    _float3 vEffectPos = vWorldHitPos + vSurfaceNormal * fOffset;

    CHit_Effect::HIT_DESC hitDesc;
    hitDesc.vRight = pTargetTransform->Get_State(CTransform::STATE_RIGHT);
    hitDesc.vUp = pTargetTransform->Get_State(CTransform::STATE_UP);
    hitDesc.vLook = pTargetTransform->Get_State(CTransform::STATE_LOOK);
    hitDesc.vScale = { 0.5f, 0.5f, 0.5f };
    hitDesc.vPos = vEffectPos;
    hitDesc.type = 0; // 단일 고정 피격 이미지

    m_pGameInstance->Add_GameObject(
        LEVEL_STATIC,
        TEXT("Prototype_GameObject_Blood_Effect"),
        LEVEL_STATIC,
        TEXT("Layer_Blood_Effect"),
        &hitDesc);
}
