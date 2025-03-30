#include "Ranged_Weapon.h"
#include "GameInstance.h"
#include "PickingSys.h"
#include "Effect_Base.h"
#include "UI_Manager.h"

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
    if (GetAsyncKeyState('W') & 0x8000)
    {
        t += speed;
    }
    else if (GetAsyncKeyState('A') & 0x8000)
    {
        t += speed;
    }
    else if (GetAsyncKeyState('D') & 0x8000)
    {
        t += speed;
    }
    else if (GetAsyncKeyState('S') & 0x8000)
    {
        t += speed;
    }

    float v = 20.0f;  // 폭을 설정 하는변수
    _float3 vNewPos;
    vNewPos.x = m_vInitialPos.x + (1 + v * cosf(t / 2)) * cosf(t);
    vNewPos.y = m_vInitialPos.y + (1 + v * cosf(t / 2)) * sinf(t);

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vNewPos);

	//Attack(fTimeDelta);

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
    //  매 프레임마다 마우스/레이 갱신
    if(m_pPickingSys)
    m_pPickingSys->Update();

    if (!(GetKeyState(VK_LBUTTON) & 0x8000))
        return S_OK;

    vector<list<CCollider*>> colliderGroups = m_pGameInstance->Get_Colliders();

    for (auto& group : colliderGroups)
    {
        for (auto* collider : group)
        {
            if (!collider || !collider->Get_Owner())
                continue;

            _float3 vHitPos{};
            if (!m_pPickingSys->Ray_Intersection(collider, &vHitPos))
                continue;

            const wstring& tag = collider->Get_Owner()->Get_Tag();
            if (tag == L"Layer_Player" || tag.find(L"Floor") != wstring::npos)
                continue;

            if (tag.find(L"Wall") != wstring::npos)
            {
                Wall_Picking(collider, EffectNum);
                m_bWall = true;
            }
            else if (tag == L"Layer_Harpoonguy")
            {
                Monster_Hit(collider, Damage);
                m_bMonster = true;
            }

            break; // 하나만 처리
        }
    }

    return S_OK;
}
void CRanged_Weapon::Wall_Picking(CCollider* pCollider, _uint EffectNum)
{
    CTransform* pWallTransform = static_cast<CTransform*>(pCollider->Get_Owner()->Get_Component(L"Com_Transform"));

    _float3 vWallNormal = pWallTransform->Get_State(CTransform::STATE_LOOK).GetNormalized();
    _float3 vTilePos = pWallTransform->Get_State(CTransform::STATE_POSITION);
    _float3 vRayOrigin = m_pPickingSys->Get_Ray().vOrigin;
    _float3 vRayDir = m_pPickingSys->Get_Ray().vDir;


    float denom = vWallNormal.Dot(vRayDir);
    if (fabs(denom) < 1e-7f) return;

    float t = vWallNormal.Dot(vTilePos - vRayOrigin) / denom;
    if (t < 0.f) return;

    _float3 vPlaneHit = vRayOrigin + vRayDir * t;

    _float4x4 worldMat = pWallTransform->Get_WorldMat();
    _float4x4 invWorld;
    D3DXMatrixInverse(&invWorld, nullptr, &worldMat);
    D3DXVECTOR3 vLocalHit;
    D3DXVec3TransformCoord(&vLocalHit, reinterpret_cast<D3DXVECTOR3*>(&vPlaneHit), &invWorld);

    if (vLocalHit.x < -0.55f || vLocalHit.x > 0.55f || vLocalHit.y < -0.55f || vLocalHit.y > 0.55f)
        return;

    _float3 vCamPos = dynamic_cast<CTransform*>(
        m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, L"Layer_Camera")->Get_Component(L"Com_Transform"))
        ->Get_State(CTransform::STATE_POSITION);

    _float3 vecTemp = vCamPos - vTilePos;
    _float3 vTileToCam = vecTemp.GetNormalized();
    float fOffset = (vWallNormal.Dot(vTileToCam) > 0.f) ? 0.01f : -0.01f;
    _float3 vEffectPos = vPlaneHit + vWallNormal * fOffset;

    CEffect_Base::EFFECT_DESC effectDesc;
    effectDesc.vRight = pWallTransform->Get_State(CTransform::STATE_RIGHT);
    effectDesc.vUp = pWallTransform->Get_State(CTransform::STATE_UP);
    effectDesc.vLook = pWallTransform->Get_State(CTransform::STATE_LOOK);
    effectDesc.vScale = { 0.5f, 0.5f, 0.5f };

    for (_uint i = 0; i < EffectNum; ++i)
    {
        _float offsetX = (((rand() % 100) / 100.f) - 0.5f) * 0.4f;
        _float offsetY = (((rand() % 100) / 100.f) - 0.5f) * 0.4f;
        effectDesc.vPos = vEffectPos + _float3(offsetX, offsetY, 0.f);

        m_pGameInstance->Add_GameObject(
            LEVEL_GAMEPLAY,
            TEXT("Prototype_GameObject_Weapon_Effect"),
            LEVEL_GAMEPLAY,
            TEXT("Layer_Weapon_Effect"),
            &effectDesc);
    }
}
void CRanged_Weapon::Monster_Hit(CCollider* pCollider, _uint Damage)
{
    if (auto pTarget = dynamic_cast<CCollisionObject*>(pCollider->Get_Owner()))
    {
        pTarget->Set_Hp(pTarget->Get_Hp() - static_cast<_int>(Damage));
    }
}
