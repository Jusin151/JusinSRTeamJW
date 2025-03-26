#pragma once
#include "Client_Defines.h"
#include "Transform.h"
#include "CollisionObject.h"
#include "Serializable.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer;
class CCollider;
class CMaterial;
END

BEGIN(Client)
class CItem final : public CCollisionObject
{

	enum class ITEM_TYPE : uint8_t // 1바이트로 설정
	{
	  HP,
	  MP,
	  AMMO,
	  EXP,
	  STAT,
	  MAX
	};
private:
	friend class CMyImGui;


private:
	CItem(LPDIRECT3DDEVICE9 pGraphic_Device);
	CItem(const CItem& Prototype);
	virtual ~CItem() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Priority_Update(_float fTimeDelta)override;
	virtual void Update(_float fTimeDelta)override;
	virtual void Late_Update(_float fTimeDelta)override;
	virtual HRESULT Render()override;
	HRESULT On_Collision(_float fTimeDelta);

private:
	void Billboarding(_float fTimeDelta);
	void Bind_ResourceByType();
	void Use_Item();
	void Init_TextureTag();


private:
	CTexture* m_pTextureCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer* m_pVIBufferCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CMaterial* m_pMaterialCom = { nullptr };
	_bool m_bIsCubeCollider = { false };
	map<ITEM_TYPE, map<_wstring,_uint>> m_mapTextureTag;	
	class CPlayer* m_pPlayer = { nullptr };	
	//_wstring m_strVIBuffer;
	//_wstring m_strCollProtoTag;
	//_wstring m_strTextureTag;
	ITEM_TYPE m_eType{ ITEM_TYPE::MAX };

private:
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

	HRESULT Ready_Components();


public:
	static CItem* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();

	virtual json Serialize() override;
	virtual void Deserialize(const json& j) override;

	// CCollisionObject을(를) 통해 상속됨
	HRESULT On_Collision(CCollisionObject* other) override;
};
END


