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


class CUI_Upgrade_Weapon final : public CUI_Base
{
private:
	CUI_Upgrade_Weapon(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUI_Upgrade_Weapon(const CUI_Upgrade_Weapon& Prototype);
	virtual ~CUI_Upgrade_Weapon() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Priority_Update(_float fTimeDelta)override;
	virtual void Update(_float fTimeDelta)override;
	virtual void Late_Update(_float fTimeDelta)override;
	virtual HRESULT Render()override;

private:
	CTexture* m_Upgrade_UI_pTextureCom{};
	CTransform* m_Upgrade_UI_pTransformCom{};
	CVIBuffer_Rect* m_Upgrade_UI_pVIBufferCom{};  // UI는 사각형이므로 Rect 버퍼 사용
	UI_Child_Desc m_Upgrade_UI_INFO{};

private:
	HRESULT Ready_Components();


public:
	static CUI_Upgrade_Weapon* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();

private:
	bool m_bIsVisible = {};
	bool m_bKeyPressed = {};
private:
	_float m_fArmor{};  // 체력
};


END
