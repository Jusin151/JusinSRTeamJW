#pragma once
#include "Component.h"

class CSound_Manager;

BEGIN(Engine)
class ENGINE_DLL CSound_Listener final : CComponent
{
private:
	CSound_Listener();
	CSound_Listener(const CSound_Listener& Prototype);
	virtual ~CSound_Listener() = default;
private:
	CSound_Manager*			m_pSound_Manager;
	_float3					m_vPos;
public:
	static	CSound_Listener* Create();
	CSound_Listener* Clone(void *pArg);
	void	Free();
};

END;