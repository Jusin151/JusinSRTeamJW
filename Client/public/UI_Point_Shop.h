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


class CUI_Point_Shop final : public CUI_Base
{
private:
	CUI_Point_Shop(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUI_Point_Shop(const CUI_Point_Shop& Prototype);
	virtual ~CUI_Point_Shop() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Priority_Update(_float fTimeDelta)override;
	virtual void Update(_float fTimeDelta)override;
	virtual void Late_Update(_float fTimeDelta)override;
	virtual HRESULT Render()override;

private:
	CTexture* m_PointShopUI_pTextureCom{};
	CTransform* m_PointShopUI_pTransformCom{};
	CVIBuffer_Rect* m_PointShopUI_pVIBufferCom{};  // UI는 사각형이므로 Rect 버퍼 사용
	UI_Child_Desc m_PointShopUI_INFO{};

private:
	HRESULT Ready_Components();


public:
	static CUI_Point_Shop* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();

private:
	bool m_bIsVisible = {};
	bool m_bKeyPressed = {};
private:
	_float m_fArmor{};  // 체력
};


END
