#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Ending final : public CLevel
{
private:
	CLevel_Ending(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CLevel_Ending() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	HRESULT Ready_Layer_BackGround();

private:


public:
	static CLevel_Ending* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	static _bool bStart;
	virtual void Free() override;
};

END