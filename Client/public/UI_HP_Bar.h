#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "UI_Base.h"
#include "Observer.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;  // UI는 사각형으로
END

BEGIN(Client)


class CUI_HP_Bar final : public CUI_Base, public CObserver
{
public:
	enum HP_STATE{ Default,Heated};
private:
	CUI_HP_Bar(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUI_HP_Bar(const CUI_HP_Bar& Prototype);
	virtual ~CUI_HP_Bar() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Priority_Update(_float fTimeDelta)override;
	virtual void Update(_float fTimeDelta)override;
	virtual void Late_Update(_float fTimeDelta)override;
	virtual HRESULT Render()override;

private:
	UI_Child_Desc m_HP_Bar_INFO{};

private:
	HRESULT Ready_Components();
	void Update_HP_Bar();

public:
	static CUI_HP_Bar* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();
public:

	void Set_HP(_uint iHealth)
	{
		m_iHealth = iHealth;
		Update_HP_Bar();
	}
	void Set_MaxHP(_uint iHealth)
	{
		m_iMaxHealth = iHealth; 
		Update_HP_Bar();
	}
	void Init_HP(_uint CurrentHP, _uint MaxHP)
	{
		m_iHealth = CurrentHP;
		m_iMaxHealth = MaxHP;
	}

	_uint Get_Health() { return m_iHealth; }

public:
	virtual void OnNotify(void* pArg, const wstring& type)override
	{
		if (type == L"HP")
		{
			Set_HP(*reinterpret_cast<_uint*>(pArg));
		}
		if (type == L"HP_Max")
		{
			Set_MaxHP(*reinterpret_cast<_uint*>(pArg)); 
		}
		if (type == L"HP_Hited")
		{
			Set_HP(*reinterpret_cast<_uint*>(pArg));
		}
	}

private:
	_uint m_iHealth{};
	_uint m_iMaxHealth{};
	HP_STATE m_eHp_State{};
	_float fHP_Ratio{};
	

};
END


