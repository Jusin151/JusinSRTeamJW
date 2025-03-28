#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "UI_Base.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;  // UI는 사각형으로
END

BEGIN(Client)
class CUI_Mid_Display final : public CUI_Base
{
private:
	CUI_Mid_Display(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUI_Mid_Display(const CUI_Mid_Display& Prototype);
	virtual ~CUI_Mid_Display() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Priority_Update(_float fTimeDelta)override;
	virtual void Update(_float fTimeDelta)override;
	void Update_Exp_Bar();
	virtual void Late_Update(_float fTimeDelta)override;
	virtual HRESULT Render()override;

private:
	UI_Child_Desc m_EXP_INFO{};

private:
	HRESULT Ready_Components();


public:
	static CUI_Mid_Display* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();

private:
	_float m_fExp{};  
};
END