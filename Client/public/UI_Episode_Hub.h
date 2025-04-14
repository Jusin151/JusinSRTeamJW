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


class CUI_Episode_Hub final : public CUI_Shop_Base
{
	enum MAP_IMAGE_ENUM
	{
		FIRST_NORMAL_MAP,

		FIRST_BOSS_MAP,SECOND_BOSS_MAP,MAP_END
	};
	enum MAP_STATE_ENUM
	{
		FIRST_NORMAP_MAP_COLOR = 0,
		FIRST_NORMAP_MAP_RED = 1,

		FIRST_BOSS_MAP_GRAY = 2,
		FIRST_BOSS_MAP_COLOR = 3,
		FISRT_BOSS_MAP_RED = 4,

		SECOND_BOSS_MAP_GRAY=5,
		SECOND_BOSS_MAP_COLOR=6,
		SECOND_BOSS_MAP_RED=7,
	};

private:
	CUI_Episode_Hub(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUI_Episode_Hub(const CUI_Episode_Hub& Prototype);
	virtual ~CUI_Episode_Hub() = default;

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
	static CUI_Episode_Hub* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();

private:
	HRESULT Register_Buttons() override;
	void Create_Episode_Button(); // 에피소드 버튼  , 큰분기
	void Create_Episode_Select_Button();  //초록색 셀렉트 버튼
	void Create_Episode_Icon_Image(); // 셀렉트 안의 맵 이미지
public:
	void Change_Button_Image(class CGamePlay_Button* type,_uint FirstImage,_uint SecondImage);
protected:
	vector<class CGamePlay_Button*> m_vecMapButtons;

public:
	void Button_Map_Set_Active(_bool bActive)
	{
		m_bIsActive = bActive;

		for (auto& pButton : m_vecMapButtons)
		{
			if (pButton)
				pButton->SetActive(bActive); // 버튼도 같이 on/off
		}
	}
private:
	vector<wstring> m_str_Skill_MouseOn_Text{}; // 스킬 버튼 마우스 올리면 보이는 텍스트
	vector<wstring> m_str_Skill_Default_Text{}; // 스킬 버튼안에 보여지는 텍스트'

	vector<wstring> m_str_Stat_MouseOn_Text{}; // 스탯 버튼 마우스 올리면 보이는 텍스트
	vector<wstring> m_str_Stat_Default_Text{}; // 스탯 버튼안에 보여지는 텍스트

	_bool bSkill_Button_Active{};
	_bool bStat_Button_Active{};
public:
	_bool m_bOnUI = { false };

public:
	void OnNotify(void* pArg, const wstring& tag)
	{

		if (tag == L"Open")
		{
			SetActive(true);
			Button_Set_Active(true);
			Button_Map_Set_Active(true);
			m_bOnUI = true;
		}
		else if (tag == L"Close")
		{
			SetActive(false);
			Button_Set_Active(false);
			Button_Map_Set_Active(false);
			m_bOnUI = false;
		}

	}

private:
	_float2 m_fFisrt_Button_Pos{};
	_float2 m_fSecond_Button_Pos{};
	_float2 m_fThird_Button_Pos{};
	class CHub_Portal* m_pPortal{ nullptr };

};

END
