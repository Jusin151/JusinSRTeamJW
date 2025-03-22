#include "Shop.h"
#include <Collider_Sphere.h>
#include <GameInstance.h>
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

    // 상점 기본 설정
    m_bIsOpen = false;

    return S_OK;
}

void CShop::Priority_Update(_float fTimeDelta)
{
}

void CShop::Update(_float fTimeDelta)
{
}

void CShop::Late_Update(_float fTimeDelta)
{
}

HRESULT CShop::Render()
{
    // 상점이 열려있을 때만 렌더링
    if (m_bIsOpen)
    {
        if (m_pTextureCom && m_pVIBufferCom)
        {
            if (FAILED(m_pTextureCom->Bind_Resource(0)))
                return E_FAIL;

            if (FAILED(SetUp_RenderState()))
                return E_FAIL;
            if (FAILED(m_pVIBufferCom->Render()))
                return E_FAIL;
            if (FAILED(Release_RenderState()))
                return E_FAIL;
        }
    }
    return S_OK;
}

HRESULT CShop::On_Collision()
{
    if (nullptr == m_pColliderCom)
        return E_FAIL;

    // 안바뀌면 충돌 안일어남
    if (m_pColliderCom->Get_Other_Type() == CG_END)
        return S_OK;

    switch (m_pColliderCom->Get_Other_Type())
    {
    case CG_PLAYER:
        // 상호 작용이 눌렸다면
        // m_bIsOpen = true;
        break;
    default:
        break;
    }

    m_pColliderCom->Set_Other_Type(CG_END);

    return S_OK;
}

HRESULT CShop::SetUp_RenderState()
{
    if (nullptr == m_pGraphic_Device)
        return E_FAIL;

    // 알파 블렌딩 활성화
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    // Z 버퍼 설정
    m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

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
    ColliderDesc.pOwner = this;
    // 이걸로 콜라이더 크기 설정
    ColliderDesc.fScale = m_pTransformCom->Compute_Scaled();
    ColliderDesc.eType = CG_SHOP;
    // 오브젝트와 상대적인 거리 설정
    ColliderDesc.fLocalPos = { 0.f, 0.f, 0.0f };


    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
        return E_FAIL;

    return S_OK;
}

void CShop::Free()
{
    __super::Free();

    // 상점 아이템 해제

    // 컴포넌트 해제
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pTransformCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pColliderCom);
}
