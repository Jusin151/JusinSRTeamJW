#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Engine)
class CPickingSys;
class CTransform;
END

enum class MonsterType
{
	THINGY,
	LOOKER,
	HELLHOUND,
	TYPE_END
};
class CHellBoss;
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
	void Spawn_Item();
	void Spawn_Monsters(_bool bInterval = true);
	void Spawn_Monster(MonsterType type, const _float3& spawnPos);

private:
	CHellBoss* m_pHellboss{ nullptr };
	CPickingSys* m_pPickingSys = { nullptr };
	CTransform* m_pPlayerTransCom{ nullptr };

	_float m_fItemSpawnCooldownTime{ 10.f };
	_float m_fItemSpawnTime{ 0.f };

	_float m_fMonsterSpawnCooldownTime{ 20.f };
	_float m_fMonsterSpawnTime{ 0.f };

	

public:
	static CLevel_Hong* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free() override;
};

END