#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CLight;
class CTransform;
class CVIBuffer_Cube;
END

class CGameObject_Light final : public CGameObject
{
private:
	CGameObject_Light(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject_Light(const CGameObject& Prototype);
	virtual ~CGameObject_Light() = default;

public:
	/* 원형이 호출하는 함수. 오래걸릴 수 있는 초기화작업 */
	virtual HRESULT Initialize_Prototype() override;
	/* 사본이 호출하는 함수. */
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
public:
	virtual HRESULT Pre_Render();
	virtual HRESULT Render() override;
	virtual HRESULT Post_Render();
	virtual void Reset() {}; // 차후에 오브젝트 풀링때 SetActive가 true가 될 때 기본정보 다시 셋팅을 위한 함수

private:
	HRESULT Ready_Components();
	CLight*			m_pLightCom		= { nullptr };
	CTransform*		m_pTransformCom = { nullptr };
	CVIBuffer_Cube* m_pVIBufferCom	= { nullptr };
	

public:
	static CGameObject_Light* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject_Light* Clone(void* pArg) override;
	virtual void Free() override;

	virtual json Serialize() override;
	virtual void Deserialize(const json& j) override;
};
