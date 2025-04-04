#include "Shader.h"
#include "Material.h"
#include "Light.h"

CShader::CShader(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CComponent { pGraphic_Device }
{
}

CShader::CShader(const CShader& Prototype)
    : CComponent{ Prototype }
    , m_pEffect { Prototype.m_pEffect }
{
    Safe_AddRef(m_pEffect);
}

HRESULT CShader::Initialize_Prototype(const _tchar* pShaderFilePath)
{
	if (FAILED(D3DXCreateEffectFromFile(m_pGraphic_Device, pShaderFilePath, nullptr, nullptr, 0, nullptr, &m_pEffect, nullptr)))
		return E_FAIL;

    return S_OK;
}

HRESULT CShader::Initialize(void* pArg)
{
	if (pArg)
	{
		SHADER_DESC desc = *reinterpret_cast<SHADER_DESC*>(pArg);
		Safe_Release(m_pEffect);
		if (FAILED(D3DXCreateEffectFromFile(m_pGraphic_Device, desc.filePath.c_str(), nullptr, nullptr, 0, nullptr, &m_pEffect, nullptr)))
			return E_FAIL;
	}
    return S_OK;
}

HRESULT CShader::Bind_Texture(D3DXHANDLE hParameter, LPDIRECT3DBASETEXTURE9 pTexture)
{
	return m_pEffect->SetTexture(hParameter, pTexture);
}

HRESULT CShader::Bind_Matrix(D3DXHANDLE hParameter, const _float4x4* pMatrix)
{
	return m_pEffect->SetMatrix(hParameter, pMatrix);
}

HRESULT CShader::Bind_Vector(D3DXHANDLE hParameter, const _float3* pVector)
{
	D3DXVECTOR4 vec = { *pVector, 1.0f };
	//m_pEffect.set
	return m_pEffect->SetVector(hParameter, &vec);
}

HRESULT CShader::Bind_Vector(D3DXHANDLE hParameter, const _float4* pVector)
{
	return m_pEffect->SetVector(hParameter, pVector);
}

HRESULT CShader::Bind_Int(D3DXHANDLE hParameter, const _int pInt)
{
	return m_pEffect->SetInt(hParameter, pInt);
}

HRESULT CShader::Bind_Float(D3DXHANDLE hParameter, const _float pFloat)
{
	return m_pEffect->SetFloat(hParameter, pFloat);;
}

HRESULT CShader::Bind_Light(CLight* pLight)
{
	pLight->Bind_Light(this);
	return S_OK;
}

HRESULT CShader::Bind_Material(CMaterial* pMaterial)
{
	pMaterial->Bind_Material(this);
	return S_OK;
}

void CShader::Begin(_uint iPassIndex)
{
	m_pEffect->Begin(nullptr, 0);
	m_pEffect->BeginPass(iPassIndex);
}

void CShader::End()
{
	m_pEffect->EndPass();
	m_pEffect->End();
}

CShader* CShader::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar* pShaderFilePath)
{
	CShader* pInstance = new CShader(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype(pShaderFilePath)))
	{
		MSG_BOX("Failed to Created : CShader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CShader::Clone(void* pArg)
{
	CShader* pInstance = new CShader(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CShader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShader::Free()
{
    __super::Free();

    Safe_Release(m_pEffect);
}
