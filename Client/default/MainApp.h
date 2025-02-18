#pragma once

#include "Client_Defines.h"
#include "../../EngineSDK/Inc/Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void Update();
	HRESULT Render();

private:
	CGameInstance*			m_pGameInstance = { nullptr };

public:
	static CMainApp* Create();
	virtual void Free() override;

};

END