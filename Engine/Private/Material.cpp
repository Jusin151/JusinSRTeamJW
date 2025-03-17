#include "Material.h"

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
    ifstream f("../../Resources/Materials/TestMaterial.json");
    L"TestMaterial.json";
    json data = json::parse(f);
    m_tMaterial.Diffuse =   { data["Diffuse"]["x"], data["Diffuse"]["z"], data["Diffuse"]["y"], data["Diffuse"]["w"] };
    m_tMaterial.Ambient =   { data["Ambient"]["x"], data["Ambient"]["z"], data["Ambient"]["y"], data["Ambient"]["w"] };
    m_tMaterial.Specular =  { data["Specular"]["x"], data["Specular"]["z"], data["Specular"]["y"], data["Specular"]["w"] };
    m_tMaterial.Power =     { data["Power"] };
    m_tMaterial.Emissive =  { data["Emissive"]["x"], data["Emissive"]["z"], data["Emissive"]["y"], data["Emissive"]["w"] };
    return S_OK;
}

HRESULT CMaterial::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CMaterial::Bind_Resource()
{
    if(FAILED(m_pGraphic_Device->SetMaterial(&m_tMaterial)))
        return E_FAIL;
    return S_OK;
}

CMaterial* CMaterial::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar* pMaterialFilePath)
{
    CMaterial* pInstance = new CMaterial(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype(pMaterialFilePath)))
    {
        MSG_BOX("Failed to Created : CTexture");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CMaterial::Clone(void* pArg)
{
    CMaterial* pInstance = new CMaterial(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CVIBuffer_Terrain");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CMaterial::Free()
{
}
