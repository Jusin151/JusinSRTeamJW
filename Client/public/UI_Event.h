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


class CUI_Event final : public CUI_Base, public CObserver
{
public:
	struct EventText
	{
		wstring wstr;
		_float ftime;
	};

	vector<EventText> m_vecEventTexts;

public:
	enum HP_STATE { Default, Heated };
private:
	CUI_Event(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUI_Event(const CUI_Event& Prototype);
	virtual ~CUI_Event() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Priority_Update(_float fTimeDelta)override;
	virtual void Update(_float fTimeDelta)override;
	virtual void Late_Update(_float fTimeDelta)override;
	virtual HRESULT Render()override;

	_float3 HSVtoRGB(float h, float s, float v);
private:
	float m_fRainbowTimer = { 0.f };

public:
	static CUI_Event* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();
public:

	void ShowEventText(_int value, const wstring& tag)
	{
		wstring text;

		if (tag == L"Exp")
			text = L"경험치 획득!  +" + to_wstring(value);
		else if (tag == L"Hp")
			text = L"체력을 회복! +" + to_wstring(value);
		else if (tag == L"Magnum_Ammo_Small") 
			text = L"당신은 약간의 권총탄약을 얻었습니다! +" + to_wstring(value);
		else if (tag == L"Magnum_Ammo_Big")
			text = L"당신은 대량의 권총탄약을 얻었습니다! +" + to_wstring(value);
		else if (tag == L"ShotGun_Ammo_Small")
			text = L"당신은 약간의 샷건탄약을 얻었습니다! +" + to_wstring(value);
		else if (tag == L"ShotGun_Ammo_Big")
			text = L"당신은 대량의 샷건탄약을 얻었습니다! +" + to_wstring(value);
		else if (tag == L"Red")
			text = L"RED를 얻었다(수정바람) +" + to_wstring(value);
		else if (tag == L"Blue")
			text = L"Blue를 얻었다(수정바람) +" + to_wstring(value);
		else if (tag == L"Yellow")
			text = L"Yellow를 얻었다(수정바람) +" + to_wstring(value);
		else if (tag == L"Yellow")
			text = L"ShotGun 획득!";
		else if (tag == L"Yellow")
			text = L"Staff 획득!";
		else if (tag == L"Yellow")
			text = L"Minigun 획득!";
		else if (tag == L"LevelUp")
			text = L"레벨업!";
		else if (tag == L"Mp_End")
			text = L"마나가 부족합니다..!";
		else if (tag == L"HellBoss_Phase3")
		{
			text = L"악마가 위기감을 느끼기 시작했습니다..!";
			m_vecEventTexts.push_back({ text, 5.f });
			return;
		}

		m_vecEventTexts.push_back({ text, 1.5f }); // 1.5초간 유지
	}



private:
	_uint m_iType{};
	wstring m_strText = L"";
	_float m_fTextDuration = { 0.f }; // 몇 초 남았는지



	// CObserver을(를) 통해 상속됨
	void OnNotify(void* pArg, const wstring& type) override;

};
END


