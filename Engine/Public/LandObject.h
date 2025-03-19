#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CLandObject abstract  : public CGameObject
{
public:
	typedef struct tagLandObjectDesc
	{
		_uint		iLevelIndex;
		_wstring	strLayerTag;
		_uint		iIndex;
		_wstring	strComponentTag;
	}LANDOBJECT_DESC;
protected:
	CLandObject(LPDIRECT3DDEVICE9 pGraphic_Device);
	CLandObject(const CLandObject& Prototype);
	virtual ~CLandObject() = default;

public:
	virtual HRESULT Initialize_Prototype();	
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	class CVIBuffer*		m_pTargetBuffer = { nullptr };

protected:
	void SetUp_HeightPosition(class CTransform* pTransform, _float fOffset = 0.f);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free();


};

END