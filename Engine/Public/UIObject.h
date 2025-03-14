#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CUIObject abstract : public CGameObject
{
public:
	typedef struct tagUIObjectDesc
	{
		_float		fX, fY, fSizeX, fSizeY;
	}UIOBJECT_DESC;
protected:
	CUIObject(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUIObject(const CUIObject& Prototype);
	virtual ~CUIObject() = default;

public:	
	virtual HRESULT Initialize_Prototype();	
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	_float				m_fX = {};
	_float				m_fY = {};
	_float				m_fSizeX = {};
	_float				m_fSizeY = {};

	_float4x4			m_ViewMatrix = {};
	_float4x4			m_ProjMatrix = {};

	_float4x4			m_OldViewMatrix = {};
	_float4x4			m_OldProjMatrix = {};

protected:
	void Begin();
	void End();
	_bool isPick(HWND hWnd);



public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free();
};

END