#pragma once
#include "Component.h"
class CSound_Manager;
class CSound_Event;

BEGIN(Engine)
class ENGINE_DLL CSound_Source final : CComponent
{
private:
	CSound_Source(LPDIRECT3DDEVICE9 pGraphic_Device);
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
	CSound_Event	Play_Event();
	void			Stop_All_Event();
	
private:
	CSound_Manager* m_pSound_Manager;
	vector<CSound_Event> m_Events2D;
	vector<CSound_Event> m_Events3D;
public:
	static	CSound_Source* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	CSound_Source*	Clone(void* pArg);

public:
	virtual void	Free() override;
};

END;