#include "Melee_Weapon.h"
#include "GameInstance.h"
#include "Collider_Cube.h"

CMelee_Weapon::CMelee_Weapon(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CWeapon_Base{pGraphic_Device}
{
}

CMelee_Weapon::CMelee_Weapon(const CMelee_Weapon& Prototype)
	: CWeapon_Base(Prototype)
{
}

HRESULT CMelee_Weapon::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CMelee_Weapon::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	Safe_AddRef(m_pColliderCom);
	Safe_AddRef(m_pColTransformCom);

	m_eType = CG_WEAPON;

	return S_OK;
}

void CMelee_Weapon::Priority_Update(_float fTimeDelta)
{
}

void CMelee_Weapon::Update(_float fTimeDelta)
{
	CGameObject* pPlayer =m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_Player"));

	if (nullptr == pPlayer)
		return;

	CTransform* pTransform = static_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")));

	if (nullptr == pTransform)
		return;

	m_pColTransformCom->Set_State(CTransform::STATE_RIGHT, pTransform->Get_State(CTransform::STATE_RIGHT));
	m_pColTransformCom->Set_State(CTransform::STATE_UP, pTransform->Get_State(CTransform::STATE_UP));
	m_pColTransformCom->Set_State(CTransform::STATE_LOOK, pTransform->Get_State(CTransform::STATE_LOOK));
	m_pColTransformCom->Set_State(CTransform::STATE_POSITION, pTransform->Get_State(CTransform::STATE_POSITION));



	m_pColliderCom->Update_Collider(TEXT("Com_Transform"), m_pColliderCom->Get_Scale());



	if(m_bIsAnimating)
		m_pGameInstance->Add_Collider(CG_WEAPON, m_pColliderCom);
}

void CMelee_Weapon::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CMelee_Weapon::Render()
{
	D3DXMATRIX matOldView, matOldProj;
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &matOldView);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &matOldProj);

	D3DXMATRIX matView;
	D3DXMatrixIdentity(&matView);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &matView);

	D3DXMATRIX matProj;
	D3DXMatrixOrthoLH(&matProj, g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &matProj);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);


	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_Resource(m_iCurrentFrame)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &matOldView);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &matOldProj);

	return S_OK;
}

HRESULT CMelee_Weapon::Ready_Components()
{
	/* For.Com_Collider */
	CCollider_Cube::COL_CUBE_DESC	ColliderDesc = {};
	ColliderDesc.pOwner = this;
	// 이걸로 콜라이더 크기 설정
	ColliderDesc.fScale = { 1.f, 1.f, 1.f };
	// 오브젝트와 상대적인 거리 설정, 
	ColliderDesc.fLocalPos = { 0.f, 0.f, 1.f };


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pColTransformCom), &tDesc)))
		return E_FAIL;

	return S_OK;
}

void CMelee_Weapon::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pColTransformCom);
}

CGameObject* CMelee_Weapon::Clone(void* pArg)
{
	return nullptr;
}


void CMelee_Weapon::Attack(_float fTimeDelta)
{
	if (!m_bIsAnimating && (GetAsyncKeyState(VK_LBUTTON) & 0x8000))
	{
		m_bIsAnimating = true;
		m_iCurrentFrame = 0;
		m_fElapsedTime = 0.0f;
	}

	if (m_bIsAnimating)
	{
		m_fElapsedTime += fTimeDelta;

		if (m_fElapsedTime >= 0.02f)
		{
			m_fElapsedTime = 0.0f;

			if (m_iCurrentFrame < 13)
			{
				m_iCurrentFrame++;
			}
			else
			{
				m_bIsAnimating = false;
			}
		}
	}
}




