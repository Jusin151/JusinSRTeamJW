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


class CUI_Exp_Bar final : public CUI_Base, public CObserver
{
public:
	enum HP_STATE { Default, Heated };
private:
	CUI_Exp_Bar(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUI_Exp_Bar(const CUI_Exp_Bar& Prototype);
	virtual ~CUI_Exp_Bar() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Priority_Update(_float fTimeDelta)override;
	virtual void Update(_float fTimeDelta)override;
	virtual void Late_Update(_float fTimeDelta)override;
	virtual HRESULT Render()override;

private:
	UI_Child_Desc m_Exp_Bar_INFO{};

private:
	HRESULT Ready_Components();
	void Update_Exp_Bar(); 

public:
	static CUI_Exp_Bar* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();
public:

	void Set_Exp(_uint iHealth)
	{
		m_iExp = iHealth;
		Update_Exp_Bar();
	}
	void Init_HP(_uint CurrentHP, _uint MaxHP)
	{
		m_iExp = CurrentHP;
		m_iMaxExp = MaxHP;
	}

	_uint Get_Health() { return m_iExp; }

public:
	virtual void OnNotify(void* pArg, const wstring& type)override
	{
		if (type == L"Exp")
		{
			Set_Exp(*reinterpret_cast<_uint*>(pArg));
		}
	}

private:
	_uint m_iExp{};
	_uint m_iMaxExp{};
	//_float fExp_Ratio{};


};
END


