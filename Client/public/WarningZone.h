#include "Client_Defines.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Transform.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"
#include "CollisionObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CMaterial;
class CVIBuffer_Rect; 
class CTransform;  
class CCollider_Cube;
END

struct WARNING_ZONE_DESC
{
	_float3 vPos;
	_float3 vScale;
};


BEGIN(Client)


class CWarningZone : public CGameObject
{
private:
	CWarningZone(LPDIRECT3DDEVICE9 pGraphic_Device);
	CWarningZone(const CWarningZone& Prototype);
	virtual ~CWarningZone() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void Reset() override;

public:

	void Set_Position(const _float3& vPos);
	void Set_Render(_bool bFlag) { m_bCanRender = bFlag; }
private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
private:
	CTransform* m_pTransformCom{ nullptr };
	CShader* m_pShaderCom {nullptr};
	CTexture* m_pTextureCom {nullptr};
	CMaterial* m_pMaterialCom  {nullptr};
	CVIBuffer_Rect* m_pVIBufferCom{ nullptr };
	CCollider_Cube* m_pColliderCom{ nullptr };

	WARNING_ZONE_DESC		m_WarningZoneDesc;
	
	_float					m_fCurrentTime = 0.f;
	_float					m_fLifeTime = 3.0f;	
	_float3 m_vOriginPos{};
	_float3 m_vOriginScale{};
	_bool m_bIsCloned{ false };
	_bool m_bCanRender{ false };

public:	
	static CWarningZone* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END

