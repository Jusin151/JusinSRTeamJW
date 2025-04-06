#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTransform;
class CTexture;
class CVIBuffer_Rect;
class CParticle_System;
END

BEGIN(Client)
class CGameObject_Snow final : public CGameObject
{
private:
	CGameObject_Snow(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject_Snow(const CGameObject_Snow& Prototype);
	virtual ~CGameObject_Snow() = default;
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
private:
	HRESULT Ready_Components();
public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
public:
	virtual HRESULT Pre_Render();
	virtual HRESULT Render() override;
	virtual HRESULT Post_Render();
public:
	virtual void Reset() {}; // 차후에 오브젝트 풀링때 SetActive가 true가 될 때 기본정보 다시 셋팅을 위한 함수

private:
	
	CParticle_System*	m_pParticleCom	= { nullptr };
	CTransform*			m_pTransformCom = { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom	= { nullptr };
public:
	static CGameObject_Snow* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject_Snow* Clone(void* pArg) override;
	virtual void Free() override;

	virtual json Serialize() override;
	virtual void Deserialize(const json& j) override;
};
END

