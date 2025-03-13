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
	virtual HRESULT Render()override;

private:
	CTexture* m_Bullet_Bar_pTextureCom{};
	CTransform* m_Bullet_Bar_pTransformCom{};
	CVIBuffer_Rect* m_Bullet_Bar_pVIBufferCom{};  // UI는 사각형이므로 Rect 버퍼 사용
	UI_Child_Desc m_Bullet_Bar_INFO{};

private:
	HRESULT Ready_Components();


public:
	static CUI_Bullet_Bar* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();



private:
	_float m_fBullet{};
};
END


