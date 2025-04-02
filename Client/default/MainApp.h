#pragma once

#include "Client_Defines.h"
#include "../../EngineSDK/Inc/Base.h"

ENGINE_DLL bool g_bDebugCollider;

BEGIN(Engine)
class CGameInstance;
class CPickingSys;
END

BEGIN(Client)
class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void Update(_float fTimeDelta);
	HRESULT Render();

private:
	CGameInstance*			m_pGameInstance = { nullptr };
	LPDIRECT3DDEVICE9		m_pGraphic_Device = { nullptr };
	CPickingSys*			m_pPickingSys = { nullptr };
private:
	HRESULT Ready_Default_Setting();
	HRESULT Ready_Component_For_Static();
	HRESULT Open_Level(LEVEL eLevelID);

	HRESULT Ready_Prototype_GameObject();
	HRESULT Ready_Prototype_Component();
	HRESULT Ready_Prototype_UI();
	HRESULT Ready_Prototype_Weapon();
	HRESULT Ready_Prototype_Inven();
	HRESULT Ready_Prototype_Player();
public:
	static CMainApp* Create();
	virtual void Free() override;

};

END