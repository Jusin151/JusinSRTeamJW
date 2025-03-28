#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "UI_Base.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CTransform;
class CVIBuffer_Rect;  // UI는 사각형으로
class CMaterial;
END

BEGIN(Client)

// 캐릭터 
class CBackGround final : public CUI_Base
{
	
private:
	CBackGround(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBackGround(const CBackGround& Prototype);
	virtual ~CBackGround() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void	Priority_Update(_float fTimeDelta)override;
	virtual void	Update(_float fTimeDelta)override;
	virtual void	Late_Update(_float fTimeDelta)override;
	virtual HRESULT Render()override;

private:
	CShader*			m_pShaderCom	= { nullptr };
	CMaterial*			m_pMaterial		= { nullptr };
	BackGround_DESC		m_BackGround_INFO{};
	_float				accumulatedDistance{};
private:
	 HRESULT Ready_Components(const _wstring& strTextureTag=L"");

public:
	static CBackGround* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();

};


END
