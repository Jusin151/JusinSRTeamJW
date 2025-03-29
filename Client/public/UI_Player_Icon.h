#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "UI_Base.h"
#include "UI_HP_Bar.h"
#include "Observer.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;  // UI는 사각형으로
END

BEGIN(Client)


class CUI_Player_Icon final : public CUI_Base, public CObserver
{
private:
	CUI_Player_Icon(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUI_Player_Icon(const CUI_Player_Icon& Prototype);
	virtual ~CUI_Player_Icon() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Priority_Update(_float fTimeDelta)override;
	virtual void Update(_float fTimeDelta)override;
	virtual void Late_Update(_float fTimeDelta)override;
	virtual HRESULT Render()override;

private:
	CTexture* m_PlayerICon_pTextureCom{};
	CTransform* m_PlayerICon_pTransformCom{};
	CVIBuffer_Rect* m_PlayerICon_pVIBufferCom{};  // UI는 사각형이므로 Rect 버퍼 사용
	UI_Child_Desc m_PlayerICon_INFO{};

private:
	HRESULT Ready_Components();
	void Update_Animation(_float fTimeDelta);
	void Update_Hit_Animation(_float fTimeDelta);

public:
	static CUI_Player_Icon* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();
public: //UI 매니저 호출용
	void Set_Hp_Event();
private:
	_float m_fElapsedTime = 0.0f; 
	_int m_iCurrentFrame = 0; 
	const _float m_fFrameDuration = 2.0f;  
	const _int m_iFrameCount = 4; 
private:
	bool m_bIsHit = {}; // 
	_float m_fHitElapsedTime = 0.0f; 
	const _float m_fHitDuration = 1.0f; 

private:
	_int m_iPriorityHp{};
	_int m_iLateHp{};

virtual	void OnNotify(_int value, const wstring& type) 
	{
		if (type == L"HP")
		{
			Set_Hp_Event();
		}
	}

};
END


