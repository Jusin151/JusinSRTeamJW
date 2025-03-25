#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "UI_Shop_Base.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;  // UI는 사각형으로
END

BEGIN(Client)


class CUI_Spell_Shop final : public CUI_Shop_Base
{
private:
	CUI_Spell_Shop(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUI_Spell_Shop(const CUI_Spell_Shop& Prototype);
	virtual ~CUI_Spell_Shop() = default;

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

	HRESULT Render();

	void Create_SkillButton();

	void Create_StatButton();


public:
	static CUI_Spell_Shop* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();

private:
	HRESULT Register_Buttons() override;

private:
	vector<wstring> m_str_Skill_MouseOn_Text{}; // 스킬 버튼 마우스 올리면 보이는 텍스트
	vector<wstring> m_str_Skill_Default_Text{}; // 스킬 버튼안에 보여지는 텍스트'

	vector<wstring> m_str_Stat_MouseOn_Text{}; // 스탯 버튼 마우스 올리면 보이는 텍스트
	vector<wstring> m_str_Stat_Default_Text{}; // 스탯 버튼안에 보여지는 텍스트

	_bool m_bOnUI = { false };

};

END
