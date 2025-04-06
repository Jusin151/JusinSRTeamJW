#pragma once
#include "Client_Defines.h"
#include "Level.h"

BEGIN(Engine)
class CPickingSys;
END

BEGIN(Client)
class CLevel_Test final : public CLevel
{
private:
	CLevel_Test(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CLevel_Test() = default;

public:
	virtual HRESULT		Initialize() override;
	virtual void		Update(_float fTimeDelta) override;
	virtual HRESULT		Render() override;

private:
	class CMyImGui* m_pImgui = { nullptr };

public:
	static CLevel_Test* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free() override;
};

END