#include "Harvester.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "Item_Manager.h"
#include "Image_Manager.h"

CHarvester::CHarvester(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CRanged_Weapon(pGraphic_Device)
{
}

CHarvester::CHarvester(const CHarvester& Prototype)
	: CRanged_Weapon(Prototype),
	m_Staff_INFO{ Prototype.m_Staff_INFO }
{
}

HRESULT CHarvester::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHarvester::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (pArg != nullptr)
		m_Staff_INFO = *reinterpret_cast<Weapon_DESC*>(pArg);
	else
		return E_FAIL;

	// 일단 Level_GamePlay에서도 비슷한값 넣는중
	m_Weapon_INFO.WeaponID = WEAPON_ID::ShotGun;
	//m_Weapon_INFO.vPos = { 0.f,-170.f }; // 하베스터 위치
	//m_Weapon_INFO.vSize = { 749,420.f };// 하베스터 크기 위에 두개는 일단 밖에서 하는중
	m_Weapon_INFO.Damage = 50;                // 데미지
	m_Weapon_INFO.AttackSpeed = 1.2f;        // 공격 속도 (ex. 초당 발사 가능 횟수)

	Ranged_INFO.CurrentAmmo = 30; 
	Ranged_INFO.MaxAmmo = 30;
	m_fAnimationSpeed = 0.03f;


	m_pTransformCom->Set_Scale(m_Staff_INFO.vSize.x, m_Staff_INFO.vSize.y, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_Staff_INFO.vPos.x, m_Staff_INFO.vPos.y, 0.f));

	m_vInitialPos = m_Staff_INFO.vPos;

	// 초기화 시 설정ㅠㅜ
	m_TextureRanges["Idle"] = { 0,0 };
	m_TextureRanges["Firing"] = { 1, 4 };
	m_TextureRanges["Reloading"] = { 5, 26 };
	

	CItem_Manager::GetInstance()->Add_Weapon(L"Harvester", this);


	__super::Ready_Picking();

	return S_OK;
}


void CHarvester::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CHarvester::Update(_float fTimeDelta)
{
	m_fElapsedTime += fTimeDelta;

	switch (m_eState)
	{
	case State::Idle:
		m_iCurrentFrame = m_TextureRanges["Idle"].first;
		break;

	case State::Firing:
		if (m_fElapsedTime >= 0.02f)
		{

			m_fElapsedTime = 0.0f;
			if (m_iCurrentFrame < m_TextureRanges["Firing"].second)
			{
				m_iCurrentFrame++;
	
			}
			else
			{
				m_eState = State::Reloading;
				m_iCurrentFrame = m_TextureRanges["Reloading"].first;
			}
		}
		break;

	case State::Reloading:
		if (m_fElapsedTime >= 0.06f)
		{
			m_fElapsedTime = 0.0f;
			if (m_iCurrentFrame < m_TextureRanges["Reloading"].second)
			{
				m_iCurrentFrame++;
			}
			else
			{
				m_eState = State::Idle;
				m_iCurrentFrame = m_TextureRanges["Idle"].first;
			}
		}
		break;
	}

}

void CHarvester::Late_Update(_float fTimeDelta)
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
		m_pLightCom->Set_Position(pTransform->Get_State(CTransform::STATE_POSITION));
		m_pLightCom->DecreaseIntensity(m_iCurrentFrame);
	}
}

void CHarvester::Attack_WeaponSpecific(_float fTimeDelta)
{
	if (Ranged_INFO.CurrentAmmo == 0) return;
	if (m_eState == State::Idle)
	{
		m_eState = State::Firing;
		m_iCurrentFrame = m_TextureRanges["Firing"].first;
		m_fElapsedTime = 0.0f;
		__super::Picking_Object(5, m_Weapon_INFO.Damage);
		Ranged_INFO.CurrentAmmo = max(0,Ranged_INFO.CurrentAmmo-4);
		Notify_Bullet();
	}
}

void CHarvester::Attack(_float fTimeDelta)
{
	__super::Attack(fTimeDelta);

	
}

HRESULT CHarvester::Render()
{


	//if (m_bWall)
	//	m_pGameInstance->Render_Font(L"MainFont", L"벽 명중!!!", _float2(-200.f, -205.0f), _float3(1.f, 1.f, 0.0f));
	//if (m_bMonster)
	//	m_pGameInstance->Render_Font(L"MainFont", L"몬스터 명중!!!", _float2(200.f, -205.0f), _float3(1.f, 1.f, 0.0f));


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

	switch (m_eState)
	{
	case State::Idle:
	case State::Firing:
		if (FAILED(m_pTextureCom->Bind_Resource(m_iCurrentFrame)))
			return E_FAIL;
		break;

	case State::Reloading:
		if (FAILED(m_pTextureCom->Bind_Resource(m_iCurrentFrame)))
			return E_FAIL;
		break;
	}

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

HRESULT CHarvester::On_Collision()
{



	return S_OK;
}

HRESULT CHarvester::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Harvester"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
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

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Material"),
		TEXT("Com_Material"), reinterpret_cast<CComponent**>(&m_pMaterialCom))))
		return E_FAIL;

	return S_OK;
}

CHarvester* CHarvester::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CHarvester* pInstance = new CHarvester(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{

		MSG_BOX("하베스터 원본 생성 실패 ");

		Safe_Release(pInstance);
	}


	return pInstance;
}

CGameObject* CHarvester::Clone(void* pArg)
{
	CHarvester* pInstace = new CHarvester(*this);

	if (FAILED(pInstace->Initialize(pArg)))
	{

		MSG_BOX("하베스터 복제 실패");

		Safe_Release(pInstace);
	}

	return pInstace;
}

void CHarvester::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pLightCom);
	Safe_Release(m_pSoundCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pMaterialCom);
}

