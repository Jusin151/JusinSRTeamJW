#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Engine)
class CPickingSys;
class CTransform;
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
	void Spawn_Item();

private:
	CPickingSys* m_pPickingSys = { nullptr };
	CTransform* m_pPlayerTransCom{ nullptr };

	_float m_fItemSpawnCooldownTime{ 20.f };
	_float m_fItemSpawnTime{ 0.f };

public:
	static CLevel_Hong* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free() override;
};

END