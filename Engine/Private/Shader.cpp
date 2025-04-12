#include "Shader.h"
#include "GameInstance.h"
#include "Texture.h"
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

HRESULT CShader::Bind_Transform()
{
	_float4x4 world, view, proj;
	m_pGraphic_Device->GetTransform(D3DTS_WORLD, &world);
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &view);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &proj);

	_float4x4 viewInverse = {};
	D3DXMatrixInverse(&viewInverse, nullptr, &view);
	_float3 cameraPos = *reinterpret_cast<_float3*>(&viewInverse.m[3][0]);

	Bind_Matrix("g_WorldMatrix",	&world);
	Bind_Matrix("g_ViewMatrix",		&view);
	Bind_Matrix("g_ProjMatrix",		&proj);

	Bind_Vector("g_CameraPosition", &cameraPos);
	return S_OK;
}

HRESULT CShader::Bind_Texture(D3DXHANDLE hParameter, LPDIRECT3DBASETEXTURE9 pTexture)
{
	return m_pEffect->SetTexture(hParameter, pTexture);
}

HRESULT CShader::Bind_Texture(CTexture* pTexture, _uint iIndex)
{
	switch (pTexture->Get_Type())
	{
	case CTexture::TYPE_2D:
		return m_pEffect->SetTexture("g_Texture", pTexture->Get_Texture(iIndex));
	case CTexture::TYPE_CUBE:
		return m_pEffect->SetTexture("g_CubeTexture", pTexture->Get_Texture(iIndex));
	}
	return E_FAIL;
}

HRESULT CShader::Bind_Matrix(D3DXHANDLE hParameter, const _float4x4* pMatrix)
{
	return m_pEffect->SetMatrix(hParameter, pMatrix);
}

HRESULT CShader::Bind_Vector(D3DXHANDLE hParameter, const _float2* pVector)
{
	D3DXVECTOR4 vec = { pVector->x, pVector->y, 1.0f, 1.0f };
	return m_pEffect->SetVector(hParameter, &vec);
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

HRESULT CShader::Bind_Value(D3DXHANDLE hParameter, void* pArg, _uint bytes)
{
	return m_pEffect->SetValue(hParameter, pArg, bytes);
}

HRESULT CShader::Bind_Lights()
{
	m_pGameInstance->Set_Lights(this);
	return S_OK;
}

HRESULT CShader::Bind_Handle(void* pArg, _uint bytes)
{
	
	D3DXHANDLE temp = m_pEffect->GetParameterByName(NULL, "g_Lights");

	return m_pEffect->SetRawValue(temp, pArg, 0, bytes);
	//m_pEffect->SetValue(temp, pArg, bytes);
	return S_OK;
}

HRESULT CShader::Set_Fog(_float3 rgb, _float start, _float end)
{
	HRESULT hr = Bind_Vector("g_FogColor", &rgb);
	hr = Bind_Float("g_FogStart", start);
	hr = Bind_Float("g_FogEnd", end);
	return hr;
}

HRESULT CShader::Set_UVScaleFactor(const _float2* pScale)
{
	return Bind_Vector("g_ScaleFactor", pScale);
}

HRESULT CShader::Set_UVOffsetFactor(const _float2* pOffset)
{
	return Bind_Vector("g_OffsetFactor", pOffset);
}

HRESULT CShader::Set_Bool(const char* pParameterName, _bool bValue)
{
	return m_pEffect->SetBool(pParameterName, bValue);
}


void CShader::Begin(_uint iPassIndex)
{
	m_pEffect->CommitChanges();
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
