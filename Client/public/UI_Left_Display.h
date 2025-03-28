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


class CUI_Left_Display final : public CUI_Base
{
private:
	CUI_Left_Display(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUI_Left_Display(const CUI_Left_Display& Prototype);
	virtual ~CUI_Left_Display() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Priority_Update(_float fTimeDelta)override;
	virtual void Update(_float fTimeDelta)override;
	virtual void Late_Update(_float fTimeDelta)override;
	virtual HRESULT Render()override;

private:
	CTexture* m_HP_pTextureCom{};
	CTransform* m_HP_pTransformCom{};
	CVIBuffer_Rect* m_HP_pVIBufferCom{};  // UI는 사각형이므로 Rect 버퍼 사용
	UI_Child_Desc m_HP_INFO{};

private:
	HRESULT Ready_Components();


public:
	static CUI_Left_Display* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();



private:
	_float m_fHealth{};  
};
END


