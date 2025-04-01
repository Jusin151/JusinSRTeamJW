#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
public:
	enum TYPE { TYPE_CUBE, TYPE_2D, TYPE_END };
private:
	CTexture(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTexture(const CTexture& Prototype);
	virtual ~CTexture() = default;
public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const _tchar* pTextureFilePath, _uint iNumTextures);
	virtual HRESULT Initialize(void* pArg) override;
public:
	HRESULT Bind_Resource(_uint iIndex);
	HRESULT Bind_Resource(class CShader* pShader, D3DXHANDLE hParameters, _uint iIndex);
public:
	_uint Get_NumTextures() { return m_iNumTextures; }
private:
	_uint								m_iNumTextures = { };
	vector<IDirect3DBaseTexture9*>		m_Textures;
	

public:
	static CTexture* Create(LPDIRECT3DDEVICE9 pGraphic_Device, TYPE eType, const _tchar* pTextureFilePath, _uint iNumTextures);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END