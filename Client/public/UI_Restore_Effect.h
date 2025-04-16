#pragma once

#include "Observer.h"
#include "Client_Defines.h"
#include "GameObject.h"
#include "UI_Base.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;  // UI는 사각형으로
END

BEGIN(Client)

class CUI_Restore_Effect final : public CUI_Base, public CObserver
{
private:
	CUI_Restore_Effect(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUI_Restore_Effect(const CUI_Restore_Effect& Prototype);
	virtual ~CUI_Restore_Effect() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
private:
	HRESULT Ready_Components();
public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
public:
	virtual HRESULT Pre_Render();
	virtual HRESULT Render();
	virtual HRESULT Post_Render();
public:
	void StartEffect();
	virtual void OnNotify(void* pArg, const wstring& type) override
	{
		if (type == L"HP")
		{
			StartEffect();
		}
	}
private:
	_bool			m_bStart = { false };
	UI_Parent_Desc	m_UI_INFO{};
	_float			m_fElpasedTime = { 0.f };
	_float			m_fAnimationTime = { 1.0f };
	_float			m_fAlphaTestValue = { 0.0f };
private:
	bool m_bIsUI{};

public:
	static CUI_Restore_Effect* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};
END