#pragma once

#include "Base.h"
#include "Serializable.h"

BEGIN(Engine)

class ENGINE_DLL CComponent abstract : public CBase, public ISerializable
{
protected:
	CComponent(LPDIRECT3DDEVICE9 pGraphic_Device);
	CComponent(const CComponent& Prototype);
	virtual ~CComponent() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void	ImGuiDisplay();

protected:
	LPDIRECT3DDEVICE9				m_pGraphic_Device = { nullptr };
	class CGameInstance*			m_pGameInstance = { nullptr };

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;

public:
	virtual json Serialize() override { return json(); };
	virtual void Deserialize(const json& j) {};
};

END