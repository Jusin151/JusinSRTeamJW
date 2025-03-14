#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Button_Base.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;  // UI는 사각형으로
END

BEGIN(Client)


class CButton_Episode final : public CButton_Base
{
private:
	CButton_Episode(LPDIRECT3DDEVICE9 pGraphic_Device);
	CButton_Episode(const CButton_Episode& Prototype);
	virtual ~CButton_Episode() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Priority_Update(_float fTimeDelta)override;
	virtual void Update(_float fTimeDelta)override;
	virtual void Late_Update(_float fTimeDelta)override;
	virtual HRESULT Render()override;

private:
	CTexture* m_EpisodeUI_pTextureCom{};
	CTransform* m_EpisodeUI_pTransformCom{};
	CVIBuffer_Rect* m_EpisodeUI_pVIBufferCom{};  // UI는 사각형이므로 Rect 버퍼 사용
	UI_Child_Desc m_EpisodeUI_INFO{};

private:
	HRESULT Ready_Components();


public:
	static CButton_Episode* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();

private:
	bool m_bIsVisible = {};
	bool m_bKeyPressed = {};
private:
	_float m_fArmor{};  // 체력
};


END
