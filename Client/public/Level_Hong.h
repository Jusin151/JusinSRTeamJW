#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Engine)
class CPickingSys;
END

BEGIN(Client)

class CLevel_Hong final : public CLevel
{
private:
	CLevel_Hong(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CLevel_Hong() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_UI();
	HRESULT Ready_Layer_Camera(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Player(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Monster(const _wstring& strLayerTag);
	HRESULT Ready_Layer_BackGround(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Weapon();
	HRESULT Ready_Layer_Weapon_Icon();
private:
	CPickingSys* m_pPickingSys = { nullptr };


public:
	static CLevel_Hong* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free() override;
};

END