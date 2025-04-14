#include "Explosion_Effect.h"

CExplosion_Effect::CExplosion_Effect(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CEffect_Base{ pGraphic_Device }
{
}

CExplosion_Effect::CExplosion_Effect(const CExplosion_Effect& Prototype)
    : CEffect_Base{ Prototype }
    , m_pTarget{ Prototype.m_pTarget }
{
}

HRESULT CExplosion_Effect::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CExplosion_Effect::Initialize(void* pArg)
{
    if (pArg != nullptr)
    {      
       EFFECT_DESC desc = *reinterpret_cast<EFFECT_DESC*>(pArg);

    }

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

   

    m_fAnimationSpeed = { 0.07f };
    m_iLastFrame = { m_pTextureCom->Get_NumTextures() };
    m_iCurrentFrame = { 0 };
    return S_OK;
}

HRESULT CExplosion_Effect::Ready_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Explosion_Effect"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &tDesc)))
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

void CExplosion_Effect::Priority_Update(_float fTimeDelta)
{
}

void CExplosion_Effect::Update(_float fTimeDelta)
{
    m_fElapsedTime += fTimeDelta;

    if (m_fElapsedTime >= m_fAnimationSpeed)
    {
        m_fElapsedTime = 0.0f;

        if (m_iCurrentFrame < m_iLastFrame)
        {
            m_iCurrentFrame++;
        }
        else
        {
            //처음 루프이후 다음루프라면 이펙트를 끈다.
            m_bDead = true;
            //m_iCurrentFrame = 0;
        }
    }
}

void CExplosion_Effect::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_BLEND, this)))
        return;
}

HRESULT CExplosion_Effect::Pre_Render()
{
    D3DXVECTOR2 vScaleFactor(1.f, 1.f);
    D3DXVECTOR2 vOffsetFactor(0.0f, 0.0f); // Y축 반전을 위한 오프셋 조정
    m_pShaderCom->Set_UVScaleFactor(&vScaleFactor);
    m_pShaderCom->Set_UVOffsetFactor(&vOffsetFactor);
    // 깊이 버퍼 테스트 비활성화 (항상 그리도록)
    m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);

    // (선택 사항) 깊이 버퍼 쓰기 비활성화
    // 이 객체가 다른 객체의 렌더링에 영향을 주지 않도록 합니다.
    m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); // 알파 값이 기준보다 크면 픽셀 렌더링
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200); // 기준값 설정 (0~255)
    return S_OK;
}

HRESULT CExplosion_Effect::Render()
{
    if (FAILED(m_pTextureCom->Bind_Resource(m_iCurrentFrame)))
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_Resource()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Transform()))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Material(m_pMaterialCom)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Texture(m_pTextureCom, m_iCurrentFrame)))
        return E_FAIL;

    Pre_Render();
    m_pShaderCom->Begin(0);
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;
    m_pShaderCom->End();
    Post_Render();

   //if (FAILED(m_pParticleCom->Render()))
   //   return E_FAIL;

    return S_OK;
}

HRESULT CExplosion_Effect::Post_Render()
{
    m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

    return S_OK;
}

CExplosion_Effect* CExplosion_Effect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CExplosion_Effect* pInstance = new CExplosion_Effect(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {

        MSG_BOX("Create Failed: CExplosion_Effect");

        Safe_Release(pInstance);
    }

    return pInstance;
}

CExplosion_Effect* CExplosion_Effect::Clone(void* pArg)
{
    CExplosion_Effect* pInstance = new CExplosion_Effect(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {

        MSG_BOX("Clone Failed: CExplosion_Effect");

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CExplosion_Effect::Free()
{
    __super::Free();
}
