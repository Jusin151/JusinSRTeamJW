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


class CPoint_Shop final : public CUI_Base
{
private:
	CPoint_Shop(LPDIRECT3DDEVICE9 pGraphic_Device);
	CPoint_Shop(const CPoint_Shop& Prototype);
	virtual ~CPoint_Shop() = default;

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
	static CPoint_Shop* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();

private:
	bool m_bIsVisible = {};
	bool m_bKeyPressed = {};
private:
	_float m_fArmor{};  // 체력
};


END
