
#include "PickingSys.h"
#include "GameInstance.h"
#include "Collider_Sphere.h"
#include "Collider_Cube.h"
#include "Camera_FirstPerson.h"
#include "Melee_Weapon.h"
#include "UI_Manager.h"
#include "UI_Player_Icon.h"
#include "Ranged_Weapon.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CCollisionObject{ pGraphic_Device }
{
}

CPlayer::CPlayer(const CPlayer& Prototype)
	: CCollisionObject(Prototype)
{
}

HRESULT CPlayer::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	
	if (!pArg)
	{
		m_tObjDesc.iLevel = 0;
		m_tObjDesc.stBufferTag = TEXT("Prototype_Component_VIBuffer_Cube");
		m_tObjDesc.stProtTextureTag = TEXT("Prototype_Component_Texture_Player");
		m_tObjDesc.iProtoLevel = 0;
	}
	else
	{
		OBJECT_DESC* pDesc = static_cast<OBJECT_DESC*>(pArg);
		m_tObjDesc = *pDesc;
	}
	if (FAILED(Ready_Components()))
		return E_FAIL;

	// Antarctic1_Test pos
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(-10.f, 0.7f, 8.3f));

	// Gameplay start pos
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(-5.f, 0.5f, -1.f));
	m_pTransformCom->Set_Scale(1.f, 1.8f, 1.f);
	//m_pColliderCom->Set_Radius(5.f);
	//m_pColliderCom->Set_Scale(_float3(1.f, 1.f, 1.f));


	if (FAILED(Ready_Player_SetUP()))
		return E_FAIL;


	m_fSpeed = 0.7f;

	CPickingSys::Get_Instance()->Set_Player(this);

	return S_OK;
}


void CPlayer::Priority_Update(_float fTimeDelta)
{
	

}


void CPlayer::Update(_float fTimeDelta)
{
	m_vCurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);


	Input_Key(fTimeDelta);

	//m_vNextPos =  m_pTransformCom->Get_State(CTransform::STATE_POSITION);


	m_vDir = m_vNextPos - m_vCurPos;
	m_fLength = m_vDir.Length();
	m_vDir.Normalize();

	m_pColliderCom->Update_Collider(TEXT("Com_Transform"),m_pTransformCom->Compute_Scaled());
	m_pGameInstance->Add_Collider(CG_PLAYER, m_pColliderCom);


	/////////트리거용 
	if (GetAsyncKeyState('0') & 0x8000)
	{
		Add_Ammo(L"Magnum_Ammo",10);

	}
	static _bool m_WeaponTrigger = { true }; //임시
 
	if (m_WeaponTrigger) 
	{
		m_pPlayer_Inven->Add_Weapon(L"Claymore", 1);
		m_pPlayer_Inven->Add_Weapon(L"Axe", 2);
	//	m_pPlayer_Inven->Add_Weapon(L"ShotGun", 3);
		m_pPlayer_Inven->Add_Weapon(L"Magnum", 4);
		m_pPlayer_Inven->Add_Weapon(L"Staff", 5);
		m_pPlayer_Inven->Add_Weapon(L"Minigun", 6);
		m_pPlayer_Inven->Add_Weapon(L"Harvester", 7);
		//m_pPlayer_Inven->Add_Weapon(L"Sonic", 8);
		
		m_WeaponTrigger = false; 
	}

	if (GetAsyncKeyState('Q') & 0x8000)//임시
	{	
		Add_Exp(1);
	}


}
void CPlayer::Late_Update(_float fTimeDelta)
{

	m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
	Attack(fTimeDelta);//좌클
}


void CPlayer::Input_ItemtoInven()
{


}

