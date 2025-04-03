#include "GameObject_Cube.h"
#include "GameInstance.h"

CGameObject_Cube::CGameObject_Cube(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CGameObject{ pGraphic_Device }
{
}

CGameObject_Cube::CGameObject_Cube(const CGameObject& Prototype)
    : CGameObject{ Prototype }
{

}
 
HRESULT CGameObject_Cube::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CGameObject_Cube::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CGameObject_Cube::Priority_Update(_float fTimeDelta)
{
}

void CGameObject_Cube::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CGameObject_Cube::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CGameObject_Cube::Pre_Render()
{
    return E_NOTIMPL;
}

HRESULT CGameObject_Cube::Render()
{
    if (FAILED(m_pTextureCom->Bind_Resource(0)))
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_Resource()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    Pre_Render();

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    Post_Render();

    return S_OK;
}

HRESULT CGameObject_Cube::Post_Render()
{
    return S_OK;
}

HRESULT CGameObject_Cube::Ready_Components()
{
    /* For.Com_Transform */
    CTransform::TRANSFORM_DESC		TransformDesc{ 10.f, D3DXToRadian(90.f) };

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
        return E_FAIL;

    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Cube_Base"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    //m_pGraphic_Device->GetMaterial()
    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Material"),
        TEXT("Com_Material"), reinterpret_cast<CComponent**>(&m_pMaterialCom))))
        return E_FAIL;

    return S_OK;
}


json CGameObject_Cube::Serialize()
{
    json j = __super::Serialize();


    //j["texture_tag"] = WideToUtf8(m_strTextureTag);
    //j["texture_path"] = WideToUtf8(m_tStructure_Desc.stTexturePath); 
    //j["vibuffer"] = WideToUtf8(m_strVIBuffer);
    //j["collider_tag"] = WideToUtf8(m_strCollProtoTag);

    auto pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    auto scale = m_pTransformCom->Compute_Scaled();
    auto angle = m_pTransformCom->Get_EulerAngles();
    j["position"] = { pos.x, pos.y, pos.z };
    j["rotation"] = { angle.x, angle.y, angle.z };
    j["scale"] = { scale.x, scale.y, scale.z };
    return j;
}

void CGameObject_Cube::Deserialize(const json& j)
{
    SET_TRANSFORM(j, m_pTransformCom);
}

CGameObject_Cube* CGameObject_Cube::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CGameObject_Cube* pInstance = new CGameObject_Cube(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CPlayer");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject_Cube* CGameObject_Cube::Clone(void* pArg)
{
    CGameObject_Cube* pInstance = new CGameObject_Cube(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CGameObject_Cube");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CGameObject_Cube::Free()
{
    __super::Free();
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pTransformCom);
    Safe_Release(m_pVIBufferCom);
}