#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "CollisionObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CMaterial;
class CTransform;
class CSound_Source;
class CVIBuffer_Rect;
class CCollider_Cube;
class CCollider_Sphere;
END

BEGIN(Client)

class CMonster_Base abstract : public CCollisionObject
{
public:
	enum MONSTERSTATE { MS_IDLE, MS_BACK, MS_HIT, MS_WALK,  MS_ATTACK, MS_MORPH, MS_DEATH, MS_END };

protected:
	CMonster_Base(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMonster_Base(const CMonster_Base& Prototype);
	virtual ~CMonster_Base() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
private:
	// texturecom는 각 monster에서 추가
	HRESULT Ready_Components();

public:
	virtual void Priority_Update(_float fTimeDelta)override;
	virtual void Update(_float fTimeDelta)override;
	virtual void Late_Update(_float fTimeDelta)override;
public:
public:
	virtual HRESULT Render()override;
	virtual HRESULT On_Collision(CCollisionObject* other) { return S_OK; }
public:
	virtual void Select_Pattern(_float fTimeDelta) PURE;
	void Look_Player();
	void Chasing(_float fTimeDelta, _float fDist);

	void Set_Trigger();

protected:
	HRESULT Create_Stains(_uint effectNum);
	HRESULT Create_Gibs(_uint eType);

	

	// 트리거가 없어지거나 트리거 설정을 안하면 이제 플레이어로 타겟을 바꾸도록 유도... 플레이어가 없으면 동작 x.
	void SetTarget(CGameObject* pTarget) { if (nullptr == m_pTarget) m_pTarget = pTarget; }

	virtual json Serialize() override;
public:	
	CGameObject* Get_Target() { return m_pTarget; }
protected:
	CShader*		m_pShaderCom = { nullptr };
	CTexture*		m_pTextureCom = { nullptr };
	CMaterial*		m_pMaterialCom = { nullptr };
	CTransform*		m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CCollider_Cube* m_pColliderCom = { nullptr };
	
public:	
	CSound_Source* m_pSoundCom = { nullptr };
protected:

	// 일단 몬스터는 플레이어만 타겟으로 하도록. 다른 걸 타겟으로 하는 것은 나중에 하도록
	CGameObject*	m_pTarget = { nullptr };

	// 애니메이션 돌리기 위해.. 한 프레임 시간, 현재 프레임 수, 그리고 상태 당 텍스쳐 범위는 자식에서 설정
	_float m_fElapsedTime = { 0.f };
	_int m_iCurrentFrame = {0};

	// 몬스터 현재 상태, 이전 상태
	MONSTERSTATE m_eCurState = { MS_IDLE };
	MONSTERSTATE m_ePreState = { MS_IDLE };

	_bool		m_bCheck = { false };
	_bool		m_bGib = { false };


public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free();
};


END