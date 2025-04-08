#include "ShotGun.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "Item_Manager.h"
#include "Image_Manager.h"


CShotGun::CShotGun(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CRanged_Weapon(pGraphic_Device)
{
}

CShotGun::CShotGun(const CShotGun& Prototype)
	: CRanged_Weapon(Prototype)
{
}
HRESULT CShotGun::Initialize_Prototype()
{
	return S_OK;
}
HRESULT CShotGun::Initialize(void* pArg)
{
	if (pArg != nullptr)
		m_Weapon_INFO = *reinterpret_cast<Weapon_DESC*>(pArg);
	else
		return E_FAIL;


	if (FAILED(Ready_Components()))
		return E_FAIL;


	// 일단 Level_GamePlay에서도 비슷한값 넣는중
	m_Weapon_INFO.WeaponID = WEAPON_ID::ShotGun;
	m_Weapon_INFO.vPos = { 0.f,-170.f }; // 샷건 위치
	m_Weapon_INFO.vSize ={ 749,420.f };// 샷건 크기 위에 두개는 일단 밖에서 하는중
	m_Weapon_INFO.Damage = 50;                // 데미지
	m_Weapon_INFO.AttackSpeed = 1.2f;           // 공격 속도 (ex. 초당 발사 가능 횟수)



	m_pTransformCom->Set_Scale(m_Weapon_INFO.vSize.x, m_Weapon_INFO.vSize.y, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_Weapon_INFO.vPos.x, m_Weapon_INFO.vPos.y, 0.f));

	m_vInitialPos = m_Weapon_INFO.vPos;

	//  등록
	CItem_Manager::GetInstance()->Add_Weapon(L"ShotGun", this);

	// 이미지에 따른 그거 
	m_TextureRanges["Idle"] = { 0, 0 };
	m_TextureRanges["Reloading"] = { 3, 16 };
	m_TextureRanges["Firing"] = { 1, 2 };

	Ranged_INFO.CurrentAmmo = 90;
	Ranged_INFO.MaxAmmo = 90;
	m_fAnimationSpeed = 0.03f;


	__super::Ready_Picking();

	return S_OK;
}



void CShotGun::Priority_Update(_float fTimeDelta)
{
}

void CShotGun::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

}
void CShotGun::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_fElapsedTime += fTimeDelta;

	switch (m_eState)
	{
	case State::Firing:
		if (m_fElapsedTime >= m_fAnimationSpeed * (1.f / m_Weapon_INFO.AttackSpeed)) // AttackSpeed 2면 2배라는뜻
		{
			m_fElapsedTime = 0.0f;

			if (!m_bHasFired && m_iCurrentFrame == m_TextureRanges["Firing"].first + 1)
			{
				__super::Picking_Object(5, m_Weapon_INFO.Damage);
				Ranged_INFO.CurrentAmmo--;
				Notify_Bullet();
				m_bHasFired = true;
			}


			if (m_iCurrentFrame < m_TextureRanges["Firing"].second)
				m_iCurrentFrame++;
			else
			{
				m_eState = State::Reloading;
				m_iCurrentFrame = m_TextureRanges["Reloading"].first;
			}
		}
		break;

	case State::Reloading:
		if (m_fElapsedTime >= m_fAnimationSpeed)
		{
			m_fElapsedTime = 0.0f;
			if (m_iCurrentFrame < m_TextureRanges["Reloading"].second)
				m_iCurrentFrame++;
			else
			{
				m_eState = State::Idle;
				m_iCurrentFrame = m_TextureRanges["Idle"].first;
			}
		}
		break;

	case State::Idle:
		m_iCurrentFrame = m_TextureRanges["Idle"].first;
		break;
	}

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
void CShotGun::Attack_WeaponSpecific(_float fTimeDelta)
{
	if (m_eState == State::Idle && Ranged_INFO.CurrentAmmo > 0)
	{
		m_eState = State::Firing;
		m_iCurrentFrame = m_TextureRanges["Firing"].first;
		m_fElapsedTime = 0.f;
		m_bHasFired = false;
	}
}

void CShotGun::Attack(_float fTimeDelta)
{
	__super::Attack(fTimeDelta); 
}




HRESULT CShotGun::Render()
{
	/*if (m_bWall)
		m_pGameInstance->Render_Font(L"MainFont", L"벽 명중!!!", _float2(-200.f, -205.0f), _float3(1.f, 1.f, 0.0f));
	if (m_bMonster)
		m_pGameInstance->Render_Font(L"MainFont", L"몬스터 명중!!!", _float2(200.f, -205.0f), _float3(1.f, 1.f, 0.0f));*/


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

HRESULT CShotGun::On_Collision()
{
	
	return S_OK;
}

HRESULT CShotGun::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC,m_Weapon_INFO.TextureKey,
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

CShotGun* CShotGun::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CShotGun* pInstance = new CShotGun(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{

		MSG_BOX("샷건  원본 생성 실패 ");

		Safe_Release(pInstance);
	}


	return pInstance;
}

CGameObject* CShotGun::Clone(void* pArg)
{
	CShotGun* pInstace = new CShotGun(*this);

	if (FAILED(pInstace->Initialize(pArg)))
	{

		MSG_BOX("샷건  복제 실패");

		Safe_Release(pInstace);
	}

	return pInstace;
}

void CShotGun::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pLightCom);
	Safe_Release(m_pSoundCom);
}


