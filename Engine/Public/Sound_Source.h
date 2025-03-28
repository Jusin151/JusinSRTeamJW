#pragma once
#include "Component.h"
class CSound_Manager;
class CSound_Event;

BEGIN(Engine)
class ENGINE_DLL CSound_Source final : CComponent
{
private:
	CSound_Source();
	CSound_Source(const CSound_Source& Prototype);
	virtual ~CSound_Source() = default;
private:
	CSound_Manager* m_pSound_Manager;
public:
	static	CSound_Source* Create();
	CSound_Source* Clone(void* pArg);
	void	Free();
};

END;