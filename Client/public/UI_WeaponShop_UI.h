#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "UI_Shop_Base.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;  // UI는 사각형으로
END

BEGIN(Client)


class CUI_WeaponShop_UI final : public CUI_Shop_Base
{
private:
	CUI_WeaponShop_UI(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUI_WeaponShop_UI(const CUI_WeaponShop_UI& Prototype);
	virtual ~CUI_WeaponShop_UI() = default;

	// CShopUI_Base을(를) 통해 상속됨
	virtual HRESULT Initialize_Prototype() override;

	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;

	virtual void Update(_float fTimeDelta) override;

	virtual void Late_Update(_float fTimeDelta) override;

private:
	HRESULT Ready_Texture();


public:
	static CUI_WeaponShop_UI* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();

private:
	HRESULT Register_Buttons() override;


};

END