void CPlayer::Equip(_float fTimeDelta)
{
	if (m_eWeaponState != WEAPON_STATE::IDLE) return;
	m_eWeaponState = WEAPON_STATE::CHANGE;
	for (int i = 1; i <= 8; ++i)
	{
		if (GetAsyncKeyState('0' + i) & 0x8000)
		{
			if (m_pPlayer_Weapon)
			{
				m_pPlayer_Weapon->SetActive(false);
			}
			m_pPlayer_Weapon = m_pPlayer_Inven->Equip(i);
			//m_pPlayer_Weapon->SetActive(true);
			break;
		}
	}


	if (m_pPlayer_Weapon)
	{
		if (auto pObserver = dynamic_cast<CObserver*>(CUI_Manager::GetInstance()->GetUI(L"Bullet_Bar")))
		{
			if (auto pRanged = dynamic_cast<CWeapon_Base*>(m_pPlayer_Weapon))
			{
				pRanged->Add_Observer(pObserver);
			}
		}
	}
	m_eWeaponState = WEAPON_STATE::IDLE;

}
HRESULT CPlayer::Render()
{

	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("플레이어 위치 X:") + to_wstring(m_pTransformCom->Get_WorldMat()._41),
		_float2(-300.f, -207.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));

	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("플레이어 위치 Y:") + to_wstring(m_pTransformCom->Get_WorldMat()._42),
		_float2(-100.f, -207.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));

	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("플레이어 위치 Z:") + to_wstring(m_pTransformCom->Get_WorldMat()._43),
		_float2(100.f, -207.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));

	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("플레이어 LOOK X:") + to_wstring(m_pTransformCom->Get_State(CTransform::STATE_LOOK).x),
		_float2(-300.f, -277.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));
	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("플레이어 LOOK Y:") + to_wstring(m_pTransformCom->Get_State(CTransform::STATE_LOOK).y),
		_float2(-300.f, -257.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));
	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("플레이어 LOOK Z:") + to_wstring(m_pTransformCom->Get_State(CTransform::STATE_LOOK).z),
		_float2(-300.f, -237.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));

	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("플레이어 Right X:") + to_wstring(m_pTransformCom->Get_State(CTransform::STATE_RIGHT).x),
		_float2(-100.f, -277.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));
	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("플레이어 Right Y:") + to_wstring(m_pTransformCom->Get_State(CTransform::STATE_RIGHT).y),
		_float2(-100.f, -257.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));
	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("플레이어 Right Z:") + to_wstring(m_pTransformCom->Get_State(CTransform::STATE_RIGHT).z),
		_float2(-100.f, -237.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));

	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("플레이어 Up X:") + to_wstring(m_pTransformCom->Get_State(CTransform::STATE_UP).x),
		_float2(100.f, -277.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));
	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("플레이어 Up Y:") + to_wstring(m_pTransformCom->Get_State(CTransform::STATE_UP).y),
		_float2(100.f, -257.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));
	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("플레이어 Up Z:") + to_wstring(m_pTransformCom->Get_State(CTransform::STATE_UP).z),
		_float2(100.f, -237.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));

	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT(" 체력:") + to_wstring(m_iHp),
		_float2(-600.f, -250.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));

	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("마나:") + to_wstring(m_iPlayerMP.second),
		_float2(-600.f, -230.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));

	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("근력:") + to_wstring(m_iStr),
		_float2(-600.f, -210.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));

	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("정신력:") + to_wstring(m_iSprit),
		_float2(-600.f, -190.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));

	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("용량:") + to_wstring(m_iCapacity),
		_float2(-600.f, -170.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));

	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("스킬포인트:") + to_wstring(m_iSkillpoint),
		_float2(400.f, -50.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));

	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("스탯포인트:") + to_wstring(m_iStatpoint),
		_float2(400.f, -30.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));


	return S_OK;
}

HRESULT CPlayer::On_Collision(CCollisionObject* other)
{
	if (nullptr == m_pColliderCom)
		return E_FAIL;

	if (nullptr == other)
		return S_OK;
	if (other->Get_Type() == CG_END)
		return S_OK;

	_float3 vMtv = m_pColliderCom->Get_MTV();
	_float3 vMove = { vMtv.x, 0.f, vMtv.z };


	switch (other->Get_Type())
	{
	case CG_MONSTER:
		m_vNextPos += vMove * 0.3f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vNextPos);
		break;

	case CG_MONSTER_PROJECTILE_CUBE:
		break;

	case CG_STRUCTURE_WALL:
		m_vNextPos += vMove;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vNextPos);

		break;
	case CG_DOOR:
		m_vNextPos += vMove;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vNextPos);

		break;
	default:
		break;
	}




	return S_OK;
}

