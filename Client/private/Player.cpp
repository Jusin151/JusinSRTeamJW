
#include "PickingSys.h"
#include "GameInstance.h"
#include "Collider_Sphere.h"
#include "Collider_Cube.h"
#include "Camera_FirstPerson.h"
#include "Melee_Weapon.h"
#include "UI_Manager.h"
#include "UI_Player_Icon.h"

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

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(-10.f, 1.0f, 8.f));
	m_vOldPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->Set_Scale(0.5f,0.5f,0.5f);
	//m_pTransformCom->Rotation(_float3(0.f, 0.8f, 0.f), D3DXToRadian(90.f));
	//m_pColliderCom->Set_Radius(5.f);
	//m_pColliderCom->Set_Scale(_float3(1.f, 1.f, 1.f));


	if (FAILED(Ready_Player_SetUP()))
		return E_FAIL;
	



	CPickingSys::Get_Instance()->Set_Player(this);

	return S_OK;
}


void CPlayer::Priority_Update(_float fTimeDelta)
{


}
//CTransform* pTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Com_Transform")));

	//if (pTransform)
	//{
	//	pTransform->Go_Straight(fTimeDelta);
	//}

void CPlayer::Update(_float fTimeDelta)
{
	m_pColliderCom->Update_Collider(TEXT("Com_Transform"), m_pColliderCom->Get_Scale());
	m_pGameInstance->Add_Collider(CG_PLAYER, m_pColliderCom);

	Input_Key(fTimeDelta);



/////////트리거용 
	static _bool m_basd = { true };

	if (m_basd)
	{
		m_pPlayer_Inven->Add_Weapon(L"Claymore", 1);
		m_pPlayer_Inven->Add_Weapon(L"Axe", 2);
		m_pPlayer_Inven->Add_Weapon(L"ShotGun", 3);
		m_pPlayer_Inven->Add_Weapon(L"Magnum", 4);
		m_pPlayer_Inven->Add_Weapon(L"Staff", 5);
		m_pPlayer_Inven->Add_Weapon(L"Minigun", 6);
		m_pPlayer_Inven->Add_Weapon(L"Harvester", 7);
		m_pPlayer_Inven->Add_Weapon(L"Sonic", 8);

		m_basd = false;
	}

}
void CPlayer::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}


void CPlayer::Input_ItemtoInven()
{


}

void CPlayer::Equip(_float fTimeDelta)
{
	if (GetAsyncKeyState('1') & 0x8000)
	{
		m_pPlayer_Weapon = m_pPlayer_Inven->Equip(1);
	}
	if (GetAsyncKeyState('2') & 0x8000)
	{
		m_pPlayer_Weapon = m_pPlayer_Inven->Equip(2);
	}
	if (GetAsyncKeyState('3') & 0x8000)
	{
		m_pPlayer_Weapon = m_pPlayer_Inven->Equip(3);
	}
	if (GetAsyncKeyState('4') & 0x8000)
	{
		m_pPlayer_Weapon = m_pPlayer_Inven->Equip(4);
	}
	if (GetAsyncKeyState('5') & 0x8000)
	{
		m_pPlayer_Weapon = m_pPlayer_Inven->Equip(5);
	}
	if (GetAsyncKeyState('6') & 0x8000)
	{
		m_pPlayer_Weapon = m_pPlayer_Inven->Equip(6);
	}
	if (GetAsyncKeyState('7') & 0x8000)
	{
		m_pPlayer_Weapon = m_pPlayer_Inven->Equip(7);
	}
	if (GetAsyncKeyState('8') & 0x8000)
	{
		m_pPlayer_Weapon = m_pPlayer_Inven->Equip(8);
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


}
HRESULT CPlayer::Render()
{

	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("플레이어 위치 X:") + to_wstring(m_pTransformCom->Get_WorldMat()._41),
		_float2(-300.f, -207.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));

	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("플레이어 위치 Y:") + to_wstring(m_pTransformCom->Get_WorldMat()._42),
		_float2(-100.f, -207.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));

	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("플레이어 위치 Z:") + to_wstring(m_pTransformCom->Get_WorldMat()._43),
		_float2(100.f, -207.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));

	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT(" 체력:") + to_wstring(m_iHp),
		_float2(-600.f, -250.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));

	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("마나:") + to_wstring(m_iPlayerMP.second),
		_float2(-600.f, -190.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));

	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT(" 경험치:") + to_wstring(m_iPlayerEXP.second),
		_float2(-600.f, -230.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));

	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT(" 총알:") + to_wstring(iBullet),
		_float2(-600.f, -210.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));

	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("근력:") + to_wstring(iStr),
		_float2(-600.f, -170.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));

	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("생명력:") + to_wstring(iLife),
		_float2(-600.f, -150.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));

	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("정신력:") + to_wstring(iSprit),
		_float2(-600.f, -130.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));

	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("용량:") + to_wstring(iCapacity),
		_float2(-600.f, -110.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));


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

