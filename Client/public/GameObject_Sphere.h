#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Sphere;
class CCollider_Sphere;
class CCollider_Cube;
class CMaterial;
END

class CGameObject_Sphere final : public CGameObject
{
private:
	CGameObject_Sphere(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject_Sphere(const CGameObject& Prototype);
	virtual ~CGameObject_Sphere() = default;

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
public:
	virtual void Reset() {}; // 차후에 오브젝트 풀링때 SetActive가 true가 될 때 기본정보 다시 셋팅을 위한 함수

private:
	HRESULT Ready_Components();
	CTexture*			m_pTextureCom	= { nullptr };
	CTransform*			m_pTransformCom = { nullptr };
	CVIBuffer_Sphere*	m_pVIBufferCom	= { nullptr };
	CMaterial*			m_pMaterialCom  = { nullptr };

public:
	virtual json Serialize() override;
	virtual void Deserialize(const json& j) override;

public:
	static CGameObject_Sphere* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject_Sphere* Clone(void* pArg) override;
	virtual void Free() override;

	
};
