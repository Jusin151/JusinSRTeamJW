#pragma once

#include "Client_Defines.h"
#include "Base.h"

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void Update();
	HRESULT Render();

public:
	static CMainApp* Create();
	virtual void Free() override;

};