void CPlayer::Move(_float fTimeDelta)
{

	_float moveSpeed = 0.51f;

	_float3 moveDir = { 0.f, 0.f, 0.f }; // 이동 방향 초기화

	if (GetAsyncKeyState('W') & 0x8000) {
		moveDir += m_pTransformCom->Get_State(CTransform::STATE_LOOK); // 앞 방향
	}

	if (GetAsyncKeyState('S') & 0x8000) {
		moveDir -= m_pTransformCom->Get_State(CTransform::STATE_LOOK); // 뒤 방향
	}

	if (GetAsyncKeyState('A') & 0x8000) {
		moveDir -= m_pTransformCom->Get_State(CTransform::STATE_RIGHT); // 왼쪽 방향
	}

	if (GetAsyncKeyState('D') & 0x8000) {
		moveDir += m_pTransformCom->Get_State(CTransform::STATE_RIGHT); // 오른쪽 방향
	}




	if (moveDir.LengthSq() > 0) 
	{
		moveDir.Normalize(); // 방향 정규화
		_float3 fPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		moveDir.y = 0.f;
		fPos += moveDir * fTimeDelta * m_fSpeed * 10;
		m_vNextPos = fPos;	
		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, fPos);
	}
	else
	{
		m_vNextPos = m_vCurPos;
	}
	


}

void CPlayer::Attack(_float fTimeDelta)
{
	if (m_pPlayer_Weapon == nullptr||m_eWeaponState == WEAPON_STATE::CHANGE)
		return;

	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		m_eWeaponState = WEAPON_STATE::FIRE;
		m_pPlayer_Weapon->Attack(fTimeDelta);
	}
	else
	{
		m_eWeaponState = WEAPON_STATE::IDLE;
	}
}

void CPlayer::Input_Key(_float fTimeDelta)
{
	if (m_bTimeControl)
		return;
	Equip(fTimeDelta); //인
	
	Move(fTimeDelta); // 플레이어 이동
}

void CPlayer::Set_Hp(_int iHp)
{
	if (iHp < m_iHp)
	{
		// 체력이 달면
		m_iHp = max(0, iHp);
	}
	else
	{
		// 체력 늘면
		m_iHp = min(iHp, (_int)m_iPlayerHP.second);
	}
	Notify(m_iHp, L"HP");
}

void CPlayer::Add_Ammo(const _wstring& stWeaponName,_int iAmmo)
{
	
	_wstring weaponName = stWeaponName.substr(0, stWeaponName.find(L"_"));

	if (m_pPlayer_Inven)
	{
		if (m_pPlayer_Inven->Exist_item(weaponName))
		{
			auto pWeapon = dynamic_cast<CRanged_Weapon*>(m_pPlayer_Inven->Get_Weapon(weaponName));

			if (pWeapon) 
			{
				pWeapon->Add_Ammo(iAmmo);
			}
		}
	}


	// 이벤트 텍스트 출력
	if (auto pUI_Event = dynamic_cast<CUI_Event*>(CUI_Manager::GetInstance()->GetUI(L"UI_Event")))
	{
		pUI_Event->ShowEventText(iAmmo, L"Ammo");
	}


	//if (!m_pPlayer_Weapon)
	//	return;

	//if (auto pRanged = dynamic_cast<CRanged_Weapon*>(m_pPlayer_Weapon))
	//{
	//	pRanged->Add_Ammo(iAmmo);
	//}
}

void CPlayer::Add_Weapon(const _wstring& stWeaponTag)
{
	if (!m_pPlayer_Inven) return;

	if (m_pPlayer_Inven->Exist_item(stWeaponTag))
	{
		return;
	}

	if (stWeaponTag == L"ShotGun")
	{

		m_pPlayer_Inven->Add_Weapon(stWeaponTag,3);

	}
}

