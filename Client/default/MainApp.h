#pragma once

#include "Client_Defines.h"
#include "../../EngineSDK/Inc/Base.h"

BEGIN(Engine)
class CGraphic_Device;
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
	CGraphic_Device* m_pGraphic_Device = { nullptr };

public:
	static CMainApp* Create();
	virtual void Free() override;

};

END