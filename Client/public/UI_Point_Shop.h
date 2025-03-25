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


class CUI_Point_Shop final : public CUI_Shop_Base
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
	virtual HRESULT Render()override;
	HRESULT Ready_Texture();
	HRESULT Ready_Skill_Button_Text();
	HRESULT Ready_Stat_Button_Text();

public:
	static CUI_Point_Shop* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();

private:
	HRESULT Register_Buttons() override;

	void Create_SkillButton(); //오른쪽 특성관련 버튼
	void Create_StatButton();  //왼쪽 스탯버튼
private:
	vector<wstring> m_str_Skill_MouseOn_Text{}; // 스킬 버튼 마우스 올리면 보이는 텍스트
	vector<wstring> m_str_Skill_Default_Text{}; // 스킬 버튼안에 보여지는 텍스트'

	vector<wstring> m_str_Stat_MouseOn_Text{}; // 스탯 버튼 마우스 올리면 보이는 텍스트
	vector<wstring> m_str_Stat_Default_Text{}; // 스탯 버튼안에 보여지는 텍스트

	_bool bSkill_Button_Active{};
	_bool bStat_Button_Active{};
public:
	_bool m_bOnUI = { false };
};

END
