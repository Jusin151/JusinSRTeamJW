#pragma once

/* 엔진과 클라이언트 간의 링크의 역활을 수행한다. */
/* 엔진 내에 존재하는 유일한 싱글톤 클래스이다. */
/* 엔진 개발자가 클라개밫자에게 보여주고싶은 함수를 ... */
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance);
private:
	CGameInstance();
	virtual ~CGameInstance() = default;


public:
	HRESULT Initialize_Engine(const ENGINE_DESC& EngineDesc);
	HRESULT Draw();


private:
	class CGraphic_Device* m_pGraphic_Device = { nullptr };


public:
	virtual void Free() override;
};

END