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


class CUI_Right_Display final : public CUI_Base
{
private:
	CUI_Right_Display(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUI_Right_Display(const CUI_Right_Display& Prototype);
	virtual ~CUI_Right_Display() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Priority_Update(_float fTimeDelta)override;
	virtual void Update(_float fTimeDelta)override;
	virtual void Late_Update(_float fTimeDelta)override;
	virtual HRESULT Render()override;

private:
	CTexture* m_Ammo_pTextureCom{};
	CTransform* m_Ammo_pTransformCom{};
	CVIBuffer_Rect* m_Ammo_pVIBufferCom{};  // UI는 사각형이므로 Rect 버퍼 사용
	UI_Child_Desc m_Ammo_INFO{};

private:
	HRESULT Ready_Components();


public:
	static CUI_Right_Display* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();
};


END
