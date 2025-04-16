#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Engine)
class CPickingSys;
class CGameObject;
END

BEGIN(Client)

class CLevel_Boss final : public CLevel
{
private:
	CLevel_Boss(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CLevel_Boss() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_Monster(const _wstring& strLayerTag);

	void SpawnItems();
private:
	CPickingSys* m_pPickingSys = { nullptr };
	class CTrigger* m_pLevelTrigger{ nullptr };
	class CPlayer* m_pPlayer{ nullptr };
	list<CGameObject*> m_ItemList;

	_float m_fItemSpawnTime{ 0.f };

public:
	static CLevel_Boss* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free() override;
};

END