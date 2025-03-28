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
class CExp_Bar final : public CUI_Base
{
private:
	CExp_Bar(LPDIRECT3DDEVICE9 pGraphic_Device);
	CExp_Bar(const CExp_Bar& Prototype);
	virtual ~CExp_Bar() = default;

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
	static CExp_Bar* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();

private:
	_float m_fExp{};
};
END