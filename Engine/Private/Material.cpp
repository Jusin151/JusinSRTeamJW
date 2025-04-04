#include "Material.h"
#include "Shader.h"

CMaterial::CMaterial(LPDIRECT3DDEVICE9 pGraphic_Device)
    :   CComponent { pGraphic_Device }
{
}

CMaterial::CMaterial(const CMaterial& Prototype)
    : CComponent    { Prototype }
    , m_tMaterial   { Prototype.m_tMaterial }
{
}

HRESULT CMaterial::Initialize_Prototype(const _tchar* pMaterialFilePath)
{
    ifstream f(pMaterialFilePath);
    json data = json::parse(f);
    m_tMaterial.Diffuse =   { data["Diffuse"]["x"], data["Diffuse"]["y"], data["Diffuse"]["z"], data["Diffuse"]["w"] };
    m_tMaterial.Ambient =   { data["Ambient"]["x"], data["Ambient"]["y"], data["Ambient"]["z"], data["Ambient"]["w"] };
    m_tMaterial.Specular =  { data["Specular"]["x"], data["Specular"]["y"], data["Specular"]["z"], data["Specular"]["w"] };
    m_tMaterial.Power =     { data["Power"] };
    m_tMaterial.Emissive =  { data["Emissive"]["x"], data["Emissive"]["y"], data["Emissive"]["z"], data["Emissive"]["w"] };
    return S_OK;
}

HRESULT CMaterial::Initialize(void* pArg)
{
    if (pArg)
    {
        MATERIAL_DESC desc = *reinterpret_cast<MATERIAL_DESC*>(pArg);
        ifstream f(desc.MaterialFilePath);
        json data = json::parse(f);
        m_tMaterial.Diffuse = { data["Diffuse"]["x"], data["Diffuse"]["y"], data["Diffuse"]["z"], data["Diffuse"]["w"] };
        m_tMaterial.Ambient = { data["Ambient"]["x"], data["Ambient"]["y"], data["Ambient"]["z"], data["Ambient"]["w"] };
        m_tMaterial.Specular = { data["Specular"]["x"], data["Specular"]["y"], data["Specular"]["z"], data["Specular"]["w"] };
        m_tMaterial.Power = { data["Power"] };
        m_tMaterial.Emissive = { data["Emissive"]["x"], data["Emissive"]["y"], data["Emissive"]["z"], data["Emissive"]["w"] };
    }
    return S_OK;
}

HRESULT CMaterial::Bind_Resource()
{
    D3DMATERIAL9 material = {};
    memcpy(&material.Diffuse,   &m_tMaterial.Diffuse,   sizeof(_float4));
    memcpy(&material.Ambient,   &m_tMaterial.Ambient,   sizeof(_float4));
    memcpy(&material.Specular,  &m_tMaterial.Specular,  sizeof(_float4));
    memcpy(&material.Emissive,  &m_tMaterial.Emissive,  sizeof(_float4));
    memcpy(&material.Power,     &m_tMaterial.Power,     sizeof(_float));
    if (FAILED(m_pGraphic_Device->SetMaterial(&material)))
        return E_FAIL;
    return S_OK;
}

HRESULT CMaterial::Bind_Material(CShader* pShader)
{
    pShader->Bind_Vector("g_MaterialDiffuse",       &m_tMaterial.Diffuse);
    pShader->Bind_Vector("g_MaterialAmbient",       &m_tMaterial.Ambient);
    pShader->Bind_Vector("g_MaterialSpecular",      &m_tMaterial.Specular);
    pShader->Bind_Float("g_MaterialSpecularPower",  m_tMaterial.Power);
    pShader->Bind_Vector("g_MaterialEmissive",      &m_tMaterial.Emissive);
    return S_OK;
}

CMaterial* CMaterial::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar* pMaterialFilePath)
{
    CMaterial* pInstance = new CMaterial(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype(pMaterialFilePath)))
    {
        MSG_BOX("Failed to Created : CMaterial");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CMaterial::Clone(void* pArg)
{
    CMaterial* pInstance = new CMaterial(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CMaterial");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CMaterial::Free()
{
    __super::Free();
}
