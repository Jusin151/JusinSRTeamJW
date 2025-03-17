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


class CUI_Bullet_Bar final : public CUI_Base
{
private:
	CUI_Bullet_Bar(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUI_Bullet_Bar(const CUI_Bullet_Bar& Prototype);
	virtual ~CUI_Bullet_Bar() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Priority_Update(_float fTimeDelta)override;
	virtual void Update(_float fTimeDelta)override;
	virtual void Late_Update(_float fTimeDelta)override;
	void Update_Bullet_Bar();
	virtual HRESULT Render()override;

private:
	UI_Child_Desc m_Bullet_Bar_INFO{};

private:
	HRESULT Ready_Components();


public:
	static CUI_Bullet_Bar* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();



private:
	_float m_fBullet{100.f};
};
END


