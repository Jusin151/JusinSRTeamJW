#include "Shop.h"
#include <Collider_Sphere.h>
#include <GameInstance.h>
#include "Player.h"
#include "CollisionObject.h"

CShop::CShop(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CGameObject(pGraphic_Device)
{
}

CShop::CShop(const CShop& Prototype)
    : CGameObject(Prototype)
{
}

HRESULT CShop::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CShop::Initialize(void* pArg) // 자식에서 무조건 __Super:: 로 부르기
{
    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pPlayer = m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Player"));
 
    m_pPlayer = static_cast<CPlayer*>(m_pPlayer);
    if (m_pPlayer == nullptr)
        return E_FAIL;
    else
        Safe_AddRef(m_pPlayer);

    if (!(m_pFirstPersonCamera = dynamic_cast<CCamera_FirstPerson*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Camera")))))
        return E_FAIL;
  

 
    // 상점 기본 설정
    m_bIsOpen = false;

    return S_OK;
}

void CShop::Priority_Update(_float fTimeDelta)
{
}

void CShop::Update(_float fTimeDelta)
{
    LookAtPlayer(fTimeDelta);

    m_pColliderCom->Update_Collider(TEXT("Com_Transform"), m_pColliderCom->Get_Scale());

    m_pGameInstance->Add_Collider(CG_SHOP, m_pColliderCom);

    if (SUCCEEDED(On_Collision())) 
    {
        if (GetAsyncKeyState(VK_SPACE) & 0x8000)
        {
            if (!m_bSpacePressed)  
            {
                m_bIsOpen = !m_bIsOpen;
               // m_pGameInstance->Open_UI(LEVEL_HUB, m_bIsOpen);
                m_bSpacePressed = true; 
                static_cast<CPlayer*>(m_pPlayer)->StopAction();
                m_pFirstPersonCamera->Camera_Lock(m_bIsOpen);
            }
        }
        else
        {
            m_bSpacePressed = false;
        }
    }

}


void CShop::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this)))
        return;

}

HRESULT CShop::Render()
{
    // 상점이 열려있을 때만 렌더링
    if (m_pTextureCom && m_pVIBufferCom)
    {
        if (FAILED(m_pTransformCom->Bind_Resource()))
            return E_FAIL;
        if (FAILED(m_pTextureCom->Bind_Resource(0)))
            return E_FAIL;
        if (FAILED(m_pMaterialCom->Bind_Resource()))
            return E_FAIL;
        if (FAILED(m_pVIBufferCom->Bind_Buffers()))
            return E_FAIL;

        if (FAILED(SetUp_RenderState()))
            return E_FAIL;
        if (FAILED(m_pShaderCom->Bind_Texture(m_pTextureCom, 0)))
            return E_FAIL;
        if (FAILED(m_pShaderCom->Bind_Transform()))
            return E_FAIL;
        if (FAILED(m_pShaderCom->Bind_Material(m_pMaterialCom)))
            return E_FAIL;
        if (FAILED(m_pShaderCom->Bind_Lights()))
            return E_FAIL;

        m_pShaderCom->Begin(1);

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;
        m_pShaderCom->End();
        if (FAILED(Release_RenderState()))
            return E_FAIL;
    }
    return S_OK;
}

void CShop::LookAtPlayer(_float fTimeDelta)
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
}



HRESULT CShop::On_Collision()
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
    const float fThresholdSq = 3.f; // 5^2
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



HRESULT CShop::SetUp_RenderState()
{
    if (nullptr == m_pGraphic_Device)
        return E_FAIL;

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    // Z 버퍼 설정
    m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    _float2 ScaleFactor = { 1.0f, 1.0f };
    _float2 Offset = { 0.f, 0.f };
    m_pShaderCom->Set_UVScaleFactor(&ScaleFactor);
    m_pShaderCom->Set_UVOffsetFactor(&Offset);
    return S_OK;
}

HRESULT CShop::Release_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

    return S_OK;
}

HRESULT CShop::Ready_Components()
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
    //if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Shop"),
    //    TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
    //    return E_FAIL;


    /* Collider Component */
    CCollider::COL_DESC	ColliderDesc = {};
  
    //ColliderDesc.pOwner = (this);
    // 이걸로 콜라이더 크기 설정
    ColliderDesc.fScale = { 100.f,100.f,100.f };
    // 오브젝트와 상대적인 거리 설정
    ColliderDesc.fLocalPos = { 10.f, 10.f, 10.5f };

    /* For.Com_Collider_Sphere */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;

    CLight::LIGHT_INIT lDesc = { L"../../Resources/Lights/GunLight.json" };
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Light_Point"),
        TEXT("Com_Light"), reinterpret_cast<CComponent**>(&m_pLightCom), &lDesc)))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_BaseShader"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Material */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Material"),
        TEXT("Com_Material"), reinterpret_cast<CComponent**>(&m_pMaterialCom))))
        return E_FAIL;
    return S_OK;
}

void CShop::Free()
{
    __super::Free();

    // 상점 아이템 해제

    // 컴포넌트 해제
    Safe_Release(m_pLightCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pMaterialCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pTransformCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pPlayer);
}
