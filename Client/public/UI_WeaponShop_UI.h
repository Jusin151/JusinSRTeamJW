#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "UI_Shop_Base.h"
#include"Observer.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;  // UI는 사각형으로
END

BEGIN(Client)


class CUI_WeaponShop_UI final : public CUI_Shop_Base, public CObserver
{
private:
	CUI_WeaponShop_UI(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUI_WeaponShop_UI(const CUI_WeaponShop_UI& Prototype);
	virtual ~CUI_WeaponShop_UI() = default;

	// CShopUI_Base을(를) 통해 상속됨
	virtual HRESULT Initialize_Prototype() override;

	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;

	virtual void Update(_float fTimeDelta) override;

	virtual void Late_Update(_float fTimeDelta) override;

private:
	HRESULT Ready_Texture();
	HRESULT Ready_01_Page_Weapon_Button_Text();
	void Create_SkillButton();
public:

	HRESULT Render();
	
	static CUI_WeaponShop_UI* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();

private:
	HRESULT Register_Buttons() override;

private:
	vector<wstring> m_str_Weapon_MouseOn_Text{}; // 스킬 버튼 쿨릭하면 오른쪽에 보여질 텍스트

public:
	_bool m_bOnUI = { false };
	_uint m_iPlayerSkillPoint{ 0u };
public:
	void OnNotify(void* pArg, const _wstring& tag) override
	{
		if (tag == L"Open")
		{
			SetActive(true);
			Button_Set_Active(true);
			m_bOnUI = true;

			if (pArg)
			{
			   m_iPlayerSkillPoint = *static_cast<_uint*>(pArg);
			}
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
