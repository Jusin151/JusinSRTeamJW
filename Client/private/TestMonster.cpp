#include "TestMonster.h"

#include "GameInstance.h"
#include "Collider_Sphere.h"
#include "Collider_Cube.h"

CTestMonster::CTestMonster(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{
}

CTestMonster::CTestMonster(const CTestMonster& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CTestMonster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTestMonster::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(20.f,1.f,20.f));
	m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);
	return S_OK;
}

void CTestMonster::Priority_Update(_float fTimeDelta)
{
}

void CTestMonster::Update(_float fTimeDelta)
{

	m_pColliderCom->Update_Collider();

	m_pGameInstance->Add_Collider(CG_MONSTER, m_pColliderCom);

}

void CTestMonster::Late_Update(_float fTimeDelta)
{
	// 충돌 판정
	On_Collision(fTimeDelta);

	//if (Find(m_))
	m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CTestMonster::Render()
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

HRESULT CTestMonster::On_Collision(_float fTimeDelta)
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
		
		fPos += fMTV;

		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, fPos);
		m_pTransformCom->Go_Straight(fTimeDelta);
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

HRESULT CTestMonster::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);
	m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	/*m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);*/

	return S_OK;
}

HRESULT CTestMonster::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	//	m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		//m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}

HRESULT CTestMonster::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Player"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 10.f, D3DXToRadian(90.f) };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* For.Com_Collider */
	CCollider_Cube::COL_CUBE_DESC	ColliderDesc = {};
	ColliderDesc.eType = CG_MONSTER;
	ColliderDesc.pOwner = this;
	// 이걸로 콜라이더 크기 설정
	ColliderDesc.fScale = { 1.f, 1.f, 1.f };
	// 오브젝트와 상대적인 거리 설정
	ColliderDesc.fLocalPos = { 0.f, 0.f, 0.f };


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

CTestMonster* CTestMonster::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTestMonster* pInstance = new CTestMonster(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTestMonster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTestMonster::Clone(void* pArg)
{
	CTestMonster* pInstance = new CTestMonster(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CTestMonster");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CTestMonster::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pColliderCom);


}
