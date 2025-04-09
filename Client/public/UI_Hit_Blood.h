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

class CUI_Hit_Blood final : public CUI_Base
{
private:
	CUI_Hit_Blood(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUI_Hit_Blood(const CUI_Hit_Blood& Prototype);
	virtual ~CUI_Hit_Blood() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
private:
	HRESULT Ready_Components();
public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
public:
	virtual HRESULT Pre_Render();
	virtual HRESULT Render();
	virtual HRESULT Post_Render();

private:
	CTexture*		m_pTextureCom	= { nullptr };
	CTransform*		m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom	= { nullptr };  // UI는 사각형이므로 Rect 버퍼 사용
	UI_Parent_Desc m_UI_INFO{};
	_float			m_fElpasedTime = { 0.f };
	_float			m_fAnimationTime = { 2.0f };
	_float			m_fAlphaTestValue = { 0.0f };
protected:
	bool m_bIsUI{};

public:
	static CUI_Hit_Blood* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};
END