HRESULT CPlayer::On_Collision(CCollisionObject* other)
{
	if (nullptr == m_pColliderCom)
		return E_FAIL;

	if (nullptr == other)
		return S_OK;
	if (other->Get_Type() == CG_END)
		return S_OK;

	_float3 fPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float3 vMtv = m_pColliderCom->Get_MTV();
	_float3 vMove = { vMtv.x, 0.f, vMtv.z };
	_float3 direction = vMove.GetNormalized();

	_float3 otherPos = static_cast<CTransform*>(other->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);

	_float3 dirOthertoOldPos = fPos - otherPos;
	_float3 dirOthertoNewPos = fPos + vMove - otherPos;
	_float fDepth = vMove.Length();

	switch (other->Get_Type())
	{
	case CG_MONSTER:
		// gameObject.Get_Tag() == "Bullet"
		// if(CMonster* pMonster = dynamic_cast<CMonster*>(gameObject))
		// {
		// _int iDamge = pMonster->Get_Damge();
		// Set_Hp()llll
		// Update_UI()
		// 
		// 
		// 
		//

		break;

	case CG_MONSTER_PROJECTILE_CUBE:
		break;

	case CG_STRUCTURE_WALL:

		if (dirOthertoOldPos.Dot(direction) < 0)
			fPos = m_vOldPos;
		else
			fPos += vMove;
		

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, fPos);
		
		break;
	default:
		break;
	}


	m_vOldPos = fPos;


	return S_OK;
}

void CPlayer::Move(_float fTimeDelta)
{

	_float moveSpeed = 0.25f;
	_float3 moveDir = { 0.f, 0.f, 0.f }; // 이동 방향 초기화

	if (GetKeyState('W') & 0x8000) {
		moveDir += m_pTransformCom->Get_State(CTransform::STATE_LOOK); // 앞 방향
	}

	if (GetKeyState('S') & 0x8000) {
		moveDir -= m_pTransformCom->Get_State(CTransform::STATE_LOOK); // 뒤 방향
	}

	if (GetKeyState('A') & 0x8000) {
		moveDir -= m_pTransformCom->Get_State(CTransform::STATE_RIGHT); // 왼쪽 방향
	}

	if (GetKeyState('D') & 0x8000) {
		moveDir += m_pTransformCom->Get_State(CTransform::STATE_RIGHT); // 오른쪽 방향
	}


	if (moveDir.LengthSq() > 0) {
		moveDir.Normalize(); // 방향 정규화
		m_vOldPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float3 fPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		fPos += moveDir * fTimeDelta * moveSpeed * 10;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, fPos);
	}


	POINT ptMouse{};
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	LONG LDistX = abs(ptMouse.x - m_lMiddlePointX);

	if (ptMouse.x - m_lMiddlePointX > 0)
	{
		if (LDistX > 80)
			m_pTransformCom->Turn(_float3(0.f, 1.f, 0.f), fTimeDelta * LDistX * 0.005f);
	}
	else if (ptMouse.x - m_lMiddlePointX < 0)
	{
		if (LDistX > 80)
			m_pTransformCom->Turn(_float3(0.f, 1.f, 0.f), -fTimeDelta * LDistX * 0.005f);
	}

}

void CPlayer::Attack(_float fTimeDelta)
{
	if (m_pPlayer_Weapon == nullptr)
		return;

	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{	
		m_pPlayer_Weapon->Attack(fTimeDelta);
	}
   
}

void CPlayer::Input_Key(_float fTimeDelta)
{
	Equip(fTimeDelta); //인벤

	Attack(fTimeDelta);//좌클

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

inline void CPlayer::Add_Ammo(_int iAmmo)
{
		//if (!m_pWeapon) return;
		//if (CRanged_Weapon* pRangeWeapon = dynamic_cast<CRanged_Weapon*>(m_pWeapon))
		//{
		//	pRangeWeapon->Add_Ammo(iAmmo);
		//}
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
	ColliderDesc.pOwner = this;
	// 이걸로 콜라이더 크기 설정
	ColliderDesc.fScale = { 0.5f,0.5f,0.5f };
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

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Inven"),
		LEVEL_GAMEPLAY, TEXT("Layer_Inven")))) 
		return E_FAIL; 

	m_pPlayer_Inven = static_cast<CInventory*>(m_pGameInstance->Find_Object 
	(LEVEL_GAMEPLAY, TEXT("Layer_Inven")));

	if (!m_pPlayer_Inven)
		return E_FAIL;



	if (auto pHpUI = dynamic_cast<CObserver*>(CUI_Manager::GetInstance()->GetUI(L"Hp_Bar"))) 
		Add_Observer(pHpUI);

	if (auto pMpUI = dynamic_cast<CObserver*>(CUI_Manager::GetInstance()->GetUI(L"Mp_Bar")))
		Add_Observer(pMpUI);

	if (auto pPlayer_Icon = dynamic_cast<CObserver*>(CUI_Manager::GetInstance()->GetUI(L"Player_Icon")))
		Add_Observer(pPlayer_Icon);

	CUI_Manager::GetInstance()->Init_HP_UI(m_iHp, m_iPlayerHP.second); 

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

