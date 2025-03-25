#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "UI_Shop_Base.h"
#include "GamePlay_Button.h"

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

	HRESULT Ready_Spell_Button_Text();


	HRESULT Render();

	void Create_SkillButton();



public:
	static CUI_Spell_Shop* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();

private:
	HRESULT Register_Buttons() override;

private:
	vector<wstring> m_str_Spell_Text{};  // 스펠 설명
	vector<wstring> m_str_ToolTip_Text{};  // 마우스와 상관없이 버튼 위에 올라와있을텍스트 


	_bool m_bOnUI = { false };

	CGamePlay_Button* m_pSelectedSpellButton = { nullptr };
};

END
