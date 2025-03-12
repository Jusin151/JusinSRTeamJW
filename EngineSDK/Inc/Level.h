#pragma once

#include "Base.h"

/* 클라 개발자가 만들 레벨클래스들의 부모가 되는 클래스다. */

BEGIN(Engine)

class ENGINE_DLL CLevel abstract : public CBase
{
protected:
	CLevel(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CLevel() = default;

public:
	virtual HRESULT Initialize();	
	virtual void Update(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	LPDIRECT3DDEVICE9		m_pGraphic_Device = { nullptr };
	class CGameInstance*	m_pGameInstance = { nullptr };

public:	
	virtual void Free() override;

};

END