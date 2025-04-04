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
class CStructure : public CCollisionObject
{
	enum class STRUCTURE_TYPE : uint8_t // 1바이트로 설정
	{
		NORMAL,
		OCEAN,  // 파도 효과가 있는 바닥
		BOSS_WALL,  // 보스 벽
		MAGMA
		// 필요에 따라 더 추가 가능
	};

private:
	friend class CMyImGui;


private:
	CStructure(LPDIRECT3DDEVICE9 pGraphic_Device);
	CStructure(const CStructure& Prototype);
	virtual ~CStructure() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Priority_Update(_float fTimeDelta)override;
	virtual void Update(_float fTimeDelta)override;
	virtual void Late_Update(_float fTimeDelta)override;
	virtual HRESULT Render()override;
	virtual HRESULT On_Collision(CCollisionObject* other) override;

private:
	CTexture* m_pTextureCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer* m_pVIBufferCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CMaterial* m_pMaterialCom = { nullptr };
	_bool m_bIsCubeCollider = { false };
	_wstring m_strVIBuffer;
	_wstring m_strCollProtoTag;
	_wstring m_strTextureTag;
	_ulong originalAddressU{}, originalAddressV{};

#pragma region 텍스쳐 스케일에 따라 반복
	_float m_fWaveTime = 0.f;   // 물결 애니메이션용 시간 변수
	_float m_fWaveSpeed = 0.5f;  // 물결 속도
	STRUCTURE_TYPE m_eStructureType = { STRUCTURE_TYPE::NORMAL };
#pragma endregion


#pragma region 허브 마그마 텍스쳐 애니메이션용

	_float m_fFrame = 0.f;   // 마그마 애니메이션용 프레임
	_uint m_iCurrentTexture = 0;  // 현재 텍스쳐 인덱스


#pragma endregion

	_bool	m_bInit = { false };

private:
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

	HRESULT Ready_Components();


public:
	static CStructure* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();

	virtual json Serialize() override;
	virtual void Deserialize(const json& j) override;
};
END
