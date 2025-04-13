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

	void Set_Exp(_uint Exp)
	{
		
		m_iExp = Exp;
		Update_Exp_Bar();
		
	}
	void Init_EXP(_uint CurrentExp, _uint MaxExp)
	{
		m_iExp = CurrentExp;
		m_iMaxExp = MaxExp;

		Update_Exp_Bar();
	}

public:
	virtual void OnNotify(void* pArg, const wstring& type)override
	{
		if (type == L"Exp")
		{
			Set_Exp(*reinterpret_cast<_uint*>(pArg));
		}
	}
	void ShowExpText(_int iExp)
	{
		m_strExpText = L"+EXP " + to_wstring(iExp);
		m_fExpTextDuration = 1.5f;  
	}

private:
	_uint m_iExp{};
	_uint m_iMaxExp{};
	_uint m_iLevel{}; 
	// 경험치 텍스트용
	wstring m_strExpText = L"";
	_float m_fExpTextDuration = 0.f; // 몇 초 남았는지



};
END


