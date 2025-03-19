#include "Anubis.h"
#include "Monster_Base.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Collider_Cube.h"
#include "Transform.h"
#include "Player.h"
#include "GameInstance.h"

CAnubis::CAnubis(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CMonster_Base(pGraphic_Device)
{
}

CAnubis::CAnubis(const CAnubis& Prototype)
	:CMonster_Base(Prototype)
{
}

HRESULT CAnubis::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAnubis::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(20.f, 1.f, 20.f));

	return S_OK;
}

void CAnubis::Priority_Update(_float fTimeDelta)
{
	
}

void CAnubis::Update(_float fTimeDelta)
{
	Select_Pattern(fTimeDelta);

	__super::Update(fTimeDelta);

	m_pColliderCom->Update_Collider();

	m_pGameInstance->Add_Collider(CG_MONSTER, m_pColliderCom);
}

void CAnubis::Late_Update(_float fTimeDelta)
{
	// 충돌 판정
	On_Collision(fTimeDelta);

	m_pGameInstance->Add_RenderGroup(CRenderer::RG_COLLIDER, this);
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this)))
		return;
}

HRESULT CAnubis::Render()
{
	if (FAILED(m_pTextureCom->Bind_Resource(0)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	SetUp_RenderState();

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	Release_RenderState();


	return S_OK;
}

HRESULT CAnubis::On_Collision(_float fTimeDelta)
{
	if (nullptr == m_pColliderCom)
		return E_FAIL;

	// 안바뀌면 충돌 안일어남
	if (m_pColliderCom->Get_Other_Type() == CG_END)
		return S_OK;

	_float3 fMTV = m_pColliderCom->Get_MTV();
	_float3 fPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float3 temp = { 1.f, 0.f, 1.f };

	switch (m_pColliderCom->Get_Other_Type())
	{
	case CG_PLAYER:

		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, fPos);
		//m_pTransformCom->Go_Backward(fTimeDelta);
		break;

	case CG_WEAPON:



		temp += fPos;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, temp);

		break;
	default:
		break;
	}

	// 충돌 처리 하고 다시 type을 수정
	m_pColliderCom->Set_Other_Type(CG_END);

	return S_OK;
}

void CAnubis::Select_Pattern(_float fTimeDelta)
{
	m_pTransformCom->Chase(static_cast<CPlayer*>(m_pTarget)->Get_TransForm()->Get_State(CTransform::STATE_POSITION), fTimeDelta * 0.25f, 2.f);
}

HRESULT CAnubis::SetUp_RenderState()
{
	// 일단 추가해보기

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); // 알파 값이 기준보다 크면 픽셀 렌더링
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200); // 기준값 설정 (0~255)

	return S_OK;
}

HRESULT CAnubis::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}

HRESULT CAnubis::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Anubis"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;
	_float3 fPos = { 10.f, 0.5f, 10.f };

	return S_OK;
}

CAnubis* CAnubis::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CAnubis* pInstance = new CAnubis(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CAnubis");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAnubis::Clone(void* pArg)
{
	CAnubis* pInstance = new CAnubis(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CAnubis");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnubis::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
}
