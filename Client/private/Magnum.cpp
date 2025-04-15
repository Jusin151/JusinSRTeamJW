#include "Magnum.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "Item_Manager.h"
#include "Image_Manager.h"
#include "Sound_Source.h"
#include "Light.h"



CMagnum::CMagnum(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CRanged_Weapon(pGraphic_Device)
{
}

CMagnum::CMagnum(const CMagnum& Prototype)
	: CRanged_Weapon(Prototype),
	m_Magnum_INFO{ Prototype.m_Magnum_INFO }
{
	
}

HRESULT CMagnum::Initialize_Prototype()
{
 

	return S_OK;
}

HRESULT CMagnum::Initialize(void* pArg)
{
	if (pArg != nullptr)
		m_Magnum_INFO = *reinterpret_cast<Weapon_DESC*>(pArg);
	else
		return E_FAIL;


	if (FAILED(Ready_Components()))
		return E_FAIL;

	// 일단 Level_GamePlay에서도 비슷한값 넣는중
	m_Weapon_INFO.WeaponID = WEAPON_ID::Magnum;
	m_Weapon_INFO.vPos = { 0.f,-170.f }; // 샷건 위치
	m_Weapon_INFO.vSize = { 749,420.f };// 샷건 크기 위에 두개는 일단 밖에서 하는중
	m_Weapon_INFO.Damage = 2000;                // 데미지
	m_Weapon_INFO.AttackSpeed = 1.2f;           // 공격 속도 (ex. 초당 발사 가능 횟수)

	m_iAp = 20;
	m_eType = CG_WEAPON;

	m_pTransformCom->Set_Scale(m_Weapon_INFO.vSize.x, m_Weapon_INFO.vSize.y, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_Weapon_INFO.vPos.x, m_Weapon_INFO.vPos.y, 0.f));

 	m_pTransformCom->Set_Scale(m_Magnum_INFO.vSize.x, m_Magnum_INFO.vSize.y, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_Magnum_INFO.vPos.x, m_Magnum_INFO.vPos.y, 0.0f));

	m_vInitialPos.x = m_Magnum_INFO.vPos.x;
	m_vInitialPos.y = m_Magnum_INFO.vPos.y;
	CItem_Manager::GetInstance()->Add_Weapon(L"Magnum", this);


	Ranged_INFO.CurrentAmmo = 50; //현총알
	Ranged_INFO.MaxAmmo = 50;    //매그넘 최대 50발
	m_fAnimationSpeed = 0.03f; // 애니메이션속도

	m_bIsActive = false;

	__super::Ready_Picking();
	

	return S_OK;
}


void CMagnum::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}
void CMagnum::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	m_fElapsedTime += fTimeDelta;

	switch (m_eState)
	{
	case State::Idle:
		m_iCurrentFrame = 0;
		break;

	case State::Firing:
	if (m_fElapsedTime >= m_fAnimationSpeed)
	{
		m_fElapsedTime = 0.f;

		if (!m_bHasFired && m_iCurrentFrame == 1)
		{
				m_bHasFired = true;
		}
		if (m_iCurrentFrame < 4)
			m_iCurrentFrame++;
		else
		{
			m_eState = State::Wait;
			m_iCurrentFrame = 0; 
		}
	}
	break;
	case State::Wait:
	{
		if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000))
		{
			m_eState = State::Idle;
			m_iCurrentFrame = 0;
		}
	}
	break;
	}
	m_pSoundCom->Update(fTimeDelta);
}


void CMagnum::Attack(_float fTimeDelta)
{
	__super::Attack(fTimeDelta);
}

void CMagnum::Attack_WeaponSpecific(_float fTimeDelta)
{
	if (m_eState == State::Idle && Ranged_INFO.CurrentAmmo > 0)
	{
		m_eState = State::Firing;
		m_iCurrentFrame = 0;
		m_fElapsedTime = 0.f;
		m_bHasFired = false;
		__super::Picking_Object(1, m_Weapon_INFO.Damage);
		Ranged_INFO.CurrentAmmo--; 
		Notify_Bullet();
		m_pSoundCom->Play_Event(L"event:/Weapons/Range/Magnum/magnum_shot")->SetVolume(0.5f);
	}
	else if (m_eState == State::Idle && Ranged_INFO.CurrentAmmo == 0)
	{
		m_eState = State::Wait;
		m_pSoundCom->Play_Event(L"event:/Weapons/Range/Gun/gun_empty")->SetVolume(0.5f);
	}
}




void CMagnum::Late_Update(_float fTimeDelta) //요거는 나중에 LEVEL_GAMLPLAY 자리에 겟커렌트 레벨 만들면 댈듯
{
	
	__super::Late_Update(fTimeDelta);
	if (State::Firing == m_eState)
	{
		CGameObject* pPlayer = m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Player"));
		if (nullptr == pPlayer)
			return;
		CTransform* pTransform = static_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")));
		if (nullptr == pTransform)
			return;
		m_pGameInstance->Add_Light(m_pLightCom);
		_float3 pos = pTransform->Get_State(CTransform::STATE_POSITION);
		pos += pTransform->Get_State(CTransform::STATE_LOOK) * 1.5f;
		m_pLightCom->Set_Position(pos);
		m_pLightCom->DecreaseIntensity(m_iCurrentFrame);
	}
}

HRESULT CMagnum::Render()
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

	_float2 ScaleFactor = { 1.0f, 1.0f };
	_float2 Offset = { 0.f, 0.f };
	m_pShaderCom->Set_UVScaleFactor(&ScaleFactor);
	m_pShaderCom->Set_UVOffsetFactor(&Offset);

	if (FAILED(m_pShaderCom->Bind_Texture(m_pTextureCom, m_iCurrentFrame)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Transform()))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Material(m_pMaterialCom)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Lights()))
		return E_FAIL;

	m_pShaderCom->Begin(2);
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	m_pShaderCom->End();
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &matOldView);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &matOldProj);

	return S_OK;
}

HRESULT CMagnum::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Magnum"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBufferm"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &tDesc)))
		return E_FAIL;


	CLight::LIGHT_INIT lDesc = { L"../../Resources/Lights/GunLight.json" };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Light_Point"),
		TEXT("Com_Light"), reinterpret_cast<CComponent**>(&m_pLightCom), &lDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound_Source"),
		TEXT("Com_Sound_Source"), reinterpret_cast<CComponent**>(&m_pSoundCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_BaseShader"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Material */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Material"),
		TEXT("Com_Material"), reinterpret_cast<CComponent**>(&m_pMaterialCom))))
		return E_FAIL;

	return S_OK;
}

CMagnum* CMagnum::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMagnum* pInstance = new CMagnum(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{

		MSG_BOX("매그넘 UI 원본 생성 실패 ");

		Safe_Release(pInstance);
	}


	return pInstance;
}

CGameObject* CMagnum::Clone(void* pArg)
{
	CMagnum* pInstace = new CMagnum(*this);

	if (FAILED(pInstace->Initialize(pArg)))
	{

		MSG_BOX("매그넘 UI 복제 실패");

		Safe_Release(pInstace);
	}

	return pInstace;
}

void CMagnum::Free()
{
	__super::Free();
	Safe_Release(m_pLightCom);
	Safe_Release(m_pSoundCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pPickingSys);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pMaterialCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
}
