
#include "PickingSys.h"
#include "GameInstance.h"
#include "Collider_Sphere.h"
#include "Collider_Cube.h"
#include "Camera_FirstPerson.h"
#include "Melee_Weapon.h"


CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLandObject{ pGraphic_Device }
{
}

CPlayer::CPlayer(const CPlayer& Prototype)
	: CLandObject( Prototype )
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	//m_tObjDesc.stBufferTag = TEXT("Prototype_Component_VIBuffer_Cube");
	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	/*CLandObject::LANDOBJECT_DESC		Desc{};
	Desc.iLevelIndex = LEVEL_GAMEPLAY;
	Desc.strLayerTag = TEXT("Layer_BackGround");
	Desc.strComponentTag = TEXT("Com_VIBuffer");
	Desc.iIndex = 0;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;*/

	
	if (!pArg)
	{
	m_tObjDesc.iLevel = 3;
	m_tObjDesc.stBufferTag = TEXT("Prototype_Component_VIBuffer_Cube");
	m_tObjDesc.stProtTextureTag = TEXT("Prototype_Component_Texture_Player");
	m_tObjDesc.iProtoLevel = 3;
	}
	else
	{
	OBJECT_DESC* pDesc = static_cast<OBJECT_DESC*>(pArg);
	m_tObjDesc = *pDesc;
	}
 	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(0.f, 1.f, 0.f));
	m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);
	//m_pColliderCom->Set_Radius(5.f);
	//m_pColliderCom->Set_Scale(_float3(1.f, 1.f, 1.f));

	m_iPlayerHP = { 100,100 };
	m_iPlayerMP = { 50, 50 };
	m_iPlayerBullet = { 0,0 }; // 총기류 마다 다른 총알 개수 받아올 예정
	m_iPlayerEXP = { 0 , 100};

	//m_pPlayer_Inven = CInventory::GetInstance(); 
	CPickingSys::Get_Instance()->Set_Player(this);
	return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
	// GetComponet 테스트용
	//CTransform* pTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Com_Transform")));

	//if (pTransform)
	//{
	//	pTransform->Go_Straight(fTimeDelta);
	//}
}

void CPlayer::Update(_float fTimeDelta)
{
	Move(fTimeDelta); 

	Inven_Update(fTimeDelta);
	m_pColliderCom->Update_Collider(TEXT("Com_Transform"), m_pColliderCom->Get_Scale());
	

	m_pGameInstance->Add_Collider(CG_PLAYER, m_pColliderCom);
	
	//__super::SetUp_HeightPosition(m_pTransformCom, 0.5f);

}

void CPlayer::Late_Update(_float fTimeDelta)
{
	// 충돌 판정
	On_Collision();

	//if (Find(m_))
	m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);



}

HRESULT CPlayer::Render()
{
	//m_pPlayer_Inven->Render();

	/*if (FAILED(m_pTextureCom->Bind_Resource(0)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	SetUp_RenderState();
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	Release_RenderState();

	m_pColliderCom->Render();*/
	return S_OK;
}

HRESULT CPlayer::On_Collision()
{
	
	_float3 temp = { 0.f,0.f, -5.f };
	_float3 fPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (nullptr == m_pColliderCom)
		return E_FAIL;

	// 안바뀌면 충돌 안일어남
	if (m_pColliderCom->Get_Other_Type() == CG_END)
		return S_OK;

	switch (m_pColliderCom -> Get_Other_Type())
	{
	case CG_MONSTER:

		break;

	case CG_MONSTER_PROJECTILE:
		temp += fPos;
		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, temp);
		break;

	default:
		break;
	}

	// 충돌 처리 하고 다시 type을 수정
	m_pColliderCom->Set_Other_Type(CG_END);

	return S_OK;
}

void CPlayer::Move(_float fTimeDelta)
{
	if (GetKeyState('W') & 0x8000)
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
	}
	if (GetKeyState('S') & 0x8000)
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
	}
	if (GetKeyState('A') & 0x8000)
	{
		m_pTransformCom->Turn(_float3(0.f, 1.f, 0.f), fTimeDelta * -1.f);
	}
	if (GetKeyState('D') & 0x8000)
	{
		m_pTransformCom->Turn(_float3(0.f, 1.f, 0.f), fTimeDelta);
	}
}

void CPlayer::Inven_Update(_float fTimeDelta)
{
	//m_bInven_Render_State = m_pPlayer_Inven->Update(fTimeDelta);


	
}

HRESULT CPlayer::SetUp_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 254);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	/*m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);*/

	return S_OK;
}

HRESULT CPlayer::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
//	m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	//m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	//m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	//m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}

HRESULT CPlayer::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_tObjDesc.stProtTextureTag,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		if (FAILED(__super::Add_Component(LEVEL_EDITOR, m_tObjDesc.stProtTextureTag,
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_tObjDesc.stBufferTag,
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 10.f, D3DXToRadian(90.f) };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	CCollider_Cube::COL_CUBE_DESC	ColliderDesc = {};
	ColliderDesc.eType = CG_PLAYER;
	ColliderDesc.pOwner = this;
	// 이걸로 콜라이더 크기 설정
	ColliderDesc.fScale = { 1.f, 1.f, 1.f };
	// 오브젝트와 상대적인 거리 설정
	ColliderDesc.fLocalPos = { 0.f, 0.f, 0.5f };

	/* For.Com_Collider_Sphere */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

CPlayer* CPlayer::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPlayer* pInstance = new CPlayer(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CPlayer::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pColliderCom);
}
