#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "UI_Shop_Base.h"
#include "Hub_PointShop.h"
#include "GamePlay_Button.h"
#include "Observer.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;  // UI는 사각형으로
END

BEGIN(Client)


class CUI_Point_Shop final : public CUI_Shop_Base, public CObserver
{
private:
	CUI_Point_Shop(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUI_Point_Shop(const CUI_Point_Shop& Prototype);
	virtual ~CUI_Point_Shop() = default;

	// CShopUI_Base을(를) 통해 상속됨
	virtual HRESULT Initialize_Prototype() override;

	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;

	virtual void Update(_float fTimeDelta) override;

	virtual void Late_Update(_float fTimeDelta) override;

private:
	HRESULT Ready_Texture();
	HRESULT Ready_Skill_Button_Text();
	HRESULT Ready_Stat_Button_Text();
public:
	virtual HRESULT Render()override;

public:
	static CUI_Point_Shop* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();

private:
	HRESULT Register_Buttons() override;
	void Create_SkillButton(); // 특성관련 버튼
	void Create_StatButton();  // 스탯버튼
private:
	class CHub_PointShop* m_pPointShop = { nullptr };

public:
	void Set_Shop(CHub_PointShop* pShop) { m_pPointShop = pShop; }



private:
	vector<wstring> m_str_Skill_MouseOn_Text{}; // 스킬 버튼 마우스 올리면 보이는 텍스트
	vector<wstring> m_str_Skill_Default_Text{}; // 스킬 버튼안에 보여지는 텍스트'

	vector<wstring> m_str_Stat_MouseOn_Text{}; // 스탯 버튼 마우스 올리면 보이는 텍스트
	vector<wstring> m_str_Stat_Default_Text{}; // 스탯 버튼안에 보여지는 텍스트

	_bool bSkill_Button_Active{};
	_bool bStat_Button_Active{};
public:
	_bool m_bOnUI = { false };
private:
	_uint m_iStr{};
	_uint m_iMaxHP{};
	_uint m_iSprit{};
	_uint m_iCapacity{};
	_uint m_iLevel{};
	_uint m_iPoint{};

	vector<wstring> m_str_ToolTip_Text{};  // 마우스와 상관없이 버튼 위에 올라와있을텍스트 

	CGamePlay_Button* m_pSelectedSpellButton = { nullptr };
public:

	void OnNotify(void* pArg, const wstring& tag) 
	{
		if (tag == L"StatBuy")
		{
			int statType = *reinterpret_cast<_int*>(pArg); 

			switch (statType)
			{
			case 0: ++m_iStr; break;
			case 1: ++m_iMaxHP; break;
			case 2: ++m_iSprit; break;
			case 3: ++m_iCapacity; break;
			}
		}
		if (tag == L"Open")
		{
			SetActive(true);
			Button_Set_Active(true);
			m_bOnUI = true;
		}
		else if (tag == L"Close")
		{
			SetActive(false);
			Button_Set_Active(false);
			m_bOnUI = false;
		}

	}


};

END
