#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CMaterial final : public CComponent
{
private:
	CMaterial(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMaterial(const CMaterial& Prototype);
	virtual ~CMaterial() = default;
public:
	virtual HRESULT Initialize_Prototype(const _tchar * pTextureFilePath, _uint iNumTextures);
	virtual HRESULT Initialize(void* pArg) override;
public:
	HRESULT Bind_Resource(_uint iIndex);
private:
	_float4							m_fDiffuse;
	_float4							m_fAmbient;
	_float4							m_fSpecular;
	_float4							m_fEmissive;
	_float							m_fPower;
	_uint							m_iNumTextures = { };
	vector<LPDIRECT3DTEXTURE9>		m_Textures;

public:
	static CMaterial* Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar * pTextureFilePath, _uint iNumTextures);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END