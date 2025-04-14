#include "Hub_Portal.h"
#include <Collider_Sphere.h>
#include <GameInstance.h>
#include "Player.h"
#include "CollisionObject.h"
#include "Level_Loading.h"

CHub_Portal::CHub_Portal(LPDIRECT3DDEVICE9 pGraphic_Device)
    :CGameObject{pGraphic_Device}
{
}

CHub_Portal::CHub_Portal(const CHub_Portal& Prototype)
    : CGameObject (Prototype),
    m_pTextureCom{ Prototype.m_pTextureCom },
     m_pTransformCom(Prototype.m_pTransformCom),
    m_pVIBufferCom(Prototype.m_pVIBufferCom)
{
}

HRESULT CHub_Portal::Initialize_Prototype()
{
   
    return S_OK;
}

HRESULT CHub_Portal::Initialize(void* pArg)
{
    if (FAILED(Ready_Components()))
        return E_FAIL;

   
    m_pPlayer = m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Player"));

    m_pPlayer = static_cast<CPlayer*>(m_pPlayer);
    if (m_pPlayer == nullptr)
        return E_FAIL;
    else
        Safe_AddRef(m_pPlayer);


    //m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(-4.3f, 0.5f, 8.2f));

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(12.f, 1.f, 2.5f));

    m_pTransformCom->Set_Scale(1.f, 2.f, 1.f);
    _float3 lightPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    lightPos.y += 0.7f;
    m_pLightCom->Set_Position(lightPos);
    /*lightPos.x += 0.4f;*/
    
    
    m_bPortal_On = false;

    if (pArg)
    {
        m_eLevel = static_cast<LEVEL>(static_cast<PORTAL_DESC*>(pArg)->eLevel);
    }

    return S_OK;
}

void CHub_Portal::Priority_Update(_float fTimeDelta)
{
}

void CHub_Portal::Update(_float fTimeDelta)
{
    LookAtPlayer(fTimeDelta);
    m_pGameInstance->Add_Collider(CG_SHOP, m_pColliderCom);

    m_fFrameTime += fTimeDelta;

    if (m_fFrameTime >= 0.1f) 
    {
        m_fFrameTime = 0.f;
        ++m_iCurrentFrame;
        if (m_iCurrentFrame > 7)
            m_iCurrentFrame = 0;
    }

    
   if (SUCCEEDED(On_Collision()))
    {
       m_bPortal_On = true;
    }
   else
   {
       m_bPortal_On = false;
   }


    if (GetAsyncKeyState('C') & 0x8000)
    {
        if (FAILED(m_pGameInstance->Process_LevelChange(LEVEL_LOADING,
            CLevel_Loading::Create(m_pGraphic_Device, LEVEL_HUB))))
            return;
    }

}


void CHub_Portal::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this)))
        return;
    if (FAILED(m_pGameInstance->Add_Light(m_pLightCom)))
        return;
}


HRESULT CHub_Portal::SetUp_RenderState()
{

    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    //m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    //m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); // 알파 값이 기준보다 크면 픽셀 렌더링
    //m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200); // 기준값 설정 (0~255)
    _float2 ScaleFactor = { 1.0f, 1.0f };
    _float2 Offset = { 0.f, 0.f };
    m_pShaderCom->Set_UVScaleFactor(&ScaleFactor);
    m_pShaderCom->Set_UVOffsetFactor(&Offset);
    return S_OK;
}

HRESULT CHub_Portal::Release_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    return S_OK;
}


HRESULT CHub_Portal::Render()
{
    if (FAILED(m_pTextureCom->Bind_Resource(m_iCurrentFrame)))  
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_Resource()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    SetUp_RenderState();
    if (FAILED(m_pShaderCom->Bind_Texture(m_pTextureCom, m_iCurrentFrame)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Transform()))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Material(m_pMaterialCom)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Lights()))
        return E_FAIL;

    m_pShaderCom->Begin(6);
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;
    m_pShaderCom->End();
    Release_RenderState();

    return S_OK;
}




