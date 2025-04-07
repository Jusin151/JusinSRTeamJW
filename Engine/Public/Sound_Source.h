#pragma once
#include "Component.h"
BEGIN(Engine)
class CSound_Manager;
class CSound_Event;
class ENGINE_DLL CSound_Source final : public CComponent
{
private:
	CSound_Source(LPDIRECT3DDEVICE9 pGraphic_Device, CSound_Manager* pSound_Manager);
	CSound_Source(const CSound_Source& Prototype);
	virtual ~CSound_Source() = default;
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);

public:
	CSound_Event*	Play_Event(_wstring strEvent);
	void			Stop_All_Event();
	
private:
	CSound_Manager*			m_pSound_Manager;
	vector<CSound_Event*>	m_Events2D;
	vector<CSound_Event*>	m_Events3D;
public:
	static	CSound_Source* Create(LPDIRECT3DDEVICE9 pGraphic_Device, CSound_Manager* pSound_Manager);
	CSound_Source*	Clone(void* pArg);

public:
	virtual void	Free() override;
};

END;