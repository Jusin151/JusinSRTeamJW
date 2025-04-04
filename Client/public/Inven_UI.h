#pragma once
#include "Client_Defines.h"
#include "UI_Base.h"
#include "Transform.h"
#include "Image.h"
#include "Image_Manager.h"
#include "Observer.h"

BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;  // UI는 사각형으로 할듯
END

BEGIN(Client)

class CInven_UI : public CUI_Base, public CObserver
{
public:

	typedef struct tagImage
	{
		_float2 vPos{};
		_float2 vSize{};
		_float fAlpha{};
		_wstring TextureKey{};
	}Image_DESC;
public:
	CInven_UI(LPDIRECT3DDEVICE9 pGraphic_Device);
	CInven_UI(const CInven_UI& Prototype);
	virtual ~CInven_UI() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render()override;
	virtual HRESULT Ready_Components();

	void Add_WeaponIcon(pair<const wstring&, _uint>& weaponIconTagAndiIndex) //인벤의 Add_Weapon에서 호출됨
	{
		CImage* pImage = CImage_Manager::GetInstance()->Get_WeaponIcon(weaponIconTagAndiIndex.first);

		if (weaponIconTagAndiIndex.second < 0 || weaponIconTagAndiIndex.second >= m_pWeaponIcon.size()) return;
		if (pImage != nullptr)
		{
			m_pWeaponIcon[weaponIconTagAndiIndex.second] = pImage;		
		}
	}

	void WeaponIcon_isActive(_uint Index)
	{
		for (auto& it : m_pWeaponIcon)
		{
			if (it)
				it->Select_WeaponIcon(false);
		}

		if (m_pWeaponIcon[Index] != nullptr)
			m_pWeaponIcon[Index]->Select_WeaponIcon(true);

		return;
	}

	void Inven_OnOff(_bool type)
	{
		m_bRender = type; // 인벤창 off

		for (auto& it : m_pWeaponIcon)
		{
			if (it)
				it->SetActive(m_bRender);
		}
	}
public:
	static CInven_UI* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject* Clone(void* pArg);
	void Free();

private:
	Image_DESC m_Image_INFO;
	vector<CImage*> m_pWeaponIcon{};
	CImage* asdasd = { nullptr };
	unordered_map<wstring, CImage*> m_Map_pWeaponIcon = {};

	void OnNotify(void* pArg, const wstring& tag)
	{

		if (tag == L"Open")
		{
			Inven_OnOff(true);  // 인벤창 on
		}
		else if (tag == L"Close")
		{
			Inven_OnOff(false);  // 인벤창 off
		}
		else if (tag == L"AddWeaponIcon") // 해당 무기의 아이콘도 같이 추가할께
		{
			pair<const wstring&, _uint>& WeaponIconTagAndIndex = *static_cast<pair<const wstring&, _uint>*>(pArg);
			Add_WeaponIcon(WeaponIconTagAndIndex);
		}
		else if (tag == L"IconActive")
		{
			_uint Index = *static_cast<_uint*>(pArg);
			WeaponIcon_isActive(Index);
		}

	}

private: // 인빈 이미지에 관련된 변수들
	_bool m_bRender = { false };
};
END
