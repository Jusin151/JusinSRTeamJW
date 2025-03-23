#include "Ranged_Weapon.h"
#include "GameInstance.h"
#include "Collider.h"
#include "PickingSys.h"
#include "Effect_Base.h"
#include "CUI_Manager.h"

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
	
}

void CRanged_Weapon::Update(_float fTimeDelta)
{
	Attack(fTimeDelta);

}

void CRanged_Weapon::Late_Update(_float fTimeDelta)
{
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

HRESULT CRanged_Weapon::Picking_Object() // 요거 주석 지우지마셈.. 공부점..
{
    //  매 프레임마다 마우스/레이 갱신
    m_pPickingSys->Update();

    // 마우스 왼쪽 버튼 누른 상태가 아니라면 그냥 리턴
    if (!(GetKeyState(VK_LBUTTON) & 0x8000))
        return S_OK;

    // 현재 등록된 모든 콜라이더 확인
    vector<list<CCollider*>> colliderGroups = m_pGameInstance->Get_Colliders();
    for (auto& group : colliderGroups)
    {
        for (auto* collider : group)
        {
            if (nullptr == collider || nullptr == collider->Get_Owner())
                continue;

            // 2) 먼저 OBB 교차로 “맞았는지” 판정
            _float3 vHitPos = {};
            if (!m_pPickingSys->Ray_Intersection(collider, &vHitPos))
                continue; // 충돌X

            // 플레이어 콜라이더라면 스킵
            if (collider->Get_Owner()->Get_Tag() == L"Layer_Player")
                continue;

            // 3) 벽 태그인 경우
            if (collider->Get_Owner()->Get_Tag() == L"Layer_Wall")
            {
                // ---- [A] 벽 Transform에서 평면 정의 ----
                CTransform* pWallTransform = static_cast<CTransform*>(
                    collider->Get_Owner()->Get_Component(L"Com_Transform"));

                // 타일(벽) 중심 위치
                _float3 vTilePos = pWallTransform->Get_State(CTransform::STATE_POSITION);
                // 타일 노멀(LOOK) - “정면 방향”이라고 가정
                _float3 vWallNormal = pWallTransform->Get_State(CTransform::STATE_LOOK).GetNormalized();

                // ---- [B] Ray-Plane 교차 ----
                // (PickingSys::m_Ray에 vOrigin, vDir이 있다고 가정)
                _float3 vRayOrigin = m_pPickingSys->Get_Ray().vOrigin;
                _float3 vRayDir = m_pPickingSys->Get_Ray().vDir;

                float denom = vWallNormal.Dot(vRayDir);
                if (fabs(denom) < 1e-6f)
                    continue; // 레이가 평면과 거의 평행 => 교차 안 함

                float t = vWallNormal.Dot(vTilePos - vRayOrigin) / denom;
                if (t < 0.f)
                    continue; // 레이 뒤쪽 => 카메라 뒤라서 보이지 않음

                // 진짜진짜진짜 “타일 평면”과의 교차점
                _float3 vPlaneHit = vRayOrigin + (vRayDir * t);

                // ---- [C] (선택) 타일 사각형 범위 체크 ----
                // 만약 VIBuffer_Rect가 1x1 크기라면, 로컬 좌표에서 -0.5~+0.5 범위
                {
                    _float4x4 worldMat = pWallTransform->Get_WorldMat();
                    _float4x4 invWorld;
                    D3DXMatrixInverse(&invWorld, nullptr, &worldMat);

                    D3DXVECTOR3 vLocalHit;
                    D3DXVec3TransformCoord(
                        &vLocalHit,
                        reinterpret_cast<const D3DXVECTOR3*>(&vPlaneHit),
                        &invWorld
                    );

                    // 범위 밖이면 스킵
                    if (vLocalHit.x < -0.5f || vLocalHit.x > 0.5f ||
                        vLocalHit.y < -0.5f || vLocalHit.y > 0.5f)
                    {
                        continue; // 타일 표면 밖 => 탄흔 안 찍음
                    }
                }

                // 타일 중심 위치
                _float3 vTilePos_1 = pWallTransform->Get_State(CTransform::STATE_POSITION);
                // 타일 노멀(벽 정면)
                _float3 vWallNormal_1 = pWallTransform->Get_State(CTransform::STATE_LOOK).GetNormalized();


                dynamic_cast<CTransform*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, L"Layer_Camera")->Get_Component(L"Com_Transform"))->Get_State(CTransform::STATE_POSITION);


                // 카메라 위치 (예: CPickingSys의 카메라 Transform 사용)
                _float3 vCamPos = dynamic_cast<CTransform*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, L"Layer_Camera")->Get_Component(L"Com_Transform"))->Get_State(CTransform::STATE_POSITION);


                _float3 vCamDir = vCamPos - vTilePos_1;

                // 타일에서 카메라 방향 벡터 계산 및 정규화
                _float3 vTileToCam = vCamDir.GetNormalized();

                // 내적을 통해 타일 노멀과 카메라 방향의 관계 판단
                float dot = vWallNormal_1.Dot(vTileToCam);

                // 카메라를 향하면 양수, 그렇지 않으면 음수
                float fOffset = (dot > 0.f) ? 0.01f : -0.01f;

                // 교차점에서  적용
                _float3 vEffectPos = vPlaneHit + (vWallNormal_1 * fOffset);

                // 타일의 Right, Up, Look 가져오기
                _float3 vWallRight = pWallTransform->Get_State(CTransform::STATE_RIGHT);
                _float3 vWallUp = pWallTransform->Get_State(CTransform::STATE_UP);
                _float3 vWallLook = pWallTransform->Get_State(CTransform::STATE_LOOK);

                CEffect_Base::EFFECT_DESC Weapon_Effect{};
                Weapon_Effect.vPos = vEffectPos;     // 타일꺼 
                Weapon_Effect.vRight = vWallRight;  
                Weapon_Effect.vUp = vWallUp;     
                Weapon_Effect.vLook = vWallLook;  
                Weapon_Effect.vScale = { 0.5f, 0.5f, 0.5f };

                if (FAILED(m_pGameInstance->Add_GameObject(
                    LEVEL_GAMEPLAY,
                    TEXT("Prototype_GameObject_Weapon_Effect"),
                    LEVEL_GAMEPLAY,
                    TEXT("Layer_Weapon_Effect"),
                    &Weapon_Effect)))
                {
                    return E_FAIL;
                }         
                m_bWall = true;
            }
            else
            {
                // 다른 태그(예: 몬스터)라면
                if (collider->Get_Owner()->Get_Tag() == L"Layer_Crocman")
                    m_bMonster = true;
                else
                    m_bMonster = false;
            }

            // 4) 1개만 처리하고 break 할지, 계속 처리할지 결정
            break;
        }
    }
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


void CRanged_Weapon::Attack(_float fTimeDelta)
{
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		m_bIsAnimating = true;
		m_iCurrentFrame = 0;
		m_fElapsedTime = 0.0f;
	}
	if (m_bIsAnimating)
	{
		m_fElapsedTime += fTimeDelta;
		if (m_fElapsedTime >= 0.02f)
		{
			m_fElapsedTime = 0.0f;
			if (m_iCurrentFrame < 13)
			{
				m_iCurrentFrame++;
			}
			else
			{
				m_bIsAnimating = false;
			}
		}
	}
    
}



