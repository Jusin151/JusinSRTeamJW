#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CSound final : public CComponent
{
private:
	CSound(LPDIRECT3DDEVICE9 pGraphic_Device);
	CSound(const CSound& Prototype);
	virtual ~CSound() = default;
public:
	virtual HRESULT Initialize_Prototype(const _tchar* pSoundFilePath, _uint iNumSounds);
	virtual HRESULT Initialize(void* pArg) override;
public:
	HRESULT Bind_Resource(_uint iIndex);
private:
	_uint							m_iNumSounds= { };
	vector<FMOD::Sound>		m_Sounds;

public:
	static CSound* Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar* pSoundFilePath, _uint iNumSounds);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END