#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CShader final : public CComponent
{
public:
	typedef struct tagShaderDesc {
		_wstring filePath;
	}SHADER_DESC;
private:
	CShader(LPDIRECT3DDEVICE9 pGraphic_Device);
	CShader(const CShader& Prototype);
	virtual ~CShader() = default;
public:
	virtual HRESULT Initialize_Prototype(const _tchar* pShaderFilePath);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_Transform();

public:
	HRESULT Bind_Texture(D3DXHANDLE hParameter, LPDIRECT3DBASETEXTURE9 pTexture);
	HRESULT Bind_Texture(class CTexture*, _uint iIndex);
	HRESULT Bind_Matrix(D3DXHANDLE hParameter, const _float4x4* pMatrix);
	HRESULT Bind_Vector(D3DXHANDLE hParameter, const _float3* pVector);
	HRESULT Bind_Vector(D3DXHANDLE hParameter, const _float4* pVector);
	HRESULT Bind_Int(D3DXHANDLE hParameter, const _int pInt);
	HRESULT Bind_Float(D3DXHANDLE hParameter, const _float pFloat);
	HRESULT Bind_Light(class CLight* pLight);
	HRESULT Bind_Material(class CMaterial* pMaterial);
	HRESULT Bind_Value(D3DXHANDLE hParameter, void* pArg, _uint bytes);
	HRESULT Bind_Lights();
	HRESULT Bind_Handle(void* pArg, _uint bytes);

public:
	HRESULT Set_Fog(_float3 rgb, _float start, _float end);

public:
	void Begin(_uint iPassIndex);
	void End();

private:
	LPD3DXEFFECT				m_pEffect = { nullptr };

public:
	static CShader* Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar* pShaderFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};


END