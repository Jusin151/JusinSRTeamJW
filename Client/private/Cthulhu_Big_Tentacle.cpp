#include "Cthulhu_Big_Tentacle.h"
#include "GameInstance.h"
#include "Collider_Cube.h"
#include "StructureManager.h"
#include <CthulhuMissile.h>
#include <Player.h>
#include <Cthulhu.h>

CCthulhu_Big_Tentacle::CCthulhu_Big_Tentacle(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CCollisionObject(pGraphic_Device)
{
}

CCthulhu_Big_Tentacle::CCthulhu_Big_Tentacle(const CCthulhu_Big_Tentacle& Prototype)
	:CCollisionObject(Prototype)
{
}

HRESULT CCthulhu_Big_Tentacle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCthulhu_Big_Tentacle::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;
	m_pTarget = m_pGameInstance->Find_Object(LEVEL_STATIC, L"Layer_Player");
	m_pTransformCom->Set_Scale(3.5f, 7.f, 2.f);

	return S_OK;
}

void CCthulhu_Big_Tentacle::Priority_Update(_float fTimeDelta)
{

}

void CCthulhu_Big_Tentacle::Update(_float fTimeDelta)
{
	if (m_pColliderCom)
	{
		m_pColliderCom->Set_WorldMat(m_pTransformCom->Get_WorldMat());

		m_pColliderCom->Update_Collider(TEXT("Com_Collider_Cube"), m_pTransformCom->Compute_Scaled());
		m_pGameInstance->Add_Collider(CG_ENVIRONMENT, m_pColliderCom);
	}

	Billboarding(fTimeDelta);
}

void CCthulhu_Big_Tentacle::Late_Update(_float fTimeDelta)
{
	_float3 fPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	fPos.y = m_fOffset;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, fPos);

	if (m_pGameInstance->IsAABBInFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Compute_Scaled()))
	{
		m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
	}
}

HRESULT CCthulhu_Big_Tentacle::Render()
{
	if (FAILED(m_pTextureCom->Bind_Resource(m_iCurrentFrame)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	m_pShaderCom->Set_Fog(_float3(0.247f, 0.55f, 0.407f), 1.f, 40.f);
	if (FAILED(m_pShaderCom->Bind_Transform()))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Material(m_pMaterialCom)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Lights()))
		return E_FAIL;
	SetUp_RenderState();
	if (FAILED(m_pShaderCom->Bind_Texture(m_pTextureCom, m_iCurrentFrame)))
		return E_FAIL;
	m_pShaderCom->Begin(1);

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;
	m_pShaderCom->End();
	Release_RenderState();
	return S_OK;
}

HRESULT CCthulhu_Big_Tentacle::On_Collision(CCollisionObject* other)
{
	return E_NOTIMPL;
}

void CCthulhu_Big_Tentacle::Billboarding(_float fTimeDelta)
{
	if (!m_pTarget) return;


	CTransform* pPlayerTransform = static_cast<CPlayer*>(m_pTarget)->Get_TransForm();

	_float3 vPlayerLook = pPlayerTransform->Get_State(CTransform::STATE_LOOK);

	vPlayerLook.y = 0.f;
	vPlayerLook.Normalize();

	_float3 vLook = -vPlayerLook;


	_float3 vUp = _float3(0.0f, 1.0f, 0.0f);
	_float3 vRight = vUp.Cross(vLook);
	vRight.Normalize();

	_float3 vNewUp = vLook.Cross(vRight);
	vNewUp.Normalize();

	_float3 vScale = m_pTransformCom->Compute_Scaled();

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight * vScale.x);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vNewUp * vScale.y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook * vScale.z);
}


void CCthulhu_Big_Tentacle::Set_Position(const _float3& vPos)
{
	if (m_pTransformCom)
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
}

HRESULT CCthulhu_Big_Tentacle::SetUp_RenderState()
{
	D3DXVECTOR2 vScaleFactor(1.f, 1.f);
	D3DXVECTOR2 vOffsetFactor(0.0f, 0.0f); // Y축 반전을 위한 오프셋 조정
	m_pShaderCom->Set_UVScaleFactor(&vScaleFactor);
	m_pShaderCom->Set_UVOffsetFactor(&vOffsetFactor);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 240);
	return S_OK;
}

HRESULT CCthulhu_Big_Tentacle::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}

HRESULT CCthulhu_Big_Tentacle::Ready_Components()
{
	// 컴포넌트 초기화 코드
	if (FAILED(__super::Add_Component(LEVEL_BOSS, TEXT("Prototype_Component_Texture_Cthulhu_Big_Tentacle"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	CCollider_Cube::COL_CUBE_DESC	ColliderDesc = {};
	ColliderDesc.pOwner = this;

	ColliderDesc.fScale = { 1.f,1.f,1.f };
	ColliderDesc.fLocalPos = { 0.f,0.f, 0.f };
	m_eType = CG_ENVIRONMENT;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Material */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Material"),
		TEXT("Com_Material"), reinterpret_cast<CComponent**>(&m_pMaterialCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_BaseShader"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

CCthulhu_Big_Tentacle* CCthulhu_Big_Tentacle::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCthulhu_Big_Tentacle* pInstance = new CCthulhu_Big_Tentacle(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to create CCthulhu_Big_Tentacle instance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCthulhu_Big_Tentacle::Clone(void* pArg)
{
	CCthulhu_Big_Tentacle* pClone = new CCthulhu_Big_Tentacle(*this);
	if (FAILED(pClone->Initialize(pArg)))
	{
		MSG_BOX("Failed to clone CCthulhu_Big_Tentacle instance");
		Safe_Release(pClone);
	}
	return pClone;
}

void CCthulhu_Big_Tentacle::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pMaterialCom);

}