_bool CPlayer::Has_Item(const _wstring& stItemTag)
{
	if (!m_pPlayer_Inven) return false;

	return m_pPlayer_Inven->Exist_item(stItemTag);
}

HRESULT CPlayer::Add_Item(const _wstring& stItemTag)
{
	if (!m_pPlayer_Inven) return E_FAIL;

	m_pPlayer_Inven->Add_Item(stItemTag);

	return S_OK;
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
	/*if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_tObjDesc.stProtTextureTag,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		if (FAILED(__super::Add_Component(LEVEL_EDITOR, m_tObjDesc.stProtTextureTag,
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}*/

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_tObjDesc.stBufferTag,
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 20.f, D3DXToRadian(90.f) };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	CCollider_Cube::COL_CUBE_DESC	ColliderDesc = {};
	ColliderDesc.pOwner = this;
	// 이걸로 콜라이더 크기 설정
	ColliderDesc.fScale = { 1.f, 1.f, 1.f };
	// 오브젝트와 상대적인 거리 설정
	ColliderDesc.fLocalPos = { 0.f, 0.f, 0.f };

	/* For.Com_Collider_Sphere */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Ready_Player_SetUP()
{
	// 이거 나중에 수정 필요할듯?

	m_iPlayerHP = { 100,100 }; // 현재/최대  이하 동문
	m_iPlayerMP = { 50, 50 };
	m_iPlayerEXP = { 0 , 100 };

	m_eType = CG_PLAYER;

	m_iHp = m_iPlayerHP.first;


	m_pPlayer_Inven = static_cast<CInventory*>(m_pGameInstance->Find_Object
	(LEVEL_STATIC, TEXT("Layer_Inven")));

	if (!m_pPlayer_Inven)
	{
 		m_pPlayer_Inven = static_cast<CInventory*>(m_pGameInstance->Find_Object
		(LEVEL_STATIC, TEXT("Layer_Inven")));
		if (!m_pPlayer_Inven)
		{
			m_pPlayer_Inven = static_cast<CInventory*>(m_pGameInstance->Find_Object
			(LEVEL_STATIC, TEXT("Layer_Inven")));
				if(!m_pPlayer_Inven)
				{
					return E_FAIL;
				}
		}		
	}

	if (auto pHpUI = dynamic_cast<CObserver*>(CUI_Manager::GetInstance()->GetUI(L"Hp_Bar")))
		Add_Observer(pHpUI);

	if (auto pMpUI = dynamic_cast<CObserver*>(CUI_Manager::GetInstance()->GetUI(L"Mp_Bar")))
		Add_Observer(pMpUI);

	if (auto pPlayer_Icon = dynamic_cast<CObserver*>(CUI_Manager::GetInstance()->GetUI(L"Player_Icon")))
		Add_Observer(pPlayer_Icon);

	if (auto pPlayer_Icon = dynamic_cast<CObserver*>(CUI_Manager::GetInstance()->GetUI(L"Exp_Bar")))
		Add_Observer(pPlayer_Icon);

	//if (auto m_pHub_PointShop = dynamic_cast<CObserver*>(m_pGameInstance->Find_Object(LEVEL_HUB, TEXT("Layer_Point_Shop"))))
	//	Add_Observer(m_pHub_PointShop); 

	//if (auto m_pHub_WeaponShop = dynamic_cast<CObserver*>(m_pGameInstance->Find_Object(LEVEL_HUB, TEXT("Layer_Weapon_Shop"))))
	//	Add_Observer(m_pHub_WeaponShop);

	//if (auto m_pHub_SpellShop = dynamic_cast<CObserver*>(m_pGameInstance->Find_Object(LEVEL_HUB, TEXT("Layer_Spell_Shop"))))
	//	Add_Observer(m_pHub_SpellShop);


	CUI_Manager::GetInstance()->Init_HP_UI(m_iHp, m_iPlayerHP.second);

	CUI_Manager::GetInstance()->Init_Exp_UI(m_iPlayerEXP.first, m_iPlayerEXP.second);

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

