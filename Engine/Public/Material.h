#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CMaterial final : public CComponent
{
public:
	typedef struct tagMaterialInit
	{
		const _wstring MaterialFilePath;
	} MATERIAL_DESC;
	typedef struct tagMaterialInfo
	{
		_float4 Diffuse;
		_float4 Ambient;
		_float4 Specular;
		_float4 Emissive;
		_float	Power;
	} MATERIAL;
private:
	CMaterial(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMaterial(const CMaterial& Prototype);
	virtual ~CMaterial() = default;
public:
	virtual HRESULT Initialize_Prototype(const _tchar * pMaterialFilePath);
	virtual HRESULT Initialize(void* pArg) override;
public:
	HRESULT Bind_Resource();
	HRESULT Bind_Material(class CShader* pShader);
private:
	//D3DMATERIAL9					m_tMaterial;
	MATERIAL						m_tMaterial;
public:
	static	CMaterial* Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar * pMaterialFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END