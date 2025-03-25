#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "CUI_Base.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;  // UI는 사각형으로
END

BEGIN(Client)


class CUI_HP_Bar final : public CUI_Base
{
public:
	enum HP_STATE{ Default,Heated};
private:
	CUI_HP_Bar(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUI_HP_Bar(const CUI_HP_Bar& Prototype);
	virtual ~CUI_HP_Bar() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Priority_Update(_float fTimeDelta)override;
	virtual void Update(_float fTimeDelta)override;
	virtual void Late_Update(_float fTimeDelta)override;
	virtual HRESULT Render()override;

private:
	UI_Child_Desc m_HP_Bar_INFO{};

private:
	HRESULT Ready_Components();
	void Update_HP_Bar(); 

public:
	static CUI_HP_Bar* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();
public:
	HP_STATE Get_HpState() { return m_eHp_State; }
	void Set_Damage(_int Damage) 
	{ 
		
		m_iHealth -= Damage;
			if (m_iHealth < 0 || m_iHealth > 100)
				m_iHealth = 0; // 최소 체력 제한

			Update_HP_Bar();
		
	}
	_uint Get_Health() { return m_iHealth; }

private:
	_uint m_iHealth{};
	HP_STATE m_eHp_State{};

};
END