HRESULT CHub_Portal::On_Collision()
{
    // 플레이어의 위치와 상점의 위치를 가져옴
    CTransform* pPlayerTransform = static_cast<CTransform*>(m_pPlayer->Get_Component(L"Com_Transform"));
    if (nullptr == pPlayerTransform)
        return E_FAIL;

    _float3 vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
    _float3 vShopPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    /*_float3 asd = vPlayerPos - vShopPos;

    float fDistanceSq = asd.LengthSq();*/

    // 플레이어와 상점 사이 거리가 25 이하 (즉, 5 단위 이하)일 때만 충돌로 간주
    const float fThresholdSq = 1.f; // 5^2
    _float tmpDist = _float3::Distance(vPlayerPos, vShopPos);
    if (tmpDist <= fThresholdSq)
    {
        return S_OK;

        /* bPlayer_Check = true;
          m_bIsOpen = true;*/
    }


    //switch (m_pColliderCom->Get_Other_Type())
    //{
    //case CG_PLAYER:
    //    // 플레이어와 충돌한 경우 상호 작용 처리 (예: 상점 열기)
    //    if (!m_bIsOpen)
    //    {
    //        Open_Shop();
    //        // 필요에 따라 m_bIsOpen = true;를 직접 설정할 수 있음.
    //    }
    //    break;
    //default:
    //    break;
    //}
    return E_FAIL;
}

void CHub_Portal::LookAtPlayer(_float fTimeDelta)
{
    if (!m_pPlayer)
        return;

    CTransform* pPlayerTransform = static_cast<CPlayer*>(m_pPlayer)->Get_TransForm();

    // 플레이어의 look 벡터 가져오기 (플레이어가 바라보는 방향)
    _float3 vPlayerLook = pPlayerTransform->Get_State(CTransform::STATE_LOOK);

    vPlayerLook.y = 0.0f;
    vPlayerLook.Normalize();

    _float3 vShopLook = -vPlayerLook;  // 벡터 방향 반전

    // 직교 기저를 계산하여 트랜스폼 설정
    _float3 vUp = _float3(0.0f, 1.0f, 0.0f);  // 월드 업 벡터
    _float3 vRight = vUp.Cross(vShopLook);
    vRight.Normalize();

    // 직교 기저를 보장하기 위해 업 벡터 재계산
    _float3 vNewUp = vShopLook.Cross(vRight);
    vNewUp.Normalize();

    // 상점의 회전 행렬 설정
    m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
    m_pTransformCom->Set_State(CTransform::STATE_UP, vNewUp);
    m_pTransformCom->Set_State(CTransform::STATE_LOOK, vShopLook);

    m_pTransformCom->Set_Scale(1.7f, 2.f, 2.f);
}

HRESULT CHub_Portal::Ready_Components()
{
    /* Transform Component */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
        return E_FAIL;

    /* VIBuffer Component */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
        return E_FAIL;

    ///* Texture Component */
     if (FAILED(__super::Add_Component(LEVEL_HUB,TEXT("Prototype_Component_Texture_Portal"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
         return E_FAIL;

    /* Collider Component */
    CCollider::COL_DESC	ColliderDesc = {};

    //ColliderDesc.pOwner = (this);
    // 이걸로 콜라이더 크기 설정
     ColliderDesc.fScale = { 1.f,1.f,1.f };
    // 오브젝트와 상대적인 거리 설정
    ColliderDesc.fLocalPos = { 0.f, 0.f, 0.0f };

    /* For.Com_Collider_Sphere */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;

    CLight::LIGHT_INIT lDesc = { L"../../Resources/Lights/PortalLight.json" };
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Light_Point"),
        TEXT("Com_Light"), reinterpret_cast<CComponent**>(&m_pLightCom), &lDesc)))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_BaseShader"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    CMaterial::MATERIAL_DESC materialDesc = { L"../../Resources/Materials/PortalMaterial.json" };

    /* For.Com_Material */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Material"),
        TEXT("Com_Material"), reinterpret_cast<CComponent**>(&m_pMaterialCom), &materialDesc)))
        return E_FAIL;

    ///* For.Com_Particle*/
    //if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Particle_Gold"),
    //    TEXT("Com_Particle"), reinterpret_cast<CComponent**>(&m_pParticleCom))))
    //    return E_FAIL;


    return S_OK;
}

CHub_Portal* CHub_Portal::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{

    CHub_Portal* pInstance = new CHub_Portal(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CHub_Portal");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CHub_Portal::Clone(void* pArg)
{
    CHub_Portal* pInstance = new CHub_Portal(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CHub_Portal");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CHub_Portal::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pTransformCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pPlayer);
    Safe_Release(m_pMaterialCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pLightCom);
    Safe_Release(m_pParticleCom);
}
